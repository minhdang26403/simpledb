#pragma once

#include <string>
#include <utility>
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
  template <typename Const>
    requires std::is_same_v<std::decay_t<Const>, Constant>
  explicit Expression(Const&& val) : expr_(std::forward<Const>(val)) {}

  /**
   * @brief Construct a field name expression
   * @param field_name name of the field
   */
  template <typename Str>
    requires std::is_constructible_v<std::string, std::decay_t<Str>>
  explicit Expression(Str&& field_name)
      : expr_(std::in_place_index<1>, std::forward<Str>(field_name)) {}

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
  const Constant& AsConstant() const & { return std::get<0>(expr_); }

  // Overload for rvalue object
  Constant AsConstant() && { return std::get<0>(std::move(expr_)); }

  /**
   * Return the field name corresponding to the expression or throw
   * `std::bad_variant_access` exception if the expression does not denote a
   * field
   * @return the expression as a field name
   */
  const std::string& AsFieldName() const& { return std::get<1>(expr_); }

  // Overload for rvalue object
  std::string AsFieldName() && { return std::get<1>(std::move(expr_)); }

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
