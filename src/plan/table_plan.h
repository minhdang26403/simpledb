#pragma once

#include <memory>
#include <string>

#include "metadata/metadata_manager.h"
#include "metadata/stat_info.h"
#include "plan/plan.h"
#include "record/layout.h"
#include "record/table_scan.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The `Plan` class corresponding to a table
 */
class TablePlan : public Plan {
 public:
  /**
   * @brief Create a leaf node in the query tree corresponding to the specified
   * table
   * @param txn the calling transaction
   * @param table_name the table name
   * @param metadata metadata manager of the database engine
   */
  TablePlan(Transaction& txn, std::string_view table_name,
            MetadataManager& metadata)
      : table_name_(table_name),
        txn_(txn),
        layout_(metadata.GetLayout(table_name_, txn_)),
        stat_info_(metadata.GetStatInfo(table_name_, layout_, txn_)) {}

  /**
   * @brief Create a table scan for this query
   * @return the table scan
   */
  std::unique_ptr<Scan> Open() override {
    return std::make_unique<TableScan>(txn_, table_name_, layout_);
  }

  /**
   * @brief Estimate the number of block accesses for the table, which is
   * obtainable from the statistics manager
   * @return the number of block accesses
   */
  int BlocksAccessed() const noexcept override {
    return stat_info_.BlocksAccessed();
  }

  /**
   * @brief Estimate the number of records in the table, which is obtainable
   * from the statistics manager
   * @return the number of records
   */
  int RecordsOutput() const noexcept override {
    return stat_info_.RecordsOutput();
  }

  /**
   * @brief Estimate the number of distinct field values in the table, which is
   * obtainable from the statistics manager
   * @param field_name the field to retrieve its number of distinct values
   * @return the number of distinct values
   */
  int DistinctValues(std::string_view field_name) const noexcept override {
    return stat_info_.DistinctValues(field_name);
  }

  /**
   * @brief Determine the schema of the table, which is obtainable from the
   * catalog manager
   * @return the table's schema
   */
  Schema& GetSchema() noexcept override { return layout_.GetSchema(); }

 private:
  std::string table_name_;
  Transaction& txn_;
  Layout layout_;
  StatInfo stat_info_;
};
}  // namespace simpledb
