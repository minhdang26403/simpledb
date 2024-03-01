#pragma once

#include <optional>
#include <string>
#include <utility>

#include "plan/plan.h"
#include "query/expression.h"

namespace simpledb {
/**
 * A term is a comparison between two expressions
 */
class Term {
 public:
  /**
   * Create a new term that compares two expressions for equality
   * @param lhs the left-hand side expression
   * @param rhs the right-hand side expression
   */
  template <typename Expr1, typename Expr2>
  Term(Expr1&& lhs, Expr2&& rhs)
      : lhs_(std::forward<Expr1>(lhs)), rhs_(std::forward<Expr2>(rhs)) {}

  /**
   * Return true if both of the term's expressions evaluate to the same
   * constant, with respect to the specified scan
   * @param scan the scan
   * @return true if both expressions have the same value in the scan;
   * otherwise, false
   */
  bool IsSatisfied(Scan& scan) const {
    return lhs_.Evaluate(scan) == rhs_.Evaluate(scan);
  }

  /**
   * @brief Calculate the extent to which selecting on the term reduces the
   * number of records output by a query. For example, if the reduction factor
   * is 2, then the term cuts the size of the output in half.
   * @param plan the query's plan
   * @return the integer reduction factor
   */
  int ReductionFactor(const Plan& plan) const noexcept;

  /**
   * @brief Determine if this term is of the form `F = c` where `F` is the
   * specified field and `c` is some constant. If so, the method returns that
   * constant. Otherwise, the method returns null.
   * @param field_name name of the field
   * @return either the constant or null
   */
  std::optional<Constant> EquatesWithConstant(
      std::string_view field_name) const noexcept;

  /**
   * @brief Determine if this term is of the form `F1 = F2` where `F1` is the
   * specified field and `F2` is another field. If so, the method returns the
   * name of that field. Otherwise, the method returns null.
   * @param field_name name of the field
   * @return either the name of the other field or null
   */
  std::optional<std::string> EquatesWithField(
      std::string_view field_name) const noexcept;

  /**
   * Return true if both of the term's expressions apply to the specified
   * schema.
   * @param schema the schema
   * @return true if both expressions apply to the schema; otherwise, false
   */
  bool AppliesTo(const Schema& schema) const noexcept {
    return lhs_.AppliesTo(schema) && rhs_.AppliesTo(schema);
  }

  /**
   * @brief Return the string representation of this term
   * @return the string representation
   */
  std::string ToString() const {
    return lhs_.ToString() + " = " + rhs_.ToString();
  }

 private:
  Expression lhs_;
  Expression rhs_;
};
}  // namespace simpledb
