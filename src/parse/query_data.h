#pragma once

#include <string>
#include <utility>
#include <vector>

#include "query/predicate.h"

namespace simpledb {
/**
 * Data for the SQL `select` statement
 */
class QueryData {
 public:
  /**
   * @brief Save the field, table list, and predicate of a query in an object
   * @param fields list of field names
   * @param tables list of table names
   * @param predicate the query predicate
   */
  template <typename T, typename U, typename V>
  QueryData(T&& fields, U&& tables, V&& predicate)
      : fields_(std::forward<T>(fields)),
        tables_(std::forward<U>(tables)),
        predicate_(std::forward<V>(predicate)) {}

  /**
   * @brief Return the fields mentioned in the select clause
   * @return a list of field names
   */
  const std::vector<std::string>& GetFields() const noexcept { return fields_; }

  /**
   * @brief Return the tables mentioned in the from clause
   * @return a list of table names
   */
  const std::vector<std::string>& GetTables() const noexcept { return tables_; }

  /**
   * @brief Return the predicate that describes which records should be in the
   * ouput table
   * @return the query predicate
   */
  const Predicate& GetPredicate() const noexcept { return predicate_; }

  /**
   * @brief Return the string representation of the query saved in this object
   * @return the query string
   */
  std::string ToString() const {
    std::string result{"select "};
    for (size_t i = 0; i < fields_.size() - 1; i++) {
      result.append(fields_[i]);
      result.append(", ");
    }
    result.append(fields_.back());

    result.append(" from ");
    for (size_t i = 0; i < tables_.size() - 1; i++) {
      result.append(tables_[i]);
      result.append(", ");
    }
    result.append(tables_.back());

    std::string predicate_str{predicate_.ToString()};
    if (!predicate_str.empty()) {
      result.append(" where ");
      result.append(predicate_str);
    }

    return result;
  }

 private:
  std::vector<std::string> fields_;
  std::vector<std::string> tables_;
  Predicate predicate_;
};
}  // namespace simpledb
