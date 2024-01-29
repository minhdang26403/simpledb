#pragma once

#include "buffer/buffer_manager.h"
#include "log/log_manager.h"
#include "txn/recovery/commit_record.h"
#include "txn/recovery/start_record.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The recovery manager. Each transaction has its own recovery manager.
 */
class RecoveryManager {
 public:
  /**
   * @brief Create a recovery manager for the specified transaction
   * @param txn the transaction to create the recovery manager for
   * @param txn_id id of the specified transaction
   * @param log_manager log manager of the database engine
   * @param buffer_manager buffer manager of the database engine
   */
  RecoveryManager(Transaction& txn, int txn_id, LogManager& log_manager,
                  BufferManager& buffer_manager);

  /**
   * @brief Write a COMMIT record to the log and flush it to disk
   */
  void Commit();

  /**
   * @brief Write a ROLLBACK record to the log and flush it to disk
   */
  void Rollback();

  /**
   * @brief Recover uncompleted transactions from the log and then write a
   * quiescent checkpoint record to the log and flush it
   */
  void Recover();

  /**
   * @brief Write a SETINT record to the log to record the old value at the
   * specified offset before being overwritten by a new value
   * @param buffer the buffer containing the page
   * @param offset offset of the value in the page
   * @return LSN of the SETINT record
   */
  int SetInt(Buffer* buffer, int offset);

  /**
   * @brief Write a SETSTRING record to the log to record the old value at the
   * specified offset before being overwritten by a new value
   * @param buffer the buffer containing the page
   * @param offset offset of the value in the page
   * @return LSN of the SETSTRING record
   */
  int SetString(Buffer* buffer, int offset);

 private:
  /**
   * @brief Rollback the transaction by iterating through the log records until
   * it finds the transaction's START record, calling `Undo()` for each of the
   * transaction's log records.
   */
  void DoRollback();

  /**
   * @brief Do a complete database recovery. The method iterates through the log
   * records. Whenever it finds a log record for an unfinished transaction, it
   * calls `Undo()` on that record. The method stops when it encounters a
   * CHECKPOINT record or the end of the log.
   */
  void DoRecover();

  Transaction& txn_;
  int txn_id_{};
  LogManager& log_manager_;
  BufferManager& buffer_manager_;
};
}  // namespace simpledb
