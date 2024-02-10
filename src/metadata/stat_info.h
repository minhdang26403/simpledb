#pragma once

#include <string_view>

#include "utils/data_type.h"

namespace simpledb {
/**
 * A StatInfo object holds three pieces of statistical information about a
 * table: the number of blocks, the number of records, and the number of
 * distinct values for each field.
 */
class StatInfo {
 public:
  /**
   * @brief Create a StatInfo object. Note that the number of distinct values is
   * not passed into the constructor. The object fakes this value.
   * @param num_blocks number of blocks in the table
   * @param num_records number of records in the table
   */
  StatInfo(int num_blocks, int num_records)
      : num_blocks_(num_blocks), num_records_(num_records) {}

  /**
   * @brief Return the estimated number of blocks in the table
   * @return the estimated number of blocks in the table
   */
  int BlocksAccessed() const noexcept { return num_blocks_; }

  /**
   * @brief Return the estimated number of records in the table
   * @return the estimated number of records in the table
   */
  int RecordsOutput() const noexcept { return num_records_; }

  /**
   * @brief Return the estimated number of distinct values for the specified
   * field. This estimate is a complete guess because doing something reasonable
   * is beyond the scope of this system.
   * @param field_name the field to retrieve its number of distinct values
   * @return the number of distinct values
   */
  int DistinctValues([[maybe_unused]] std::string_view field_name) const {
    return 1 + num_records_ / 3;
  }

 private:
  int num_blocks_;
  int num_records_;
};
}  // namespace simpledb
