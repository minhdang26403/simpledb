#pragma once

#include "query/constant.h"

namespace simpledb {
/**
 * A directory entry has two components: the block number of the child block and
 * the key of the first record in the subtree rooted at the child block
 */
class DirectoryEntry {
 public:
  /**
   * @brief Create a new entry for the specified key and block number
   * @param key the key component of the entry
   * @param block_num the block number component of the entry
   */
  DirectoryEntry(const Constant& key, int block_num)
      : key_(key), block_num_(block_num) {}

  /**
   * @brief Return the key component of the entry
   * @return the key component of the entry
   */
  Constant Key() const noexcept { return key_; }

  /**
   * @brief Return the block number component of the entry
   * @return the block number component of the entry
   */
  int BlockNumber() const noexcept { return block_num_; }

 private:
  Constant key_;
  int block_num_;
};
}  // namespace simpledb
