#pragma once

#include <memory>

#include "parse/query_data.h"
#include "plan/plan.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The interface implemented by planners for the SQL select statement
 */
class QueryPlanner {
 public:
  virtual ~QueryPlanner() = default;

  /**
   * @brief Create a plan for the parsed query
   * @param data the parsed representation of the query
   * @param txn the calling transaction
   * @return a plan for that query
   */
  virtual std::shared_ptr<Plan> CreatePlan(const QueryData& data,
                                           Transaction& txn) const = 0;
};
}  // namespace simpledb
