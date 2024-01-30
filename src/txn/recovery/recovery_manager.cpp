#include "txn/recovery/recovery_manager.h"

#include <unordered_set>

#include "buffer/buffer_manager.h"
#include "log/log_manager.h"
#include "txn/recovery/checkpoint_record.h"
#include "txn/recovery/commit_record.h"
#include "txn/recovery/log_record.h"
#include "txn/recovery/rollback_record.h"
#include "txn/recovery/set_int_record.h"
#include "txn/recovery/set_string_record.h"
#include "txn/recovery/start_record.h"
#include "txn/transaction.h"

namespace simpledb {
RecoveryManager::RecoveryManager(Transaction& txn, int txn_id,
                                 LogManager& log_manager,
                                 BufferManager& buffer_manager)
    : txn_(txn),
      txn_id_(txn_id),
      log_manager_(log_manager),
      buffer_manager_(buffer_manager) {
  StartRecord::WriteToLog(log_manager_, txn_id_);
}

void RecoveryManager::Commit() {
  buffer_manager_.FlushAll(txn_id_);
  int lsn = CommitRecord::WriteToLog(log_manager_, txn_id_);
  log_manager_.Flush(lsn);
}

void RecoveryManager::Rollback() {
  DoRollback();
  buffer_manager_.FlushAll(txn_id_);
  int lsn = RollbackRecord::WriteToLog(log_manager_, txn_id_);
  log_manager_.Flush(lsn);
}

void RecoveryManager::Recover() {
  DoRecover();
  buffer_manager_.FlushAll(txn_id_);
  int lsn = CheckpointRecord::WriteToLog(log_manager_);
  log_manager_.Flush(lsn);
}

int RecoveryManager::SetInt(Buffer* buffer, int offset) {
  int old_val = buffer->Contents().GetInt(offset);
  return SetIntRecord::WriteToLog(log_manager_, txn_id_,
                                  buffer->Block().value(), offset, old_val);
}

int RecoveryManager::SetString(Buffer* buffer, int offset) {
  auto old_val = buffer->Contents().GetString(offset);
  return SetStringRecord::WriteToLog(log_manager_, txn_id_,
                                     buffer->Block().value(), offset, old_val);
}

void RecoveryManager::DoRollback() {
  auto iter = log_manager_.Iterator();
  while (iter.HasNext()) {
    auto bytes = iter.Next();
    auto record = LogRecord::CreateLogRecord(bytes);
    if (record->TxnId() == txn_id_) {
      if (record->Op() == LogType::START) {
        return;
      }
      record->Undo(txn_);
    }
  }
}

void RecoveryManager::DoRecover() {
  std::unordered_set<int> finished_txns_;
  auto iter = log_manager_.Iterator();
  while (iter.HasNext()) {
    auto bytes = iter.Next();
    auto record = LogRecord::CreateLogRecord(bytes);
    if (record->Op() == LogType::CHECKPOINT) {
      return;
    }
    if (record->Op() == LogType::COMMIT || record->Op() == LogType::ROLLBACK) {
      finished_txns_.insert(record->TxnId());
    } else if (!finished_txns_.contains(record->TxnId())) {
      record->Undo(txn_);
    }
  }
}
}  // namespace simpledb
