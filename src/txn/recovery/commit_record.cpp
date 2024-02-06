#include "txn/recovery/commit_record.h"

#include <memory>
#include <span>  // NOLINT(build/include_order)
#include <sstream>

#include "file/page.h"
#include "log/log_manager.h"
#include "txn/recovery/log_record.h"

namespace simpledb {
CommitRecord::CommitRecord(const Page& page) {
  int txn_pos = sizeof(int);
  txn_id_ = page.GetInt(txn_pos);
}

std::string CommitRecord::ToString() const {
  std::stringstream output;
  output << "<COMMMIT " << txn_id_ << '>';

  return output.str();
}

int CommitRecord::WriteToLog(LogManager& log_manager, int txn_id) {
  size_t record_size = 2 * sizeof(int);
  auto record = std::make_unique<char[]>(record_size);
  Page page{record.get(), record_size};
  page.SetInt(0, static_cast<int>(LogType::COMMIT));
  page.SetInt(sizeof(int), txn_id);

  return log_manager.Append(std::span{record.get(), record_size});
}
}  // namespace simpledb
