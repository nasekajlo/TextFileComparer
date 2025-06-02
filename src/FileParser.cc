// Author: Sofiya Nasiakaila

#include "FileParser.h"
#include <fstream>
#include <sstream>
#include <boost/scoped_ptr.hpp>
#include <boost/throw_exception.hpp>
#include <unicode/brkiter.h>
#include <unicode/normlzr.h>

using namespace icu;

namespace {
    // Helper: escape text for HTML (for potential use in UI or tests)
    std::string escapeForHtml(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for (char c : s) {
            switch(c) {
                case '&': out += "&amp;"; break;
                case '<': out += "&lt;"; break;
                case '>': out += "&gt;"; break;
                case '\"': out += "&quot;"; break;
                default: out += c;
            }
        }
        return out;
    }

    // Helper: segment a UnicodeString paragraph into word tokens using ICU.
    std::vector<UnicodeString> segmentWords(const UnicodeString& paragraph) {
        std::vector<UnicodeString> tokens;
        UErrorCode status = U_ZERO_ERROR;
        // Create word boundary iterator for Polish locale (or default if not available).
        boost::scoped_ptr<BreakIterator> word_iter(BreakIterator::createWordInstance(Locale("pl"), status));
        if (U_FAILURE(status) || !word_iter) {
            // Fallback: treat entire paragraph as one token if ICU failed
            tokens.push_back(paragraph);
            return tokens;
        }
        word_iter->setText(paragraph);
        int32_t start = word_iter->first();
        for (int32_t end = word_iter->next(); end != BreakIterator::DONE; start = end, end = word_iter->next()) {
            UnicodeString word = paragraph.tempSubStringBetween(start, end);
            // Trim whitespace-only tokens
            UnicodeString trimmed = word;
            trimmed.trim();
            if (trimmed.isEmpty()) continue;
            tokens.push_back(word);
        }
        return tokens;
    }
}

Document FileParser::parseFile(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        // Attach file name to exception and throw
        BOOST_THROW_EXCEPTION(FileOpenException() << boost::errinfo_file_name(file_path));
    }
    // Read entire file into a string
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    file.close();
    return FileParser::parseText(content, file_path);
}

Document FileParser::parseText(const std::string& text, const std::string& name) {
    Document doc;
    doc.name = name;
    // Convert UTF-8 text to ICU UnicodeString
    UnicodeString utext = UnicodeString::fromUTF8(text);
    // Normalize to NFC to avoid spurious differences due to accent encoding
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString utext_norm;
    Normalizer::normalize(utext, UNORM_NFC, 0, utext_norm, status);
    if (U_SUCCESS(status)) {
        utext = utext_norm;
    }
    // Iterate through text to split into paragraphs
    int32_t length = utext.length();
    UnicodeString current_para;
    for (int32_t i = 0; i < length; ) {
        // Find end of line or end of text
        int32_t line_end = utext.indexOf((UChar)'\n', i);
        if (line_end < 0) line_end = length;
        UnicodeString line = utext.tempSubStringBetween(i, line_end);
        i = line_end + 1; // move past the newline
        if (line.isEmpty()) {
            // Blank line -> paragraph break
            if (!current_para.isEmpty()) {
                current_para.trim(); // remove any trailing whitespace
                Paragraph para;
                para.text = current_para;
                para.words = segmentWords(current_para);
                doc.paragraphs.push_back(std::move(para));
                current_para.remove(); // clear current paragraph accumulator
            }
            // If current_para was already empty, it means multiple blank lines - skip extras
        } else {
            // Non-empty line -> append to current paragraph (with space if paragraph already has content)
            if (!current_para.isEmpty()) {
                current_para.append((UChar)' ');
            }
            current_para.append(line);
        }
    }
    // Add the last paragraph if any content remains
    if (!current_para.isEmpty()) {
        current_para.trim();
        Paragraph para;
        para.text = current_para;
        para.words = segmentWords(current_para);
        doc.paragraphs.push_back(std::move(para));
    }
    return doc;
}