#pragma once

#include <string>

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
   * TODO: fix parameter type?
   */
  ModifyData(std::string_view table_name, std::string_view field_name,
             const Expression& new_val, const Predicate& predicate)
      : table_name_(table_name),
        field_name_(field_name),
        new_val_(new_val),
        predicate_(predicate) {}

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
   */
  Predicate ModificationPredicate() const noexcept { return predicate_; }

 private:
  std::string table_name_;
  std::string field_name_;
  Expression new_val_;
  Predicate predicate_;
};
}  // namespace simpledb
