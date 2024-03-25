#pragma once

#include <cassert>
#include <string>
#include <utility>
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
   */
  template <typename Str, typename VecStr, typename VecConst>
  InsertData(Str&& table_name, VecStr&& fields, VecConst&& values)
      : table_name_(std::forward<Str>(table_name)),
        fields_(std::forward<VecStr>(fields)),
        values_(std::forward<VecConst>(values)) {
    assert(fields_.size() == values_.size());
  }

  /**
   * @brief Return the name of the affected table
   * @return name of the affected table
   */
  const auto& TableName() const& noexcept { return table_name_; }

  auto TableName() && noexcept { return std::move(table_name_); }

  /**
   * @brief Return a list of fields for which values will be specified in the
   * new record
   * @return a list of field names
   */
  const auto& Fields() const & noexcept { return fields_; }

  auto Fields() && noexcept { return std::move(fields_); }

  /**
   * @brief Return a list of values for the specified fields. There is a one-one
   * correspondence between this list of values and the list of fields.
   * @return a list of `Constant` values
   */
  const auto& Values() const & noexcept { return values_; }

  auto Values() && noexcept { return std::move(values_); }

 private:
  std::string table_name_;
  std::vector<std::string> fields_;
  std::vector<Constant> values_;
};
}  // namespace simpledb
