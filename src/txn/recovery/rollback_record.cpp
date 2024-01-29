#include "txn/recovery/rollback_record.h"

#include <memory>
#include <span>  // NOLINT(build/include_order)
#include <sstream>
#include <string>

#include "file/page.h"
#include "log/log_manager.h"
#include "txn/recovery/log_record.h"

namespace simpledb {
RollbackRecord::RollbackRecord(const Page& page) {
  int txn_pos = sizeof(int);
  txn_id_ = page.GetInt(txn_pos);
}

std::string RollbackRecord::ToString() const {
  std::stringstream output;
  output << "<ROLLBACK " << txn_id_ << '>';

  return output.str();
}

int RollbackRecord::WriteToLog(LogManager& log_manager, int txn_id) {
  size_t record_size = 2 * sizeof(int);
  auto record = std::make_unique<char[]>(record_size);
  Page page{record.get(), record_size};
  page.SetInt(0, static_cast<int>(LogType::ROLLBACK));
  page.SetInt(sizeof(int), txn_id);

  return log_manager.Append(std::span{record.get(), record_size});
}
}  // namespace simpledb
