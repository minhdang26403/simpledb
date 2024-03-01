#pragma once

#include <string>
#include <utility>

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
   */
  template <typename Str, typename Sch>
  CreateTableData(Str&& table_name, Sch&& schema)
      : table_name_(std::forward<Str>(table_name)),
        schema_(std::forward<Sch>(schema)) {}

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
