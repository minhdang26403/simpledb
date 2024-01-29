#pragma once

#include <string>

#include "log/log_manager.h"
#include "txn/recovery/log_record.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The CHECKPOINT log record
 */
class CheckpointRecord : public LogRecord {
 public:
  /**
   * @brief Return the log record's type
   * @return the log record's type
   */
  LogType Op() const noexcept override { return LogType::CHECKPOINT; }

  /**
   * @brief CHECKPOINT records have no associated transaction, so the method
   * returns a "dummy", negative transaction id.
   * @return a "dummy" transaction id
   */
  int TxnId() const noexcept override { return -1; }

  /**
   * @brief Do nothing because a CHECKPOINT record contains no undo information
   * @param txn the transaction to undo
   */
  void Undo([[maybe_unused]] Transaction& txn) override {}

  /**
   * @brief Return the string representation of the CHECKPOINT log record
   * @return the string representation of this log record
   */
  std::string ToString() const noexcept { return "<CHECKPOINT>"; }

  /**
   * @brief Write this CHECKPOINT record to the log. This log record contains
   * the CHECKPOINT operator and nothing else.
   * @param log_manager log manager of the database engine
   * @return LSN of the last log value
   */
  static int WriteToLog(LogManager& log_manager);
};
}  // namespace simpledb
