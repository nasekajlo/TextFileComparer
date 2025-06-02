// Author: Sofiya Nasiakaila

#include "Merger.h"
#include <fstream>
#include <boost/throw_exception.hpp>

Merger::Merger(const Document& left_doc, const Document& right_doc, std::vector<DiffOp> diff_ops)
    : left_doc_(left_doc), right_doc_(right_doc), diffs_(std::move(diff_ops)) {
    // Initialize all changes as not accepted (false)
    accepted_.resize(diffs_.size(), false);
}

void Merger::acceptChange(size_t index) {
    if (index >= diffs_.size()) return;
    // If this change is already accepted or is an Equal (no change), ignore
    if (diffs_[index].type == DiffType::Equal || accepted_[index]) {
        return;
    }
    // Accept this change
    accepted_[index] = true;
    // If it's part of a moved pair, accept the matching operation as well
    if (diffs_[index].is_moved) {
        int j = diffs_[index].matching_index;
        if (j >= 0 && j < (int)accepted_.size()) {
            accepted_[j] = true;
        }
    }
}

Document Merger::produceMergedDocument() const {
    Document result;
    // Iterate through diff operations in order
    for (size_t idx = 0; idx < diffs_.size(); ++idx) {
        const DiffOp& op = diffs_[idx];
        switch (op.type) {
        case DiffType::Equal:
            // Include unchanged paragraph (from left or right - they are identical)
            result.paragraphs.push_back(left_doc_.paragraphs[op.left_index]);
            break;
        case DiffType::Modify:
            if (accepted_[idx]) {
                // Use the paragraph from the right document
                result.paragraphs.push_back(right_doc_.paragraphs[op.right_index]);
            } else {
                // Keep the original from the left document
                result.paragraphs.push_back(left_doc_.paragraphs[op.left_index]);
            }
            break;
        case DiffType::Remove:
            if (accepted_[idx]) {
                // Removal accepted: omit the left paragraph (do nothing)
            } else {
                // Not accepted: keep the left paragraph
                result.paragraphs.push_back(left_doc_.paragraphs[op.left_index]);
            }
            break;
        case DiffType::Add:
            if (accepted_[idx]) {
                // Addition accepted: include the new paragraph from right
                result.paragraphs.push_back(right_doc_.paragraphs[op.right_index]);
            } else {
                // Not accepted: skip this paragraph
            }
            break;
        }
    }
    return result;
}

bool Merger::saveMerged(const std::string& file_path) const {
    // Produce merged document and write to file
    Document merged = produceMergedDocument();
    std::ofstream out(file_path, std::ios::binary);
    if (!out.is_open()) {
        BOOST_THROW_EXCEPTION(FileSaveException() << boost::errinfo_file_name(file_path));
    }
    // Write each paragraph followed by a blank line
    for (size_t pi = 0; pi < merged.paragraphs.size(); ++pi) {
        // Convert UnicodeString paragraph to UTF-8
        std::string utf8;
        merged.paragraphs[pi].text.toUTF8String(utf8);
        out << utf8;
        if (pi + 1 < merged.paragraphs.size()) {
            out << "\n\n"; // blank line separator
        } else {
            out << "\n";   // end of file newline
        }
    }
    out.close();
    return true;
}