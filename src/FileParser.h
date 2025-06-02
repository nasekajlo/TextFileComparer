// Author: Sofiya Nasiakaila

#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include <stdexcept>
#include <boost/exception/all.hpp>
#include "Document.h"

/** 
 * @brief Exception thrown when a file cannot be opened. 
 * Carries the file path in boost::errinfo_file_name.
 */
struct FileOpenException : virtual std::exception, virtual boost::exception {
    const char* what() const noexcept override {
        return "Failed to open file.";
    }
};

/**
 * @brief FileParser module: loads text from files or strings and produces a Document.
 */
class FileParser {
public:
    /**
     * @brief Load text from a file and parse into a Document.
     * @param file_path Path to the text file (UTF-8).
     * @return Document containing paragraphs and words.
     * @throws FileOpenException if the file cannot be opened.
     */
    static Document parseFile(const std::string& file_path);

    /**
     * @brief Parse text from a UTF-8 string into a Document.
     * @param text     Input text.
     * @param name     Optional name for the Document.
     * @return Document with paragraphs and word tokens.
     */
    static Document parseText(const std::string& text, const std::string& name = "");
};

#endif
