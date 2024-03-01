#pragma once

#include <string>
#include <utility>

#include "query/expression.h"
#include "query/predicate.h"

namespace simpledb {
/**
 * Data for the SQL `update` statement
 */
class ModifyData {
 public:
  /**
   * @brief Save the table name, the modified field, its new value, and the
   * predicate
   * @param table_name the table name
   * @param field_name the modified field
   * @param new_val new value of the modified field
   * @param predicate the modification predicate
   */
  template <typename Str1, typename Str2, typename Expr, typename Pred>
  ModifyData(Str1&& table_name, Str2&& field_name, Expr&& new_val,
             Pred&& predicate)
      : table_name_(std::forward<Str1>(table_name)),
        field_name_(std::forward<Str2>(field_name)),
        new_val_(std::forward<Expr>(new_val)),
        predicate_(std::forward<Pred>(predicate)) {}

  /**
   * @brief Return the name of the affected table
   * @return name of the affected table
   */
  std::string TableName() const noexcept { return table_name_; }

  /**
   * @brief Return the field whose values will be modified
   * @return name of the target field
   */
  std::string TargetField() const noexcept { return field_name_; }

  /**
   * @brief Return an expression. Evaluating this expression for a record
   * produces the value that will be stored in the record's target field.
   * @return the target expression
   */
  Expression NewValue() const noexcept { return new_val_; }

  /**
   * @brief Return the predicate that describes which records should be modified
   * @return the modification predicate
   * TODO: Fix return type?
   */
  Predicate ModificationPredicate() const noexcept { return predicate_; }

 private:
  std::string table_name_;
  std::string field_name_;
  Expression new_val_;
  Predicate predicate_;
};
}  // namespace simpledb
