// Author: Sofiya Nasiakaila

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include <vector>
#include <unicode/unistr.h>

/**
 * @brief Represents a paragraph of text, with its content and segmented words.
 */
struct Paragraph {
    icu::UnicodeString text;                 ///< Full text of the paragraph (Unicode).
    std::vector<icu::UnicodeString> words;   ///< Words (and punctuation tokens) in the paragraph.
};

/**
 * @brief Represents a text document as a sequence of paragraphs.
 */
struct Document {
    std::vector<Paragraph> paragraphs;       ///< All paragraphs in the document.
    std::string name;                        ///< Optional name or file path of the document.
};

#endif
