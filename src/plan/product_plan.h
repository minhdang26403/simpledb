#pragma once

#include <memory>
#include <string_view>
#include <utility>

#include "plan/plan.h"
#include "record/schema.h"

namespace simpledb {
/**
 * The `Plan` class corresponding to the `product` relational algebra operator
 */
class ProductPlan : public Plan {
 public:
  /**
   * @brief Create a new product node in the query tree, having the two
   * specified subqueries
   * @param plan1 the left-hand subquery
   * @param plan2 the right-hand subquery
   */
  ProductPlan(std::unique_ptr<Plan> plan1, std::unique_ptr<Plan> plan2)
      : plan1_(std::move(plan1)), plan2_(std::move(plan2)) {
    schema_.AddAll(plan1_->GetSchema());
    schema_.AddAll(plan2_->GetSchema());
  }

  /**
   * @brief Create a `product` scan for this query
   * @return the `product` scan
   */
  std::unique_ptr<Scan> Open() noexcept override {
    return std::make_unique<Scan>(plan1_->Open(), plan2_->Open());
  }

  /**
   * @brief Estimate the number of block accesses in the product
   * @return the number of block accesses
   */
  int BlocksAccessed() const noexcept override {
    return plan1_->BlocksAccessed() +
           plan1_->RecordsOutput() * plan2_->BlocksAccessed();
  }

  /**
   * @brief Estimate the number of output records in the product
   * @return the number of output records
   */
  int RecordsOutput() const noexcept override {
    return plan1_->RecordsOutput() * plan2_->RecordsOutput();
  }

  /**
   * @brief Estimate the distinct number of field values in the product. Since
   * the product does not increase or decrease field values, the estimate is the
   * same as in the appropriate underlying query
   * @param field_name the field to retrieve its number of distinct values
   * @return the number of distinct values
   */
  int DistinctValues(std::string_view field_name) const noexcept override {
    if (plan1_->GetSchema().HasField(field_name)) {
      return plan1_->DistinctValues(field_name);
    }
    return plan2_->DistinctValues(field_name);
  }

  /**
   * @brief Return the schema of the product, which is the union of the schemas
   * of the underlying queries
   * @return the product's schema
   */
  Schema& GetSchema() noexcept override { return schema_; }

 private:
  std::unique_ptr<Plan> plan1_;
  std::unique_ptr<Plan> plan2_;
  Schema schema_;
};
}  // namespace simpledb
