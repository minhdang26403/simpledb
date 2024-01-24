#include "server/simpledb.h"

#include <iostream>
#include <string_view>

#include "log/log_manager.h"

namespace simpledb {
SimpleDB::SimpleDB(std::string_view dirname)
    : SimpleDB(dirname, BLOCK_SIZE, BUFFER_SIZE) {
  bool is_new = file_manager_.IsNew();
  if (is_new) {
    std::cout << "creating new database\n";
  } else {
    std::cout << "recovering existing database\n";
  }
}

}  // namespace simpledb
