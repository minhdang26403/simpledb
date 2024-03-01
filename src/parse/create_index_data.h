#pragma once

#include <sstream>
#include <string>
#include <utility>

namespace simpledb {
/**
 * Data for the SQL `create index` statement
 */
class CreateIndexData {
 public:
  /**
   * @brief Save the table name and field name of the specified index
   * @param index_name name of the index
   * @param table_name name of the indexed table
   * @param field_name name of the indexed field
   */
  template <typename Str1, typename Str2, typename Str3>
  CreateIndexData(Str1&& index_name, Str2&& table_name, Str3&& field_name)
      : index_name_(std::forward<Str1>(index_name)),
        table_name_(std::forward<Str2>(table_name)),
        field_name_(std::forward<Str3>(field_name)) {}

  /**
   * @brief Return the name of the index
   * @return name of the index
   * TODO: return type?
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
