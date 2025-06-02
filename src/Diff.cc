// Author: Sofiya Nasiakaila

#include "Diff.h"
#include <algorithm>
#include <cassert>
#include <unicode/unistr.h>

using icu::UnicodeString;

std::vector<DiffOp> DiffEngine::compareDocuments(const Document& left_doc, const Document& right_doc) {
    const auto& left_paras = left_doc.paragraphs;
    const auto& right_paras = right_doc.paragraphs;
    int m = left_paras.size();
    int n = right_paras.size();
    // DP table for LCS lengths
    std::vector<int> dp((m + 1) * (n + 1), 0);
    auto idx = [&](int i, int j) -> int& { return dp[i * (n + 1) + j]; };

    // Fill DP table
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (left_paras[i-1].text == right_paras[j-1].text) {
                idx(i, j) = idx(i-1, j-1) + 1;
            } else {
                idx(i, j) = (idx(i-1, j) >= idx(i, j-1)) ? idx(i-1, j) : idx(i, j-1);
            }
        }
    }

    // Backtrack to get diff operations (in reverse order)
    std::vector<DiffOp> rev_ops;
    int i = m, j = n;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && left_paras[i-1].text == right_paras[j-1].text) {
            // Paragraphs are identical -> part of LCS
            DiffOp op;
            op.type = DiffType::Equal;
            op.left_index = i-1;
            op.right_index = j-1;
            op.is_moved = false;
            op.matching_index = -1;
            rev_ops.push_back(op);
            --i;
            --j;
        } else if (j > 0 && (i == 0 || idx(i, j-1) >= idx(i-1, j))) {
            // Paragraph from right not in LCS -> Addition
            DiffOp op;
            op.type = DiffType::Add;
            op.left_index = -1;
            op.right_index = j-1;
            op.is_moved = false;
            op.matching_index = -1;
            rev_ops.push_back(op);
            --j;
        } else if (i > 0 && (j == 0 || idx(i, j-1) < idx(i-1, j))) {
            // Paragraph from left not in LCS -> Removal
            DiffOp op;
            op.type = DiffType::Remove;
            op.left_index = i-1;
            op.right_index = -1;
            op.is_moved = false;
            op.matching_index = -1;
            rev_ops.push_back(op);
            --i;
        } else {
            // This else should not be reached logically
            assert(false);
        }
    }
    // Reverse the operations to get forward order
    std::reverse(rev_ops.begin(), rev_ops.end());

    // At this point, rev_ops is an initial diff (with Equal/Add/Remove). Next, detect moved pairs.
    // We iterate to find identical content in Remove vs Add ops.
    std::vector<DiffOp> ops = rev_ops;  // rename for clarity
    int ops_count = ops.size();
    for (int a = 0; a < ops_count; ++a) {
        if (ops[a].type == DiffType::Remove && !ops[a].is_moved) {
            for (int b = 0; b < ops_count; ++b) {
                if (ops[b].type == DiffType::Add && !ops[b].is_moved) {
                    // Compare content of removed paragraph on left and added paragraph on right
                    const UnicodeString& left_text = left_doc.paragraphs[ops[a].left_index].text;
                    const UnicodeString& right_text = right_doc.paragraphs[ops[b].right_index].text;
                    if (left_text == right_text) {
                        // Mark as moved pair
                        ops[a].is_moved = true;
                        ops[b].is_moved = true;
                        ops[a].matching_index = b;
                        ops[b].matching_index = a;
                        break;
                    }
                }
            }
        }
    }

    // Combine Remove+Add into Modify if they are consecutive (and not moved)
    std::vector<DiffOp> combined;
    combined.reserve(ops.size());
    for (size_t k = 0; k < ops.size(); ++k) {
        if (k + 1 < ops.size() && 
            ops[k].type == DiffType::Remove && !ops[k].is_moved &&
            ops[k+1].type == DiffType::Add && !ops[k+1].is_moved) {
            // Combine into a Modify operation
            DiffOp mod_op;
            mod_op.type = DiffType::Modify;
            mod_op.left_index = ops[k].left_index;
            mod_op.right_index = ops[k+1].right_index;
            mod_op.is_moved = false;
            mod_op.matching_index = -1;
            combined.push_back(mod_op);
            k += 1; // skip the next op as it is combined
        } else {
            // Otherwise, copy the operation as-is
            combined.push_back(ops[k]);
        }
    }

    return combined;
}