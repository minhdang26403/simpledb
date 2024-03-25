#include "query/predicate.h"

#include <string>

namespace simpledb {
void Predicate::ConjoinWith(const Predicate& predicate) {
  for (const auto& term : predicate.terms_) {
    AddTerm(term);
  }
}

bool Predicate::IsSatisfied(Scan& scan) const {
  for (const auto& term : terms_) {
    if (!term.IsSatisfied(scan)) {
      return false;
    }
  }

  return true;
}

int Predicate::ReductionFactor(const Plan& plan) const noexcept {
  int factor = 1;
  for (const auto& term : terms_) {
    factor *= term.ReductionFactor(plan);
  }

  return factor;
}

std::optional<Predicate> Predicate::SelectSubPredicate(
    const Schema& schema) const {
  Predicate result;
  for (const auto& term : terms_) {
    if (term.AppliesTo(schema)) {
      result.AddTerm(term);
    }
  }

  if (result.IsEmpty()) {
    return {};
  }

  return result;
}

std::optional<Predicate> Predicate::JoinSubPredicate(
    const Schema& schema1, const Schema& schema2) const {
  Predicate result;
  Schema new_schema;
  new_schema.AddAll(schema1);
  new_schema.AddAll(schema2);
  for (const auto& term : terms_) {
    if (!term.AppliesTo(schema1) && !term.AppliesTo(schema2) &&
        term.AppliesTo(new_schema)) {
      result.AddTerm(term);
    }
  }

  if (result.IsEmpty()) {
    return {};
  }

  return result;
}

std::optional<Constant> Predicate::EquatesWithConstant(
    std::string_view field_name) const noexcept {
  for (const auto& term : terms_) {
    auto result = term.EquatesWithConstant(field_name);
    if (result.has_value()) {
      return result;
    }
  }

  return {};
}

std::optional<std::string> Predicate::EquatesWithField(
    std::string_view field_name) const noexcept {
  for (const auto& term : terms_) {
    auto result = term.EquatesWithField(field_name);
    if (result.has_value()) {
      return result;
    }
  }

  return {};
}

std::string Predicate::ToString() const {
  if (IsEmpty()) {
    return "";
  }

  auto result = terms_.front().ToString();
  for (size_t i = 1; i < terms_.size(); i++) {
    result.append(" and ");
    result.append(terms_[i].ToString());
  }

  return result;
}
}  // namespace simpledb
