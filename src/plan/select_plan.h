#pragma once

#include <algorithm>
#include <memory>
#include <utility>

#include "plan/plan.h"
#include "query/predicate.h"
#include "query/select_scan.h"

namespace simpledb {
/**
 * The `Plan` class corresponding to the `select` relational algebra operator
 */
class SelectPlan : public Plan {
 public:
  /**
   * @brief Create a new `select` node in the query tree, having the specified
   * subquery and predicate.
   * @param plan the subquery
   * @param predicate the predicate
   */
  SelectPlan(const std::shared_ptr<Plan>& plan, const Predicate& predicate)
      : plan_(plan), predicate_(predicate) {}

  SelectPlan(std::shared_ptr<Plan>&& plan, const Predicate& predicate)
      : plan_(std::move(plan)), predicate_(predicate) {}

  /**
   * @brief Create a `select` scan for this query
   * @return the `select` scan
   */
  std::unique_ptr<Scan> Open() noexcept override {
    return std::make_unique<SelectScan>(plan_->Open(), predicate_);
  }

  /**
   * @brief Estimate the number of block accesses in the selection, which is the
   * same as in the underlying query
   * @return the number of block accesses
   */
  int BlocksAccessed() const noexcept override {
    return plan_->BlocksAccessed();
  }

  /**
   * @brief Estimate the number of output records in the selection, which is
   * determined by the reduction factor of the predicate
   * @return the number of output records
   */
  int RecordsOutput() const noexcept override {
    return plan_->RecordsOutput() / predicate_.ReductionFactor(*plan_);
  }

  /**
   * @brief Estimate the number of distinct field values in the projection. If
   * the predicate contains a term equating the specified field to a constant,
   * then this value will be 1. Otherwise, it will be the number of the distinct
   * values in the underlying query (but not more than the size of the output
   * table)
   * @param field_name the field to retrieve its number of distinct values
   * @return the number of distinct values
   */
  int DistinctValues(std::string_view field_name) const noexcept override {
    if (predicate_.EquatesWithConstant(field_name).has_value()) {
      return 1;
    }
    auto res = predicate_.EquatesWithField(field_name);
    if (res.has_value()) {
      return std::min(plan_->DistinctValues(field_name),
                      plan_->DistinctValues(res.value()));
    }
    return plan_->DistinctValues(field_name);
  }

  /**
   * @brief Return the schema of the selection, which is the same as in the
   * underlying query
   * @return the selection's schema
   */
  Schema& GetSchema() noexcept override { return plan_->GetSchema(); }

 private:
  std::shared_ptr<Plan> plan_;
  Predicate predicate_;
};
}  // namespace simpledb
