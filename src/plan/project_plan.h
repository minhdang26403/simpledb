#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "plan/plan.h"
#include "query/project_scan.h"
#include "record/schema.h"

namespace simpledb {
/**
 * The `Plan` class corresponding to the `project` relational algebra operator
 */
class ProjectPlan : public Plan {
 public:
  /**
   * @brief Create a new project node in the query tree, having the specified
   * subquery and field list
   * @param plan the subquery
   * @param field_list the list of fields
   */
  ProjectPlan(const std::shared_ptr<Plan>& plan,
              const std::vector<std::string>& field_list)
      : plan_(plan) {
    for (const auto& field_name : field_list) {
      schema_.Add(field_name, plan_->GetSchema());
    }
  }

  ProjectPlan(std::shared_ptr<Plan>&& plan,
              const std::vector<std::string>& field_list)
      : plan_(std::move(plan)) {
    for (const auto& field_name : field_list) {
      schema_.Add(field_name, plan_->GetSchema());
    }
  }

  /**
   * @brief Create a `project` scan for this query
   * @return the `project` scan
   */
  std::unique_ptr<Scan> Open() noexcept override {
    StringSet field_list{schema_.Fields().begin(), schema_.Fields().end()};
    return std::make_unique<ProjectScan>(plan_->Open(), field_list);
  }

  /**
   * @brief Estimate the number of block accesses in the projection, which is
   * the same as in the underlying subquery
   * @return the number of block accesses
   */
  int BlocksAccessed() const noexcept override {
    return plan_->BlocksAccessed();
  }

  /**
   * @brief Estimate the number of output records in the projection, which is
   * the same as in the underlying query
   * @return the number of output records
   */
  int RecordsOutput() const noexcept override { return plan_->RecordsOutput(); }

  /**
   * @brief Estimate the number of distinct field values in the projection,
   * which is the same as in the underlying query
   * @param field_name the field to retrieve its number of distinct values
   * @return the number of distinct values
   */
  int DistinctValues(std::string_view field_name) const noexcept override {
    return plan_->DistinctValues(field_name);
  }

  /**
   * @brief Return the schema of the projection, which is taken from the field
   * list
   * @return the projection's schema
   */
  Schema& GetSchema() noexcept override { return schema_; }

 private:
  std::shared_ptr<Plan> plan_;
  Schema schema_;
};
}  // namespace simpledb
