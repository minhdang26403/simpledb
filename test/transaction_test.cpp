#include "txn/transaction.h"

#include <iostream>
#include <string>

#include "file/block_id.h"
#include "server/simpledb.h"

namespace simpledb {
void TransactionTest() {
  SimpleDB db{"txn_test", 400, 8};
  auto& file_manager = db.GetFileManager();
  auto& log_manager = db.GetLogManager();
  auto& buffer_manager = db.GetBufferManager();

  Transaction txn1{file_manager, log_manager, buffer_manager};
  BlockId block{"test_file", 1};
  txn1.Pin(block);
  // The block initially contains unknown bytes,
  // so don't log those values here.
  txn1.SetInt(block, 80, 1, false);
  txn1.SetString(block, 40, "one", false);
  txn1.Commit();

  Transaction txn2{file_manager, log_manager, buffer_manager};
  txn2.Pin(block);
  int ival = txn2.GetInt(block, 80);
  std::string_view sval = txn2.GetString(block, 40);
  std::cout << "Initial value at location 80 = " << ival << '\n';
  std::cout << "Initial value at location 40 = " << sval << '\n';

  int new_ival = ival + 1;
  std::string new_sval = std::string(sval) + "!";
  txn2.SetInt(block, 80, new_ival, true);
  txn2.SetString(block, 40, new_sval, true);
  txn2.Commit();

  Transaction txn3{file_manager, log_manager, buffer_manager};
  txn3.Pin(block);
  std::cout << "New value at location 80 = " << txn3.GetInt(block, 80) << '\n';
  std::cout << "New value at location 40 = " << txn3.GetString(block, 40) << '\n';
  txn3.SetInt(block, 80, 9999, true);
  std::cout << "pre-rollback value at location 80 = " << txn3.GetInt(block, 80) << '\n';
  txn3.Rollback();

  Transaction txn4 {file_manager, log_manager, buffer_manager};
  txn4.Pin(block);
  std::cout << "post-rollback at location 80 = " << txn4.GetInt(block, 80) << '\n';
  txn4.Commit();
}
}  // namespace simpledb

int main() {
  simpledb::TransactionTest();

  return 0;
}
