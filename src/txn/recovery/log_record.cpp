#include "txn/recovery/log_record.h"

#include <memory>
#include <span>  // NOLINT(build/include_order)

#include "file/page.h"
#include "txn/recovery/checkpoint_record.h"
#include "txn/recovery/commit_record.h"
#include "txn/recovery/rollback_record.h"
#include "txn/recovery/set_int_record.h"
#include "txn/recovery/set_string_record.h"
#include "txn/recovery/start_record.h"

namespace simpledb {
std::unique_ptr<LogRecord> LogRecord::CreateLogRecord(std::span<char> bytes) {
  Page page{bytes.data(), bytes.size()};
  switch (static_cast<LogType>(page.GetInt(0))) {
    case LogType::CHECKPOINT:
      return std::make_unique<CheckpointRecord>();
    case LogType::START:
      return std::make_unique<StartRecord>(page);
    case LogType::COMMIT:
      return std::make_unique<CommitRecord>(page);
    case LogType::ROLLBACK:
      return std::make_unique<RollbackRecord>(page);
    case LogType::SETINT:
      return std::make_unique<SetIntRecord>(page);
    case LogType::SETSTRING:
      return std::make_unique<SetStringRecord>(page);
    default:
      return nullptr;
  }
}
}  // namespace simpledb
