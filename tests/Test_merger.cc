// Author: Sofiya Nasiakaila

#include <gtest/gtest.h>
#include "FileParser.h"
#include "Diff.h"
#include "Merger.h"

TEST(MergerTest, NoChangesResultsInleft_doc) {
    std::string text = "X\n\nY\n";
    Document left = FileParser::parseText(text);
    Document right = FileParser::parseText(text);
    auto ops = DiffEngine::compareDocuments(left, right);
    Merger merger(left, right, ops);
    Document merged = merger.produceMergedDocument();
    // Merged should equal left (and right) since no diffs
    ASSERT_EQ(merged.paragraphs.size(), left.paragraphs.size());
    for (size_t i = 0; i < left.paragraphs.size(); ++i) {
        std::string orig, out;
        left.paragraphs[i].text.toUTF8String(orig);
        merged.paragraphs[i].text.toUTF8String(out);
        EXPECT_EQ(out, orig);
    }
}

TEST(MergerTest, AcceptAddAndRemove) {
    std::string left_text = "Alpha\n";
    std::string right_text = "Alpha\nBeta\n";
    Document left = FileParser::parseText(left_text);
    Document right = FileParser::parseText(right_text);
    auto ops = DiffEngine::compareDocuments(left, right);
    Merger merger(left, right, ops);
    // Initially, merged should be left (Beta not included)
    Document merged_initial = merger.produceMergedDocument();
    ASSERT_EQ(merged_initial.paragraphs.size(), 1u);
    std::string para;
    merged_initial.paragraphs[0].text.toUTF8String(para);
    EXPECT_EQ(para, "Alpha");
    // Accept the addition of "Beta"
    // Find the diff index for Add
    for (size_t i = 0; i < ops.size(); ++i) {
        if (ops[i].type == DiffType::Add) {
            merger.acceptChange(i);
        }
    }
    Document merged_after = merger.produceMergedDocument();
    // Now merged should have Alpha and Beta
    ASSERT_EQ(merged_after.paragraphs.size(), 2u);
    std::string out0, out1;
    merged_after.paragraphs[0].text.toUTF8String(out0);
    merged_after.paragraphs[1].text.toUTF8String(out1);
    EXPECT_EQ(out0, "Alpha");
    EXPECT_EQ(out1, "Beta");
    // Test removal: remove Beta from right back to left
    auto ops2 = DiffEngine::compareDocuments(right, left);
    Merger merger2(right, left, ops2);
    // Accept removal (Beta removal operation)
    for (size_t i = 0; i < ops2.size(); ++i) {
        if (ops2[i].type == DiffType::Remove) {
            merger2.acceptChange(i);
        }
    }
    Document merged2 = merger2.produceMergedDocument();
    // Expect Beta removed, result should equal "Alpha"
    ASSERT_EQ(merged2.paragraphs.size(), 1u);
    std::string only;
    merged2.paragraphs[0].text.toUTF8String(only);
    EXPECT_EQ(only, "Alpha");
}

TEST(MergerTest, AcceptModification) {
    std::string left_text = "Hello A\n";
    std::string right_text = "Hello B\n";
    Document left = FileParser::parseText(left_text);
    Document right = FileParser::parseText(right_text);
    auto ops = DiffEngine::compareDocuments(left, right);
    Merger merger(left, right, ops);
    // Not accepting modify -> output should be left_text
    Document out1 = merger.produceMergedDocument();
    ASSERT_EQ(out1.paragraphs.size(), 1u);
    std::string out_str1;
    out1.paragraphs[0].text.toUTF8String(out_str1);
    EXPECT_EQ(out_str1, "Hello A");
    // Accept the modification
    for (size_t i = 0; i < ops.size(); ++i) {
        if (ops[i].type == DiffType::Modify) {
            merger.acceptChange(i);
        }
    }
    Document out2 = merger.produceMergedDocument();
    ASSERT_EQ(out2.paragraphs.size(), 1u);
    std::string out_str2;
    out2.paragraphs[0].text.toUTF8String(out_str2);
    EXPECT_EQ(out_str2, "Hello B");
}

TEST(MergerTest, AcceptMove) {
    // Left: D, E; Right: E, D (swapped, which will be move scenario)
    std::string left_text = "D\n\nE\n";
    std::string right_text = "E\n\nD\n";
    Document left = FileParser::parseText(left_text);
    Document right = FileParser::parseText(right_text);
    auto ops = DiffEngine::compareDocuments(left, right);
    Merger merger(left, right, ops);
    // Case1: reject move (do nothing) -> merged should equal left content
    Document result1 = merger.produceMergedDocument();
    std::string r10, r11;
    ASSERT_EQ(result1.paragraphs.size(), 2u);
    result1.paragraphs[0].text.toUTF8String(r10);
    result1.paragraphs[1].text.toUTF8String(r11);
    EXPECT_EQ(r10, "D");
    EXPECT_EQ(r11, "E");
    // Case2: accept move -> merged should equal right content order
    // Accept all non-equal changes (the move add/remove)
    for (size_t i = 0; i < ops.size(); ++i) {
        if (ops[i].type != DiffType::Equal) {
            merger.acceptChange(i);
        }
    }
    Document result2 = merger.produceMergedDocument();
    std::string r20, r21;
    ASSERT_EQ(result2.paragraphs.size(), 2u);
    result2.paragraphs[0].text.toUTF8String(r20);
    result2.paragraphs[1].text.toUTF8String(r21);
    // After accepting move, order should be E, D (right's order)
    EXPECT_EQ(r20, "E");
    EXPECT_EQ(r21, "D");
}