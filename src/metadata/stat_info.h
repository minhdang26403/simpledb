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
   * @brief Create a StatInfo object
   * @param num_blocks number of blocks in the table
   * @param num_records number of records in the table
   * @param num_distinct_vals number of distinct values of each field in the
   * table
   */
  StatInfo(int num_blocks, int num_records,
           const StringHashMap<int>& num_distinct_values)
      : num_blocks_(num_blocks),
        num_records_(num_records),
        num_distinct_values_(num_distinct_values) {}

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
   * @brief Return the number of distinct values of a field
   * @param field_name the field to retrieve its number of distinct values
   * @return the number of distinct values
   */
  int DistinctValues(std::string_view field_name) const {
    auto entry = num_distinct_values_.find(field_name);
    if (entry == num_distinct_values_.end()) {
      throw std::runtime_error("The field does not exist");
    }
    return entry->second;
  }

 private:
  int num_blocks_;
  int num_records_;
  StringHashMap<int> num_distinct_values_;
};
}  // namespace simpledb
