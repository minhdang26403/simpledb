#pragma once

#include <string>
#include <variant>

#include "query/constant.h"
#include "query/scan.h"
#include "record/schema.h"

namespace simpledb {
/**
 * The interface corresponding to SQL expressions
 */
class Expression {
 public:
  /**
   * @brief Construct a constant expression
   * @param val value of the constant
   */
  explicit Expression(const Constant& val) : expr_(val) {}

  /**
   * @brief Construct a field name expression
   * @param field_name name of the field
   */
  explicit Expression(std::string_view field_name)
      : expr_(std::in_place_index<1>, field_name) {}

  /**
   * Evaluate the expression with respect to the
   * current record of the specified scan
   * @param scan the scan
   * @return the value of the expression, as a `Constant`
   */
  Constant Evaluate(Scan& scan) const {
    return std::holds_alternative<Constant>(expr_) ? AsConstant()
                                                   : scan.GetVal(AsFieldName());
  }

  /**
   * Return true if the expression is a field reference
   * @return true if the expression denotes a field; otherwise, false
   */
  bool IsFieldName() const noexcept {
    return std::holds_alternative<std::string>(expr_);
  }

  /**
   * Return the constant corresponding to the expression or throw
   * `std::bad_variant_access` exception if the expression does not denote a
   * constant
   * @return the expression as a constant
   */
  Constant AsConstant() const { return std::get<0>(expr_); }

  /**
   * Return the field name corresponding to the expression or throw
   * `std::bad_variant_access` exception if the expression does not denote a
   * field
   * @return the expression as a field name
   */
  std::string_view AsFieldName() const { return std::get<1>(expr_); }

  /**
   * Determine if all of the fields mentioned in this expression are contained
   * in the specified schema
   * @param schema the schema
   * @return true if all fields in the expression are in the schema; otherwise,
   * false
   */
  bool AppliesTo(const Schema& schema) const noexcept {
    return std::holds_alternative<Constant>(expr_)
               ? true
               : schema.HasField(AsFieldName());
  }

  /**
   * @brief Return the string representation of this expression
   * @return the string representation
   */
  std::string ToString() const {
    return std::holds_alternative<Constant>(expr_) ? AsConstant().ToString()
                                                   : std::string{AsFieldName()};
  }

 private:
  std::variant<Constant, std::string> expr_;
};
}  // namespace simpledb
