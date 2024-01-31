#pragma once

#include <string_view>

#include "buffer/buffer_manager.h"
#include "file/block_id.h"
#include "file/file_manager.h"
#include "log/log_manager.h"
#include "txn/buffer_list.h"
#include "txn/concurrency/concurrency_manager.h"
#include "txn/recovery/recovery_manager.h"

namespace simpledb {
/**
 * Provide transaction management for clients, ensuring that all transactions
 * are serializable, recoverable, and in general satisfy the ACID properties
 */
class Transaction {
 public:
  /**
   * @brief Create a new transaction and its associated recovery and concurrency
   * managers. This constructor depends on the file, log, amd buffer managers
   * that it gets from the SimpleDB server instance.
   * @param file_manager file manager of the database engine
   * @param log_manager log manager of the database engine
   * @param buffer_manager buffer manager of the database engine
   */
  Transaction(FileManager& file_manager, LogManager& log_manager,
              BufferManager& buffer_manager)
      : file_manager_(file_manager),
        buffer_manager_(buffer_manager),
        txn_id_(NextTxnId()),
        my_buffers_(buffer_manager),
        recovery_manager_(*this, txn_id_, log_manager, buffer_manager) {}

  /**
   * Commit the current transaction. Flush all modified buffers (and their log
   * records), write and flush a commit record to the log, release all locks,
   * and unpin any pinned buffers.
   */
  void Commit();

  /**
   * Rollback the current transaction. Undo any modified values,
   * flush those buffers, write and flush a rollback record to the log, release
   * all locks, and unpin any pinned buffers.
   */
  void Rollback();

  /**
   * Flush all modified buffers. Then go through the log, rolling back all
   * uncommitted transactions. Finally, write a quiescent checkpoint record to
   * the log. This method is called during system startup, before user
   * transactions begin.
   */
  void Recover();

  /**
   * Pin the specified block. The transaction manages the buffer for the client.
   * @param block a reference to the disk block
   */
  void Pin(const BlockId& block) { my_buffers_.Pin(block); }

  /**
   * Unpin the specified block. The transaction looks up the buffer pinned to
   * this block and unpins it.
   * @param block a reference to the disk block
   */
  void Unpin(const BlockId& block) { my_buffers_.Unpin(block); }

  /**
   * Return the integer value stored at the specified offset of the specified
   * block. The method first obtains a SharedLock on the block, then it calls
   * the buffer to retrieve the value.
   * @param block a reference to a disk block
   * @param offset the byte offset within the block
   * @return the integer stored at that offset
   */
  int GetInt(const BlockId& block, int offset);

  /**
   * Return the string value stored at the specified offset of the specified
   * block. The method first obtains a SharedLock on the block, then it calls
   * the buffer to retrieve the value.
   * @param block a reference to a disk block
   * @param offset the byte offset within the block
   * @return the string stored at that offset
   */
  std::string_view GetString(const BlockId& block, int offset);

  /**
   * @brief Store an integer at the specified offset of the specified block. The
   * method first obtains an ExclusiveLock on the block. It then logs this
   * update operation by calling `SetInt` method of recovery manager. Finally,
   * it calls the buffer to store the value, passing in the LSN of the log
   * record and the transaction's id.
   * @param block a reference to the disk block
   * @param offset the byte offset within the disk block
   * @param val the new value to store
   * @param OkToLog whether to log this operation
   */
  void SetInt(const BlockId& block, int offset, int val, bool OkToLog);

  /**
   * @brief Store a string at the specified offset of the specified block. The
   * method first obtains an ExclusiveLock on the block. It then logs this
   * update operation by calling `SetString` method of recovery manager.
   * Finally, it calls the buffer to store the value, passing in the LSN of the
   * log record and the transaction's id.
   * @param block a reference to the disk block
   * @param offset the byte offset within the disk block
   * @param val the new value to store
   * @param OkToLog whether to log this operation
   */
  void SetString(const BlockId& block, int offset, std::string_view val,
                 bool OkToLog);

  /**
   * @brief Return the number of blocks in the specified file. This method first
   * obtains a SharedLock on the "end of file" block before asking the file
   * manager to return the file size.
   * @param filename name of the file
   * @return the number of blocks in the file
   */
  int Size(std::string_view filename);

  /**
   * @brief Append a new block to the end of the specified file and return a
   * reference to it. This method first obtains an ExclusiveLock on the "end of
   * file" block before performing the append.
   * @param filename name of the file
   * @return a reference to the newly-created disk block
   */
  BlockId Append(std::string_view filename);

  /**
   * @brief Get the size of a disk block
   * @return size of a disk block
   */
  int BlockSize() const noexcept { return file_manager_.BlockSize(); }

  /**
   * @brief Return the number of available (i.e. unpinned) buffers
   * @return the number of available buffers
   */
  int AvailableBuffers() const noexcept { return buffer_manager_.Available(); }

 private:
  /**
   * @brief Return the next transaction id for use
   * @return the next transaction id
   */
  static int NextTxnId() {
    return __atomic_add_fetch(&next_txn_id_, 1, __ATOMIC_SEQ_CST);
  }

  static int next_txn_id_;
  static constexpr int END_OF_FILE = -1;

  FileManager& file_manager_;
  BufferManager& buffer_manager_;
  int txn_id_{};
  BufferList my_buffers_;
  ConcurrencyManager concurrency_manager_{};
  RecoveryManager recovery_manager_;
};
}  // namespace simpledb
