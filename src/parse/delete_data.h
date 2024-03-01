#pragma once

#include <string>

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
   * TODO: Fix parameter type?
   */
  DeleteData(std::string_view table_name, const Predicate& predicate)
      : table_name_(table_name), predicate_(predicate) {}

  /**
   * @brief Return the name of the affected table
   * @return name of the affected table
   */
  std::string TableName() const noexcept { return table_name_; }

  /**
   * @brief Return the predicate that describes which records should be deleted
   * @return the deletion predicate
   */
  Predicate DeletionPredicate() const noexcept { return predicate_; }

 private:
  std::string table_name_;
  Predicate predicate_;
};
}  // namespace simpledb
