#include "server/simpledb.h"

#include <iostream>
#include <string_view>
#include <utility>

#include "log/log_manager.h"
#include "plan/basic_query_planner.h"
#include "plan/basic_update_planner.h"
#include "plan/query_planner.h"
#include "plan/update_planner.h"
#include "txn/transaction.h"

namespace simpledb {
Transaction SimpleDB::NewTxn() noexcept {
  return Transaction{file_manager_, log_manager_, buffer_manager_};
}

SimpleDB::SimpleDB(std::string_view dirname)
    : SimpleDB(dirname, BLOCK_SIZE, BUFFER_SIZE) {
  auto txn = NewTxn();
  bool is_new = file_manager_.IsNew();
  if (is_new) {
    std::cout << "Creating new database\n";
  } else {
    std::cout << "Recovering existing database\n";
    txn.Recover();
  }
  metadata_manager_ = std::make_unique<MetadataManager>(is_new, txn);
  std::unique_ptr<QueryPlanner> query_planner =
      std::make_unique<BasicQueryPlanner>(*metadata_manager_);
  std::unique_ptr<UpdatePlanner> update_planer =
      std::make_unique<BasicUpdatePlanner>(*metadata_manager_);

  planner_ = Planner{std::move(query_planner), std::move(update_planer)};

  txn.Commit();
}

}  // namespace simpledb
