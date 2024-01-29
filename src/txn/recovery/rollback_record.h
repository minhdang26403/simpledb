#pragma once

#include <string>

#include "file/page.h"
#include "log/log_manager.h"
#include "txn/recovery/log_record.h"

namespace simpledb {
/**
 * The ROLLBACK log record
 */
class RollbackRecord : public LogRecord {
 public:
  /**
   * @brief Construct a new ROLLBACK log record that uses the given `page`
   * as the underlying storage
   * @param page the page containing the log values
   */
  explicit RollbackRecord(const Page& page);

  /**
   * @brief Return the log record's type
   * @return the log record's type
   */
  LogType Op() const noexcept override { return LogType::ROLLBACK; }

  /**
   * @brief Return the transaction id stored with this log record
   * @return the log record's transaction id
   */
  int TxnId() const noexcept override { return txn_id_; }

  /**
   * @brief Do nothing because a ROLLBACK record contains no undo information
   * @param txn the transaction to undo
   */
  void Undo([[maybe_unused]] Transaction& txn) override {}

  /**
   * @brief Return the string representation of the ROLLBACK log record
   * @return the string representation of this log record
   */
  std::string ToString() const;

  /**
   * @brief Write this ROLLBACK record to the log
   * @param log_manager log manager of the database engine
   * @return the LSN of the last log value
   */
  int WriteToLog(LogManager& log_manager);

 private:
  int txn_id_;
};
}  // namespace simpledb
