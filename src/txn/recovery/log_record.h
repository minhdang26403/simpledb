#pragma once

#include <span>
#include <memory>

#include "txn/transaction.h"

namespace simpledb {
/**
 * Log record type
 */
enum class LogType : int {
  CHECKPOINT,
  START,
  COMMIT,
  ROLLBACK,
  SETINT,
  SETSTRING
};

/**
 * The interface implemented by each type of log record
 */
class LogRecord {
 public:
  virtual ~LogRecord() = default;
  /**
   * @brief Return the log record's type
   * @return the log record's type
   */
  virtual LogType Op() const noexcept = 0;

  /**
   * @brief Return the transaction id stored with the log record
   * @return the log record's transaction id
   */
  virtual int TxnId() const noexcept = 0;

  /**
   * @brief Undo the operation encoded by this log record. The only log record
   * types for which this method does anything interesting are SETINT and
   * SETSTRING. 
   * @param txn the transaction to undo
   */
  virtual void Undo([[maybe_unused]] Transaction& txn) = 0;

  /**
   * @brief Interpret the bytes returned by the log iterator.
   * @param bytes the bytes to interpret
   * @return the log record encoded by the bytes
   */
  static std::unique_ptr<LogRecord> CreateLogRecord(std::span<char> bytes);
};
}  // namespace simpledb
