#pragma once

#include <memory>

#include "query/scan.h"
#include "record/schema.h"

namespace simpledb {
/**
 * The interface implemented by each query plan. There is a `Plan` class for
 * each relational algebra operator.
 */
class Plan {
 public:
  virtual ~Plan() = default;

  /**
   * @brief Open a scan corresponding to this plan. The scan will be positioned
   * before its first record.
   * @return a scan
   */
  virtual std::unique_ptr<Scan> Open() = 0;

  /**
   * @brief Return an estimate of the number of block accesses that will occur
   * when the scan is read to completion.
   * @return the estimated number of block accesses
   */
  virtual int BlocksAccessed() const noexcept = 0;

  /**
   * @brief Return an estimate of the number of records in the query's output
   * table
   * @return the estimated number of output records
   */
  virtual int RecordsOutput() const noexcept = 0;

  /**
   * @brief Return an estimate of the number of distinct values for the
   * specified field in the query's output table
   * @param field_name name of the field
   * @return the estimated number of distinct field values in the output
   */
  virtual int DistinctValues(std::string_view field_name) const noexcept = 0;

  /**
   * @brief Return the schema of the query
   * @return the query's schema
   */
  virtual Schema& GetSchema() noexcept = 0;
};
}  // namespace simpledb
