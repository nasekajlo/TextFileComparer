// Author: Sofiya Nasiakaila

#ifndef MERGER_H
#define MERGER_H

#include <vector>
#include <string>
#include <boost/exception/all.hpp>
#include "Document.h"
#include "Diff.h"

/** 
 * @brief Exception thrown when saving the merged file fails. 
 * Carries the output file path.
 */
struct FileSaveException : virtual std::exception, virtual boost::exception {
    const char* what() const noexcept override {
        return "Failed to save file.";
    }
};

/**
 * @brief Handles merging of two documents based on accepted changes.
 */
class Merger {
public:
    Merger(const Document& left_doc, const Document& right_doc, std::vector<DiffOp> diff_ops);

    /** @brief Mark a change (by index in diff list) as accepted. 
     * If part of a moved pair, the counterpart is also accepted.
     */
    void acceptChange(size_t index);

    /** @brief Generate the merged document after applying accepted changes. */
    Document produceMergedDocument() const;

    /** 
     * @brief Save the merged document to a file (UTF-8).
     * @param file_path Path to save the merged text.
     * @return true if saved successfully.
     * @throws FileSaveException if the file cannot be written.
     */
    bool saveMerged(const std::string& file_path) const;

    /** @brief Get read-only access to the diff operations list. */
    const std::vector<DiffOp>& getDiffs() const { return diffs_; }

private:
    const Document& left_doc_;
    const Document& right_doc_;
    std::vector<DiffOp> diffs_;
    std::vector<bool> accepted_;  ///< Parallel to diffs_: true if change is accepted.
};

#endif