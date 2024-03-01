#pragma once

#include <sstream>
#include <string>

namespace simpledb {
/**
 * Data for the SQL `create index` statement
 */
class CreateIndexData {
 public:
  /**
   * TODO: Fix paramater type?
   * @brief Save the table name and field name of the specified index
   * @param index_name name of the index
   * @param table_name name of the indexed table
   * @param field_name name of the indexed field
   */
  CreateIndexData(std::string_view index_name, std::string_view table_name,
                  std::string_view field_name)
      : index_name_(index_name),
        table_name_(table_name),
        field_name_(field_name) {}

  /**
   * @brief Return the name of the index
   * @return name of the index
   */
  std::string IndexName() const noexcept { return index_name_; }

  /**
   * @brief Return the name of the indexed table
   * @return name of the indexed table
   */
  std::string TableName() const noexcept { return table_name_; }

  /**
   * @brief Return the name of the indexed field
   * @return name of the indexed field
   */
  std::string FieldName() const noexcept { return field_name_; }

 private:
  std::string index_name_;
  std::string table_name_;
  std::string field_name_;
};
}  // namespace simpledb
