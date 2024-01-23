#include "server/simpledb.h"
#include "log/log_manager.h"

namespace simpledb {
FileManager& SimpleDB::GetFileManager() { return file_manager_; }

LogManager& SimpleDB::GetLogManager() { return log_manager_; }
}  // namespace simpledb
