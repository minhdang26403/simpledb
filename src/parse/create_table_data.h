#pragma once

#include <string>
#include "record/schema.h"

namespace simpledb {
/**
 * Data for the SQL `create table` statement
 */
class CreateTableData {
 public:
  /**
   * @brief Save the table name and schema
   * @param table_name the table name
   * @param schema the table schema
   * TODO: Fix parameter type here?
   */
  CreateTableData(std::string_view table_name, const Schema& schema)
      : table_name_(table_name), schema_(schema) {}

  /**
   * @brief Return the name of the new table
   * @return name of the new table
   */
  std::string TableName() const noexcept { return table_name_; }

  /**
   * @brief Return the schema of the new table
   * @return schema of the new table
   * TODO: can return a const reference here?
   */
  Schema NewSchema() const noexcept { return schema_; }

 private:
  std::string table_name_;
  Schema schema_;
};
}  // namespace simpledb
