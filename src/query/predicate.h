#pragma once

#include <optional>
#include <string>
#include <vector>

#include "query/term.h"

namespace simpledb {
/**
 * A predicate is a boolean combination of terms
 */
class Predicate {
 public:
  /**
   * @brief Create an empty predicate, corresponding to "true"
   */
  Predicate() = default;

  /**
   * @brief Create a predicate containing a single term
   * @param term the term
   */
  explicit Predicate(const Term& term) { AddTerm(term); }

  /**
   * @brief Modify the predicate to be the conjunction of itself and the
   * specified term
   * @param term the term
   */
  void AddTerm(const Term& term) { terms_.emplace_back(term); }

  /**
   * @brief Return whether the predicate is empty
   * @return true if the predicate is empty; otherwise, false
   */
  bool IsEmpty() const noexcept { return terms_.empty(); }

  /**
   * @brief Modify the predicate to be the conjunction of itself and the
   * specified predicate
   * @param predicate the other predicate
   */
  void ConjoinWith(const Predicate& predicate);

  /**
   * @brief Return true if the predicate evaluates to true with respect to the
   * specified scan
   * @param scan the scan
   * @return true if the predicate is true in the scan; otherwise, false
   */
  bool IsSatisfied(Scan& scan) const;

  /**
   * @brief Calculate the extent to which selecting on the predicate reduces the
   * number of records output by a query. For example, if the reduction factor
   * is 2, then the predicate cuts the size of the output in half.
   * @param plan the query's plan
   * @return the integer reduction factor
   */
  int ReductionFactor(const Plan& plan) const noexcept;

  /**
   * @brief Return the sub-predicate that applies to the specified schema
   * @param schema the schema
   * @return the sub-predicate applying to the schema
   */
  std::optional<Predicate> SelectSubPredicate(const Schema& schema) const;

  /**
   * @brief Return the sub-predicate consisting of terms that apply to the union
   * of the two specified schemas, but not to either schema separately
   * @param schema1 the first schema
   * @param schema2 the second schema
   * @return the sub-predicate whose terms apply to the union of the two schemas
   * but not either schema separately
   */
  std::optional<Predicate> JoinSubPredicate(const Schema& schema1,
                                            const Schema& schema2) const;

  /**
   * @brief Determine if there is a term of the form `F = c` where `F` is the
   * specified field and `c` is some constant. If so, the method returns that
   * constant. Otherwise, the method returns null.
   * @param field_name name of the field
   * @return either the constant or null
   */
  std::optional<Constant> EquatesWithConstant(
      std::string_view field_name) const noexcept;

  /**
   * @brief Determine if there is a term of the form `F1 = F2` where `F1` is the
   * specified field and `F2` is another field. If so, the method returns the
   * name of that field. Otherwise, the method returns null.
   * @param field_name name of the field
   * @return the name of the other field or null
   */
  std::optional<std::string> EquatesWithField(
      std::string_view field_name) const noexcept;

  /**
   * @brief Return the string representation of this predicate
   * @return the string representation
   */
  std::string ToString() const;

 private:
  std::vector<Term> terms_;
};
}  // namespace simpledb
