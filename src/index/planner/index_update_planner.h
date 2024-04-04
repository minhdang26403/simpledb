#pragma once

#include "metadata/metadata_manager.h"
#include "plan/update_planner.h"

namespace simpledb {
/**
 * A modification of the basic update planner. It dispatches each update
 * statement to the corresponding index planner
 */
class IndexUpdatePlanner : public UpdatePlanner {
 public:
  explicit IndexUpdatePlanner(MetadataManager& metadata_manager)
      : metadata_manager_(metadata_manager) {}

  /**
   * @brief Execute the specified `insert` statement, and return the number of
   * affected records
   * @param data the parsed representation of the `insert` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteInsert(const InsertData& data, Transaction& txn);

  /**
   * @brief Execute the specified `delete` statement, and return the number of
   * affected records
   * @param data the parsed representation of the `delete` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteDelete(const DeleteData& data, Transaction& txn);

  /**
   * @brief Execute the specified `modify` statement, and return the number of
   * affected records
   * @param data the parsed representation of the `modify` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteModify(const ModifyData& data, Transaction& txn);

  /**
   * @brief Execute the specified `create table` statement, and return the
   * number of affected records
   * @param data the parsed representation of the `create table` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteCreateTable(const CreateTableData& data, Transaction& txn);

  /**
   * @brief Execute the specified `create view` statement, and return the number
   * of affected records
   * @param data the parsed representation of the `create view` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteCreateView(const CreateViewData& data, Transaction& txn);

  /**
   * @brief Execute the specified `create index` statement, and return the
   * number of affected records
   * @param data the parsed representation of the `create index` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteCreateIndex(const CreateIndexData& data, Transaction& txn);

 private:
  MetadataManager& metadata_manager_;
};
}  // namespace simpledb
