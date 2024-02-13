#pragma once

#include "query/scan.h"
#include "record/schema.h"

namespace simpledb {
/**
 * The interface implemented by each query plan. There is a `Plan` class for
 * each relational algebra operator.
 */
class Plan {
 public:
  // TODO(DANG): Mark const, noexcept for methods and check return type of
  // `GetSchema`
  virtual ~Plan() = default;

  /**
   * @brief Open a scan corresponding to this plan. The scan will be positioned
   * before its first record.
   * @return a scan
   */
  virtual Scan Open() = 0;

  /**
   * @brief Return an estimate of the number of block accesses that will occur
   * when the scan is read to completion.
   * @return the estimated number of block accesses
   */
  virtual int BlocksAccessed() = 0;

  /**
   * @brief Return an estimate of the number of records in the query's output
   * table
   * @return the estimated number of output records
   */
  virtual int RecordsOutput() = 0;

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
  virtual Schema& GetSchema() = 0;
};
}  // namespace simpledb
