#pragma once

#include "parse/create_index_data.h"
#include "parse/create_table_data.h"
#include "parse/create_view_data.h"
#include "parse/delete_data.h"
#include "parse/insert_data.h"
#include "parse/modify_data.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The interface implemented by the planners for SQL insert, delete, and modify
 * statements
 */
class UpdatePlanner {
 public:
  virtual ~UpdatePlanner() = default;

  /**
   * @brief Execute the specified `insert` statement, and return the number of
   * affected records
   * @param data the parsed representation of the `insert` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  virtual int ExecuteInsert(const InsertData& data, Transaction& txn) = 0;

  /**
   * @brief Execute the specified `delete` statement, and return the number of
   * affected records
   * @param data the parsed representation of the `delete` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  virtual int ExecuteDelete(const DeleteData& data, Transaction& txn) = 0;

  /**
   * @brief Execute the specified `modify` statement, and return the number of
   * affected records
   * @param data the parsed representation of the `modify` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  virtual int ExecuteModify(const ModifyData& data, Transaction& txn) = 0;

  /**
   * @brief Execute the specified `create table` statement, and return the
   * number of affected records
   * @param data the parsed representation of the `create table` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  virtual int ExecuteCreateTable(const CreateTableData& data, Transaction& txn) = 0;

  /**
   * @brief Execute the specified `create view` statement, and return the number
   * of affected records
   * @param data the parsed representation of the `create view` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  virtual int ExecuteCreateView(const CreateViewData& data, Transaction& txn) = 0;

  /**
   * @brief Execute the specified `create index` statement, and return the
   * number of affected records
   * @param data the parsed representation of the `create index` statement
   * @param txn the calling transaction
   * @return the number of affected records
   */
  virtual int ExecuteCreateIndex(const CreateIndexData& data, Transaction& txn) = 0;
};
}  // namespace simpledb
