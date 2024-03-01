#pragma once

#include <cassert>
#include <string>
#include <vector>

#include "query/constant.h"

namespace simpledb {
/**
 * Data for the SQL `insert` statement
 */
class InsertData {
 public:
  /**
   * @brief Save the table name, field list, and value lists
   * @param table_name the table name
   * @param fields a list of field names
   * @param values a list of `Constant` values
   * TODO: Fix parameter type?
   */
  InsertData(std::string_view table_name,
             const std::vector<std::string>& fields,
             const std::vector<Constant>& values)
      : table_name_(table_name), fields_(fields), values_(values) {
    assert(fields_.size() == values_.size());
  }

  /**
   * @brief Return the name of the affected table
   * @return name of the affected table
   */
  std::string TableName() const noexcept { return table_name_; }

  /**
   * @brief Return a list of fields for which values will be specified in the
   * new record
   * @return a list of field names
   */
  std::vector<std::string> Fields() const noexcept { return fields_; }

  /**
   * @brief Return a list of values for the specified fields. There is a one-one
   * correspondence between this list of values and the list of fields.
   * @return a list of `Constant` values
   */
  std::vector<Constant> Values() const noexcept { return values_; }

 private:
  std::string table_name_;
  std::vector<std::string> fields_;
  std::vector<Constant> values_;
};
}  // namespace simpledb
