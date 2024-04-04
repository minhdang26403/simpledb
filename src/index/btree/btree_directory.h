#pragma once

#include <memory>
#include <optional>
#include <string>

#include "file/block_id.h"
#include "index/btree/btree_page.h"
#include "index/btree/directory_entry.h"
#include "record/layout.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * A B-tree directory block
 */
class BTreeDirectory {
 public:
  /**
   * @brief Create an object to hold the contents of the specified B-tree block
   * @param txn the calling transaction
   * @param block a reference to the specified B-tree block
   * @param layout the metadata of the B-tree directory file
   */
  BTreeDirectory(Transaction& txn, const BlockId& block, Layout& layout)
      : txn_(txn),
        layout_(layout),
        contents_(std::make_unique<BTreePage>(txn_, block, layout_)),
        filename_(block.Filename()) {}

  /**
   * @brief Close the directory page
   */
  void Close();

  /**
   * @brief Return the block number of the B-tree leaf block that contains the
   * specified search key
   * @param search_key the search key value
   * @return the block number of the leaf block containing that search key
   */
  int Search(const Constant& search_key);

  /**
   * @brief Create a new root block for the B-tree.
   * The new root will have two children: the old root and the specified block.
   * Since the root must always be in block 0 of the file, the contents of the
   * old root will get transferred to a new block.
   * @param entry the directory entry to be added as a child of the new root
   */
  void MakeNewRoot(const DirectoryEntry& entry);

  /**
   * @brief Insert a new directory entry into the B-tree block. If the block is
   * at level 0, then the entry is inserted there. Otherwise, the entry is
   * inserted into the appropriate child node, and the return value is examined.
   * A non-null return value indicates that the child node split, and so the
   * returned entry is inserted into this block. If this block splits, then the
   * method similarly returns the entry information of the new block to its
   * caller; otherwise, the method returns null.
   * @param entry the directory entry to be inserted
   * @return the directory entry of the newly-split block, if one exists;
   * otherwise, null
   */
  std::optional<DirectoryEntry> Insert(const DirectoryEntry& entry);

 private:
  std::optional<DirectoryEntry> InsertEntry(const DirectoryEntry& entry);

  BlockId FindChildBlock(const Constant& search_key);

  Transaction& txn_;
  Layout& layout_;
  std::unique_ptr<BTreePage> contents_;
  std::string filename_;
};
}  // namespace simpledb
