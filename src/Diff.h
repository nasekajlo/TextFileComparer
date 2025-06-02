// Author: Sofiya Nasiakaila

#ifndef DIFF_H
#define DIFF_H

#include <vector>
#include "Document.h"

/** @brief Type of a diff operation for paragraphs. */
enum class DiffType { Equal, Remove, Add, Modify };

/** 
 * @brief Represents a single diff operation (at paragraph level) between two documents.
 */
struct DiffOp {
    DiffType type;         ///< Type of difference.
    int left_index;         ///< Paragraph index in left document (valid if type is not Add).
    int right_index;        ///< Paragraph index in right document (valid if type is not Remove).
    bool is_moved;          ///< True if this operation is part of a moved paragraph pair.
    int matching_index;     ///< Index of the matching operation (for moved pairs), or -1 if none.
};

/** @brief Comparison and matching engine for documents. */
class DiffEngine {
public:
    /**
     * @brief Compare two documents and produce a list of diff operations.
     * @param left_doc   The original Document (left side).
     * @param right_doc  The modified Document (right side).
     * @return Sequence of DiffOp representing differences in order.
     */
    static std::vector<DiffOp> compareDocuments(const Document& left_doc, const Document& right_doc);
};

#endif
