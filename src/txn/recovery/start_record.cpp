#include "txn/recovery/start_record.h"

#include <memory>

#include "file/page.h"
#include "log/log_manager.h"
#include "txn/recovery/log_record.h"

namespace simpledb {
StartRecord::StartRecord(const Page& page) {
  int txn_pos = sizeof(int);
  txn_id_ = page.GetInt(txn_pos);
}

int StartRecord::WriteToLog(LogManager& log_manager) {
  size_t record_size = 2 * sizeof(int);
  auto record = std::make_unique<char[]>(record_size);
  Page page{record.get(), record_size};
  page.SetInt(0, static_cast<int>(LogType::START));
  page.SetInt(sizeof(int), txn_id_);

  return log_manager.Append(std::span{record.get(), record_size});
}
}  // namespace simpledb
