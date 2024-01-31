#include "server/simpledb.h"

#include <iostream>
#include <string_view>

#include "log/log_manager.h"
#include "txn/transaction.h"

namespace simpledb {
SimpleDB::SimpleDB(std::string_view dirname)
    : SimpleDB(dirname, BLOCK_SIZE, BUFFER_SIZE) {
  Transaction txn{file_manager_, log_manager_, buffer_manager_};
  bool is_new = file_manager_.IsNew();
  if (is_new) {
    std::cout << "Creating new database\n";
  } else {
    std::cout << "Recovering existing database\n";
  }
}

}  // namespace simpledb
