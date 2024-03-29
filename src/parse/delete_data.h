#pragma once

#include <string>
#include <utility>

#include "query/predicate.h"

namespace simpledb {
/**
 * Data for the SQL `delete` statement
 */
class DeleteData {
 public:
  /**
   * @brief Save the table name and predicate
   * @param table_name the table name
   * @param predicate the predicate
   */
  template <typename Str, typename Pred>
  DeleteData(Str&& table_name, Pred&& predicate)
      : table_name_(std::forward<Str>(table_name)),
        predicate_(std::forward<Pred>(predicate)) {}

  /**
   * @brief Return the name of the affected table
   * @return name of the affected table
   */
  const auto& TableName() const & noexcept { return table_name_; }

  auto TableName() && noexcept { return std::move(table_name_); }

  /**
   * @brief Return the predicate that describes which records should be deleted
   * @return the deletion predicate
   */
  const auto& DeletionPredicate() const & noexcept { return predicate_; }

  auto DeletionPredicate() && noexcept { return std::move(predicate_); }

 private:
  std::string table_name_;
  Predicate predicate_;
};
}  // namespace simpledb
