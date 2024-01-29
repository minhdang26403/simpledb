#pragma once

#include <string>

#include "file/page.h"
#include "log/log_manager.h"
#include "txn/recovery/log_record.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The COMMIT log record
 */
class CommitRecord : public LogRecord {
 public:
  /**
   * @brief Construct a new COMMIT log record that uses the given `page`
   * as the underlying storage
   * @param page the page containing the log values
   */
  explicit CommitRecord(const Page& page);

  /**
   * @brief Return the log record's type
   * @return the log record's type
   */
  LogType Op() const noexcept override { return LogType::COMMIT; }

  /**
   * @brief Return the transaction id stored with the log record
   * @return the log record's transaction id
   */
  int TxnId() const noexcept override { return txn_id_; }

  /**
   * @brief Do nothing because a COMMIT record contains no undo information
   * @param txn the transaction to undo
   */
  void Undo([[maybe_unused]] Transaction& txn) override {}

  /**
   * @brief Return the string representation of the COMMIT log record
   * @return the string representation of this record
   */
  std::string ToString() const;

  /**
   * @brief Write this COMMIT record to the log. This log record contains the
   * COMMIT operator, followed by the transaction id.
   * @param log_manager log manager of the database engine
   * @param txn_id transaction id
   * @return LSN of the last log value
   */
  static int WriteToLog(LogManager& log_manager, int txn_id);

 private:
  int txn_id_{};
};
}  // namespace simpledb
