#pragma once

#include <memory>
#include <string>
#include <utility>

#include "plan/plan.h"
#include "plan/query_planner.h"
#include "plan/update_planner.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The object that executes SQL statements
 */
class Planner {
 public:
  Planner() = default;

  /**
   * @brief Construct a new Planner object
   * @param query_planner the query planner
   * @param update_planner the update planner
   */
  Planner(std::unique_ptr<QueryPlanner> query_planner,
          std::unique_ptr<UpdatePlanner> update_planner)
      : query_planner_(std::move(query_planner)),
        update_planner_(std::move(update_planner)) {}

  /**
   * @brief Create a plan for an SQL `select` statement, using the supplied
   * planner
   * @param query the SQL query string
   * @param txn the transaction
   * @return the scan corresponding to the query plan
   */
  std::shared_ptr<Plan> CreateQueryPlan(const std::string& query,
                                        Transaction& txn) const;

  /**
   * @brief Execute an SQL `insert`, `delete`, `modify`, or `create` statement.
   * The method dispatches to the appropriate method of the supplied update
   * planner, depending on what the parser returns
   * @param command the SQL update string
   * @param txn the transaction
   * @return an integer denoting the number of affected records
   */
  int ExecuteUpdate(const std::string& command, Transaction& txn) const;

 private:
  std::unique_ptr<QueryPlanner> query_planner_;
  std::unique_ptr<UpdatePlanner> update_planner_;
};
}  // namespace simpledb
