#pragma once

#include <mutex>  // NOLINT(build/c++11)

#include "metadata/stat_info.h"
#include "metadata/table_manager.h"
#include "utils/data_type.h"

namespace simpledb {
/**
 * The statistics manager is responsible for keeping statistical information
 * about each table. The manager does not store this information in the
 * database. Instead, it calculates this information on system startup, and
 * periodically refreshes it.
 */
class StatManager {
 public:
  /**
   * @brief Create a statistics manager
   * @param table_manager a reference to the table manager
   * @param txn the startup transaction
   */
  StatManager(TableManager& table_manager, Transaction& txn);

  /**
   * @brief Return the statistical information about the specified table
   * @param table_name name of the table
   * @param layout the table's layout
   * @param txn the transaction that requests statistics
   * @return the statistical information about the table
   */
  StatInfo GetStatInfo(std::string_view table_name, Layout& layout,
                       Transaction& txn);

 private:
  /**
   * @brief Recalculate the statistics for all tables in the database
   * @param txn the transaction that requests statistics
   */
  void RefreshStatistics(Transaction& txn);

  /**
   * @brief Calculate the statistics for a table
   * @param table_name name of the table to calculate statistics
   * @param layout layout of the record in the table
   * @param txn the transaction that requests statistics
   * @return a StatInfo object that holds the number of blocks, records, and
   * distinct values of each field in the table
   */
  StatInfo CalculateTableStats(std::string_view table_name, Layout& layout,
                               Transaction& txn);

  TableManager& table_manager_;
  StringHashMap<StatInfo> table_stats_;
  int num_calls_{};
  std::mutex mutex_;
};
}  // namespace simpledb
