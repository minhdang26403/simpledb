#pragma once

#include <cmath>
#include <memory>
#include <string>

#include "index/btree/btree_leaf.h"
#include "index/btree/btree_page.h"
#include "index/index.h"
#include "record/layout.h"
#include "record/rid.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * A B-tree implementation of the `Index` interface
 */
class BTreeIndex : public Index {
 public:
  /**
   * @brief Open a B-tree index for the specified index. The method determines
   * the appropriate files for the leaf and directory records, creating them if
   * they did not exist.
   * @param txn the calling transaction
   * @param index_name the name of the index
   * @param leaf_layout the schema of the index leaf index records
   */
  BTreeIndex(Transaction& txn, std::string_view index_name,
             const Layout& leaf_layout);

  /**
   * @brief Traverse the directory to find the leaf block corresponding to the
   * specified search key. The method then opens a page for that leaf block and
   * positions the page before the first record (if any) having that search key.
   * The leaf page is kept open, for use by the methods `Next` and `GetRID`
   * @param search_key the search key value
   */
  void BeforeFirst(const Constant& search_key);

  /**
   * @brief Move to the next leaf record having the previously-specified search
   * key
   * @return false if there are no more such index records
   */
  bool Next();

  /**
   * @brief Return the RID value from the current leaf record
   * @return the RID value from the current leaf record
   */
  RID GetRID();

  /**
   * @brief Insert the specified record into the index. The method first
   * traverses the directory to find the appropriate leaf page; then it inserts
   * the record into the leaf. If the insertion causes the leaf to split, then
   * the method calls `Insert` on the root, passing it the directory entry of
   * the new leaf page. If the root node splits, then `MakeNewRoot` is called.
   * @param key the key of the new index record
   * @param rid the RID of the new index record
   */
  void Insert(const Constant& key, const RID& rid);

  /**
   * @brief Delete the specified index record. The method first traverses the
   * directory to find the leaf page containing that record; then it deletes the
   * record from the page.
   * @param key the key of the deleted index record
   * @param rid the RID of the deleted index record
   */
  void Delete(const Constant& key, const RID& rid);

  /**
   * @brief Close the index by closing its open leaf page, if necessary
   */
  void Close();

  /**
   * @brief Estimate the number of block accesses required to find all index
   * records having a particular search key
   * @param num_blocks the number of blocks in the B-tree directory
   * @param records_per_block the number of index entries per block
   * @return the estimated traversal cost
   */
  static int SearchCost(int num_blocks, int records_per_block) noexcept {
    return 1 + std::log(num_blocks) / std::log(records_per_block);
  }

 private:
  Transaction& txn_;
  Layout dir_layout_;
  Layout leaf_layout_;
  std::string leaf_table_;
  std::unique_ptr<BTreeLeaf> leaf_;
  BlockId root_block_;
};
}  // namespace simpledb
