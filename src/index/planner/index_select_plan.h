#pragma once

#include <memory>

#include "index/query/index_select_scan.h"
#include "metadata/index_info.h"
#include "plan/plan.h"
#include "plan/table_plan.h"
#include "query/scan.h"

namespace simpledb {
/**
 * The `Plan` class corresponding to the `index select` relational algebra
 * operator
 */
class IndexSelectPlan : public Plan {
 public:
  /**
   * @brief Create a new `index select` node in the query tree for the specified
   * index and selection constant
   * @param plan the input table
   * @param index_info information about the index
   * @param value the selection constant
   */
  IndexSelectPlan(const std::shared_ptr<Plan>& plan,
                  const std::shared_ptr<IndexInfo>& index_info,
                  const Constant& value)
      : plan_(plan), index_info_(index_info), value_(value) {}

  /**
   * @brief Create a new `index select` scan for this query
   * @return an index select scan
   */
  std::unique_ptr<Scan> Open() override;

  /**
   * Estimate the number of block accesses to compute the index selection,
   * which is the same as the index traversal cost plus the number of matching
   * data records.
   * @return the estimated number of block accesses
   */
  int BlocksAccessed() const noexcept override;

  /**
   * @brief Estimate the number of output records in the index selection,
   * which is the same as the number of search key values
   * for the index
   * @return the estimated number of output records
   */
  int RecordsOutput() const noexcept override;

  /**
   * @brief Return the distinct values as defined by the index.
   * @param field_name name of a field
   * @return the estimated number of distinct field values in the output
   */
  int DistinctValues(std::string_view field_name) const noexcept override;

  /**
   * @brief Return the schema of the data table
   * @return the data table's schema
   */
  Schema& GetSchema() noexcept override;

 private:
  std::shared_ptr<Plan> plan_;
  std::shared_ptr<IndexInfo> index_info_;
  Constant value_;
};
}  // namespace simpledb
