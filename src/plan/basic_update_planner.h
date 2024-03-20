#pragma once

#include "metadata/metadata_manager.h"
#include "plan/update_planner.h"

namespace simpledb {
class BasicUpdatePlanner final : public UpdatePlanner {
 public:
  explicit BasicUpdatePlanner(MetadataManager& metadata_manager)
      : metadata_manager_(metadata_manager) {}

  /**
   * @brief Execute the specified `insert` statement, and return the number of
   * affected records
   * @param data the parsed representation of the `insert` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteInsert(const InsertData& data, Transaction& txn) override;

  /**
   * @brief Execute the specified `delete` statement, and return the number of
   * affected records
   * @param data the parsed representation of the `delete` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteDelete(const DeleteData& data, Transaction& txn) override;

  /**
   * @brief Execute the specified `modify` statement, and return the number of
   * affected records
   * @param data the parsed representation of the `modify` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteModify(const ModifyData& data, Transaction& txn) override;

  /**
   * @brief Execute the specified `create table` statement, and return the
   * number of affected records
   * @param data the parsed representation of the `create table` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteCreateTable(const CreateTableData& data,
                         Transaction& txn) override;

  /**
   * @brief Execute the specified `create view` statement, and return the number
   * of affected records
   * @param data the parsed representation of the `create view` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteCreateView(const CreateViewData& data, Transaction& txn) override;

  /**
   * @brief Execute the specified `create index` statement, and return the
   * number of affected records
   * @param data the parsed representation of the `create index` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  int ExecuteCreateIndex(const CreateIndexData& data,
                         Transaction& txn) override;

 private:
  MetadataManager& metadata_manager_;
};
}  // namespace simpledb
