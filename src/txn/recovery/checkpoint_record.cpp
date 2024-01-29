#include "txn/recovery/checkpoint_record.h"

#include <memory>
#include <span>  // NOLINT(build/include_order)

#include "file/page.h"
#include "log/log_manager.h"
#include "txn/recovery/log_record.h"

namespace simpledb {
int CheckpointRecord::WriteToLog(LogManager& log_manager) {
  size_t record_size = sizeof(int);
  auto record = std::make_unique<char[]>(record_size);
  Page page{record.get(), record_size};
  page.SetInt(0, static_cast<int>(LogType::CHECKPOINT));

  return log_manager.Append(std::span{record.get(), record_size});
}
}  // namespace simpledb
