#pragma once

#include <memory>
#include <string>

#include "metadata/index_info.h"
#include "plan/plan.h"
#include "record/schema.h"

namespace simpledb {
/**
 * The `Plan` class corresponding to the `index join` relational algebra
 * operator
 */
class IndexJoinPlan : public Plan {
 public:
  /**
   * @brief Implement the join operator, using the specified LHS and RHS plans.
   * @param plan1 the left-hand plan
   * @param plan2 the right-hand plan
   * @param index_info information about the right-hand index
   * @param join_field the left-hand field used for joining
   */
  IndexJoinPlan(const std::shared_ptr<Plan>& plan1,
                const std::shared_ptr<Plan>& plan2,
                const std::shared_ptr<IndexInfo>& index_info,
                std::string_view join_field);

  /**
   * @brief Open an `index join` scan for this query
   * @return a scan
   */
  std::unique_ptr<Scan> Open() override;

  /**
   * @brief Estimate the number of block accesses to compute the join.
   * The formula is:
   *  B(index_join(p1, p2, idx)) = B(p1) + R(p1)*B(idx) +
   *    R(index_join(p1, p2, idx))
   * @return the estimated number of block accesses
   */
  int BlocksAccessed() const noexcept override;

  /**
   * @brief Estimate the number of output records in the join.
   * The formula is:
   *  R(index_join(p1, p2, idx)) = R(p1)*R(idx)
   * @return the estimated number of output records
   */
  int RecordsOutput() const noexcept override;

  /**
   * @brief Estimate the number of distinct values for the specified field
   * @return the estimated number of distinct field values in the output
   */
  int DistinctValues(std::string_view field_name) const noexcept override;

  /**
   * @brief Return the schema of the index join
   * @return the index join's schema
   */
  Schema& GetSchema() noexcept override;

 private:
  std::shared_ptr<Plan> plan1_;
  std::shared_ptr<Plan> plan2_;
  std::shared_ptr<IndexInfo> index_info_;
  std::string join_field_;
  Schema schema_;
};
}  // namespace simpledb
