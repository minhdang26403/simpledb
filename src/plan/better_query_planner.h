#pragma once

#include <memory>

#include "metadata/metadata_manager.h"
#include "parse/query_data.h"
#include "plan/query_planner.h"

namespace simpledb {
/**
 * A small improvement on the basic query planner
 */
class BetterQueryPlanner final : public QueryPlanner {
 public:
  /**
   * @brief Construct a new `BetterQueryPlanner` object
   * @param metadata_manager the metadata manager
   */
  explicit BetterQueryPlanner(MetadataManager& metadata_manager)
      : metadata_manager_(metadata_manager) {}

  /**
   * @brief Create a query plan as follows. It first takes the product of all
   * tables and views; it then selects on the predicate; and finally it projects
   * on the field list.
   * @param data the parsed representation of the query
   * @param txn the calling transaction
   * @return a plan for that query
   */
  std::shared_ptr<Plan> CreatePlan(const QueryData& data,
                                   Transaction& txn) override;

 private:
  MetadataManager& metadata_manager_;
};
}  // namespace simpledb
