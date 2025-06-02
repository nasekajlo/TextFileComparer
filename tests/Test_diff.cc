// Author: Sofiya Nasiakaila

#include <gtest/gtest.h>
#include "FileParser.h"
#include "Diff.h"

TEST(DiffEngineTest, IdenticalDocuments) {
    std::string text = "Paragraph1\n\nParagraph2\n";
    Document doc1 = FileParser::parseText(text);
    Document doc2 = FileParser::parseText(text);
    auto ops = DiffEngine::compareDocuments(doc1, doc2);
    // Expect only Equal operations for each paragraph
    ASSERT_EQ(ops.size(), 2u);
    EXPECT_EQ(ops[0].type, DiffType::Equal);
    EXPECT_EQ(ops[1].type, DiffType::Equal);
    // Check indices align
    EXPECT_EQ(ops[0].left_index, 0);
    EXPECT_EQ(ops[0].right_index, 0);
    EXPECT_EQ(ops[1].left_index, 1);
    EXPECT_EQ(ops[1].right_index, 1);
}

TEST(DiffEngineTest, AdditionAndRemoval) {
    std::string left_text = "LineA\nLineB\n";
    std::string right_text = "LineA\nLineB\nLineC\n";
    Document left_doc = FileParser::parseText(left_text);
    Document right_doc = FileParser::parseText(right_text);
    auto ops = DiffEngine::compareDocuments(left_doc, right_doc);
    // Expect: Equal(LineA), Equal(LineB), Add(LineC)
    ASSERT_EQ(ops.size(), 3u);
    EXPECT_EQ(ops[0].type, DiffType::Equal);
    EXPECT_EQ(ops[1].type, DiffType::Equal);
    EXPECT_EQ(ops[2].type, DiffType::Add);
    // Add should reference right_doc index 2 (LineC)
    EXPECT_EQ(ops[2].right_index, 2);
    // Removal test: remove LineC from right text to left
    auto ops2 = DiffEngine::compareDocuments(right_doc, left_doc);
    // Expect: Equal(LineA), Equal(LineB), Remove(LineC)
    ASSERT_EQ(ops2.size(), 3u);
    EXPECT_EQ(ops2[2].type, DiffType::Remove);
    EXPECT_EQ(ops2[2].left_index, 2);  // index of LineC in right_doc as left in this comparison
}

TEST(DiffEngineTest, ModificationCombined) {
    // One paragraph slightly changed
    std::string text1 = "Hello world\n";
    std::string text2 = "Hello brave world\n";
    Document doc1 = FileParser::parseText(text1);
    Document doc2 = FileParser::parseText(text2);
    auto ops = DiffEngine::compareDocuments(doc1, doc2);
    // Should be one Modify operation (since both docs have one paragraph that differs)
    ASSERT_EQ(ops.size(), 1u);
    EXPECT_EQ(ops[0].type, DiffType::Modify);
    // Indices should map first paragraph of each
    EXPECT_EQ(ops[0].left_index, 0);
    EXPECT_EQ(ops[0].right_index, 0);
    // Ensure not incorrectly marked as moved
    EXPECT_FALSE(ops[0].is_moved);
}

TEST(DiffEngineTest, MovedParagraphsDetected) {
    // File1: A, B, C; File2: B, A, C (A and B swapped)
    std::string text_a = "A\n\nB\n\nC\n";
    std::string text_b = "B\n\nA\n\nC\n";
    Document doc_a = FileParser::parseText(text_a);
    Document doc_b = FileParser::parseText(text_b);
    auto ops = DiffEngine::compareDocuments(doc_a, doc_b);
    // We expect: one of A or B will be aligned as Equal (LCS of length 2), and the other will be marked moved.
    // Thus ops likely: Equal(?), Remove(?), Add(?), Equal(C). Identify moved pair:
    int moved_count = 0;
    int remove_index = -1, add_index = -1;
    for (size_t i = 0; i < ops.size(); ++i) {
        if (ops[i].is_moved) {
            moved_count++;
            if (ops[i].type == DiffType::Remove) remove_index = i;
            if (ops[i].type == DiffType::Add) add_index = i;
        }
    }
    // There should be exactly 2 moved ops (one remove, one add)
    EXPECT_EQ(moved_count, 2);
    ASSERT_NE(remove_index, -1);
    ASSERT_NE(add_index, -1);
    // The moved remove and add should reference identical text
    int li = ops[remove_index].left_index;
    int rj = ops[add_index].right_index;
    std::string moved_left, moved_right;
    doc_a.paragraphs[li].text.toUTF8String(moved_left);
    doc_b.paragraphs[rj].text.toUTF8String(moved_right);
    EXPECT_EQ(moved_left, moved_right);
    // Also check matching indices
    EXPECT_EQ(ops[remove_index].matching_index, add_index);
    EXPECT_EQ(ops[add_index].matching_index, remove_index);
}