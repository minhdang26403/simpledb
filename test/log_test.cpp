#include <iostream>
#include <span>  // NOLINT(build/include_order)

#include "file/page.h"
#include "log/log_manager.h"
#include "server/simpledb.h"
#include "utils/logger.h"

namespace simpledb {

void PrintLogRecords(LogManager& log_manager, std::string_view msg) {
  std::cout << msg << '\n';
  auto iter = log_manager.Iterator();

  while (iter.HasNext()) {
    std::span<char> record = iter.Next();
    Page page{record.data(), record.size()};
    std::string s{page.GetString(0)};
    int npos = Page::StringLength(s);
    int val = page.GetInt(npos);
    std::cout << "[" << s << ", " << val << "]\n";
  }
  std::cout << '\n';
}

// Create a log record having two values: a string and an integer
std::vector<char> CreateLogRecord(std::string_view str, int num) {
  int str_pos = 0;
  int num_pos = Page::StringLength(str);
  std::vector<char> record(num_pos + sizeof(int));
  Page page{record.data(), record.size()};
  page.SetString(str_pos, str);
  page.SetInt(num_pos, num);

  return record;
}

void CreateRecords(LogManager& log_manager, int start, int end) {
  std::cout << "Creating records: ";
  auto iter = log_manager.Iterator();
  for (int i = start; i <= end; i++) {
    auto record = CreateLogRecord("record" + std::to_string(i), i + 100);
    int lsn = log_manager.Append(std::span{record.data(), record.size()});
    std::cout << lsn << " ";
  }
  std::cout << '\n';
}

void LogTest() {
  SimpleDB db{"logtest", 400, 8};
  LogManager& log_manager = db.GetLogManager();

  PrintLogRecords(log_manager, "The initial empty log file:");
  std::cout << "done\n";
  CreateRecords(log_manager, 1, 35);
  PrintLogRecords(log_manager, "The log file now has these records:");
  CreateRecords(log_manager, 36, 70);
  log_manager.Flush(65);
  PrintLogRecords(log_manager, "The log file now has these records:");
}
}  // namespace simpledb

int main() {
  simpledb::LogTest();

  return 0;
}
