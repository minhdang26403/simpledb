#pragma once

#include <string>

#include "file/block_id.h"
#include "file/page.h"
#include "log/log_manager.h"
#include "txn/recovery/log_record.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The SETINT log record
 */
class SetIntRecord : public LogRecord {
 public:
  /**
   * @brief Construct a new SETINT log record that uses the given `page`
   * as the underlying storage
   * @param page the page containing the log values
   */
  explicit SetIntRecord(const Page& page);

  /**
   * @brief Return the log record's type
   * @return the log record's type
   */
  LogType Op() const noexcept override { return LogType::SETINT; }

  /**
   * @brief Return the transaction id stored with the log record
   * @return the log record's transaction id
   */
  int TxnId() const noexcept override { return txn_id_; }

  /**
   * @brief Replace the specifed data value with the value saved in the log
   * record
   * @param txn the transaction to undo
   */
  void Undo(Transaction& txn) override;

  /**
   * @brief Return the string representation of this SETINT log record
   * @return the string representation of this record
   */
  std::string ToString() const;

  /**
   * @brief Write this SETINT record to the log. This log record contains the
   * SETINT operator, followed by the transaction id, filename, block number,
   * offset within the block, and the previous integer value at that offset.
   * @param log_manager log manager of the database engine
   * @param txn_id transaction id
   * @param block a reference to the disk block
   * @param offset offset in the block
   * @param val old value at the specified offset
   * @return LSN of the last log value
   */
  static int WriteToLog(LogManager& log_manager, int txn_id,
                        const BlockId& block, int offset, int val);

 private:
  int txn_id_{};
  BlockId block_;
  int offset_{};
  int val_{};
};
}  // namespace simpledb
