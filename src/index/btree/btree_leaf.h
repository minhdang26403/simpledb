#pragma once

#include <memory>
#include <optional>
#include <string>

#include "index/btree/btree_page.h"
#include "index/btree/directory_entry.h"
#include "query/constant.h"
#include "record/layout.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * An object that holds the contents of a B-tree leaf block
 */
class BTreeLeaf {
 public:
  /**
   * @brief Open a buffer to hold the specified leaf block. The buffer is
   * positioned immediately before the first record having the specified search
   * key (if any).
   * @param txn the calling transaction
   * @param block a reference to the disk block
   * @param layout the metadata of the B-tree leaf file
   * @param search_key the search key value
   */
  BTreeLeaf(Transaction& txn, const BlockId& block, Layout& layout,
            const Constant& search_key)
      : txn_(txn),
        layout_(layout),
        search_key_(search_key),
        contents_(std::make_unique<BTreePage>(txn_, block, layout_)),
        current_slot_(contents_->FindSlotBefore(search_key_)),
        filename_(block.Filename()) {}

  /**
   * @brief Close the leaf page
   */
  void Close();

  /**
   * @brief Move to the next leaf record having the previously-specified search
   * key. Return false if there is no more such records
   * @return false if there are no more leaf records for the search key
   */
  bool Next();

  /**
   * @brief Return the RID value of the current leaf record
   * @return the RID of the current record
   */
  RID GetRID();

  /**
   * @brief Delete the leaf record having the specified RID
   * @param rid the RID whose record is to be deleted
   */
  void Delete(const RID& rid);

  /**
   * Inserts a new leaf record having the specified RID and the
   * previously-specified search key. If the record does not fit in the page,
   * then the page splits and the method returns the directory entry for the new
   * page; otherwise, the method returns null. If all of the records in the page
   * have the same key, then the block does not split; instead, all but one
   * of the records are placed into an overflow block.
   * @param rid the RID value of the new record
   * @return the directory entry of the newly-split page, if one exists.
   */
  std::optional<DirectoryEntry> Insert(const RID& rid);

 private:
  bool TryOverflow();

  Transaction& txn_;
  Layout& layout_;
  Constant search_key_;
  std::unique_ptr<BTreePage> contents_;
  int current_slot_;
  std::string filename_;
};
}  // namespace simpledb
