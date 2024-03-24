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
  template <typename VecStr1, typename VecStr2, typename Pred>
  QueryData(VecStr1&& fields, VecStr2&& tables, Pred&& predicate)
      : fields_(std::forward<VecStr1>(fields)),
        tables_(std::forward<VecStr2>(tables)),
        predicate_(std::forward<Pred>(predicate)) {}

  /**
   * @brief Return the fields mentioned in the select clause
   * @return a list of field names
   */
  const auto& GetFields() const & noexcept { return fields_; }
  
  auto GetFields() && noexcept { return std::move(fields_); }

  /**
   * @brief Return the tables mentioned in the from clause
   * @return a list of table names
   */
  const auto& GetTables() const & noexcept { return tables_; }

  auto GetTables() && noexcept { return std::move(tables_); }

  /**
   * @brief Return the predicate that describes which records should be in the
   * ouput table
   * @return the query predicate
   */
  const auto& GetPredicate() const & noexcept { return predicate_; }
  
  auto GetPredicate() && noexcept { return std::move(predicate_); }

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
