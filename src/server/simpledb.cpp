#include "server/simpledb.h"

namespace simpledb {
FileManager& SimpleDB::GetFileManager() { return file_manager_; }
}  // namespace simpledb
