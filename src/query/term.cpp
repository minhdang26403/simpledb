#include "query/term.h"

#include <algorithm>
#include <string>

namespace simpledb {
int Term::ReductionFactor(const Plan& plan) const noexcept {
  if (lhs_.IsFieldName() && rhs_.IsFieldName()) {
    return std::max(plan.DistinctValues(lhs_.AsFieldName()),
                    plan.DistinctValues(rhs_.AsFieldName()));
  }
  if (lhs_.IsFieldName()) {
    return plan.DistinctValues(lhs_.AsFieldName());
  }
  if (rhs_.IsFieldName()) {
    return plan.DistinctValues(rhs_.AsFieldName());
  }
  // otherwise, the term equates constants
  if (lhs_.AsConstant() == rhs_.AsConstant()) {
    return 1;
  }

  return INT_MAX;
}

std::optional<Constant> Term::EquatesWithConstant(
    std::string_view field_name) const noexcept {
  if (lhs_.IsFieldName() && lhs_.AsFieldName() == field_name &&
      !rhs_.IsFieldName()) {
    return rhs_.AsConstant();
  } else if (rhs_.IsFieldName() && rhs_.AsFieldName() == field_name &&
             !lhs_.IsFieldName()) {
    return lhs_.AsConstant();
  }

  return {};
}

std::optional<std::string> Term::EquatesWithField(
    std::string_view field_name) const noexcept {
  if (lhs_.IsFieldName() && lhs_.AsFieldName() == field_name &&
      rhs_.IsFieldName()) {
    return std::string{rhs_.AsFieldName()};
  } else if (rhs_.IsFieldName() && rhs_.AsFieldName() == field_name &&
             lhs_.IsFieldName()) {
    return std::string{lhs_.AsFieldName()};
  }

  return {};
}
}  // namespace simpledb
