#pragma once

#include <span>

#include "file/block_id.h"
#include "file/file_manager.h"
#include "file/page.h"

namespace simpledb {
/**
 * A class that provides the ability to move through the records of the log file
 * in reverse order (from most recent to least recent log records).
 */
class LogIterator {
 public:
  /**
   * @brief Create an iterator for the records in the log file, positioned after
   * the last log record
   * @param file_manager file manager of the database engine
   * @param block the disk block that this iterator traverses on
   */
  LogIterator(FileManager& file_manager, const BlockId& block);

  /**
   * @brief Determine if the current log record is the earliest record in the
   * log file
   * @return true if there is an earlier record; otherwise, false
   */
  bool HasNext() const noexcept {
    return current_pos_ < file_manager_.BlockSize() || block_.BlockNumber() > 0;
  }

  /**
   * @brief Move to the next log record in the block. If there are no more log
   * records in the block, then move to the previous block and return the log
   * record from there.
   * @return the next earliest log record
   */
  std::span<char> Next() noexcept;

 private:
  /**
   * @brief Move to the specified log block and position it at the first record
   * in that block (i.e., the most recent one)
   * @param block the block to move to
   */
  void MoveToBlock(const BlockId& block);

  FileManager& file_manager_;
  BlockId block_;
  Page page_;
  int current_pos_{};
};
}  // namespace simpledb
