#pragma once

#include <string>

#include "file/page.h"
#include "log/log_manager.h"
#include "txn/recovery/log_record.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The START record
 */
class StartRecord : public LogRecord {
 public:
  /**
   * @brief Construct a new START log record that uses the given `page`
   * as the underlying storage
   * @param page the page containing the log values
   */
  explicit StartRecord(const Page& page);

  /**
   * @brief Return the log record's type
   * @return the log record's type
   */
  LogType Op() const noexcept override { return LogType::START; }

  /**
   * @brief Return the transaction id stored with the log record
   * @return the log record's transaction id
   */
  int TxnId() const noexcept override { return txn_id_; }

  /**
   * @brief Do nothing because a START record contains no undo information
   * @param txn the transaction to undo
   */
  void Undo([[maybe_unused]] Transaction& txn) override {}

  /**
   * @brief Return the string representation of this START log record
   * @return the string representation of this record
   */
  std::string ToString() const;

  /**
   * @brief Write a START record to the log. This log record contains the START
   * operator, followed by the transaction id.
   * @return LSN of the last log value
   */
  static int WriteToLog(LogManager& log_manager, int txn_id);

 private:
  int txn_id_{};
};
}  // namespace simpledb
