#pragma once

#include <optional>

#include "file/block_id.h"
#include "file/file_manager.h"
#include "file/page.h"
#include "log/log_manager.h"

namespace simpledb {
/**
 * An individual buffer. A data buffer wraps a page and stores information about
 * its status, such as the associated disk block, the number of times the buffer
 * has been pinned, whether its contents have modified, and if so, the id and
 * lsn of the modifying transaction.
 */
class Buffer {
 public:
  /**
   * @brief Construct a new Buffer object
   * @param file_manager file manager of the database engine
   * @param log_manager log manager of the database engine
   */
  Buffer(FileManager& file_manager, LogManager& log_manager)
      : file_manager_(file_manager),
        log_manager_(log_manager),
        contents_(file_manager.BlockSize()) {}

  /**
   * @brief Retrieve the in-memory page version of the disk block that this
   * buffer holds
   * @return a reference to the page
   */
  Page& Contents() noexcept { return contents_; }

  /**
   * @brief Return a `BlockId` object that represents a reference to the disk
   * block allocated to the buffer
   * @return an optional `BlockId` object (this buffer may or may not be
   * pinned to any disk block)
   */
  std::optional<BlockId> Block() const noexcept { return block_opt_; }

  /**
   * @brief Set the transaction id and the log sequence number to indicate
   * that the page that this buffer holds has been modified
   * @param txn_id transaction id
   * @param lsn log sequence number
   */
  void SetModified(int txn_id, int lsn) noexcept;

  /**
   * @brief Return whether the buffer is currently pinned
   * @return true if the buffer is pinned; otherwise, false
   */
  bool IsPinned() const noexcept { return pin_count_ > 0; }

  /**
   * @brief Get the id of the transaction that modifies this page
   * @return transaction id
   */
  int ModifyingTxn() const noexcept { return txn_id_; }

  /**
   * @brief Read the contents of the specified block into the contents of the
   * buffer. If the buffer was dirty, then its previous contents are first
   * written to disk.
   * @param block a reference to some disk block
   */
  void AssignToBlock(const BlockId& block);

  /**
   * @brief Write the buffer to its disk block if it is dirty
   */
  void Flush();

  /**
   * @brief Pin a page, indicating that it should not be used for other disk
   * blocks
   */
  void Pin() noexcept { pin_count_++; }

  /**
   * @brief Unpin a page, indicating that it can now be used to hold other disk
   * blocks
   */
  void Unpin() noexcept { pin_count_--; }

 private:
  FileManager& file_manager_;
  LogManager& log_manager_;
  Page contents_;
  std::optional<BlockId> block_opt_;
  int pin_count_{};
  int txn_id_{-1};
  int lsn_{-1};
};
}  // namespace simpledb
