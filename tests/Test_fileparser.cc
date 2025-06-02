// Author: Sofiya Nasiakaila

#include <gtest/gtest.h>
#include "FileParser.h"

TEST(FileParserTest, ParagraphSplitAndLineJoin) {
    std::string text = "Line1\nLine2\n\nLine3\n";
    Document doc = FileParser::parseText(text);
    // Should be 2 paragraphs: "Line1 Line2" and "Line3"
    ASSERT_EQ(doc.paragraphs.size(), 2u);
    std::string para0, para1;
    doc.paragraphs[0].text.toUTF8String(para0);
    doc.paragraphs[1].text.toUTF8String(para1);
    EXPECT_EQ(para0, "Line1 Line2");
    EXPECT_EQ(para1, "Line3");
}

TEST(FileParserTest, WordSegmentationBasic) {
    std::string text = "Zażółć gęślą jaźń.\nNew-line test.\n";
    Document doc = FileParser::parseText(text);
    ASSERT_EQ(doc.paragraphs.size(), 1u);
    const Paragraph& para = doc.paragraphs[0];
    // Expect words including punctuation as separate tokens
    // "Zażółć", "gęślą", "jaźń", "." , "New", "-", "line", "test", "."
    std::vector<std::string> expected_tokens = {
        "Zażółć", "gęślą", "jaźń", ".", "New", "-", "line", "test", "."
    };
    std::vector<std::string> actualTokens;
    for (auto &token : para.words) {
        std::string utf8;
        token.toUTF8String(utf8);
        actualTokens.push_back(utf8);
    }
    ASSERT_EQ(actualTokens.size(), expected_tokens.size());
    for (size_t i = 0; i < expected_tokens.size(); ++i) {
        EXPECT_EQ(actualTokens[i], expected_tokens[i]);
    }
}

TEST(FileParserTest, LineBreakDifferenceIgnored) {
    // Same text content, but one has an extra newline
    std::string text1 = "ABC DEF\nGHI\n";
    std::string text2 = "ABC DEF GHI\n";
    Document doc1 = FileParser::parseText(text1);
    Document doc2 = FileParser::parseText(text2);
    // Both should result in a single paragraph "ABC DEF GHI"
    ASSERT_EQ(doc1.paragraphs.size(), 1u);
    ASSERT_EQ(doc2.paragraphs.size(), 1u);
    std::string para1, para2;
    doc1.paragraphs[0].text.toUTF8String(para1);
    doc2.paragraphs[0].text.toUTF8String(para2);
    EXPECT_EQ(para1, "ABC DEF GHI");
    EXPECT_EQ(para2, "ABC DEF GHI");
}