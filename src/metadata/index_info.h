#pragma once

#include <memory>
#include <string>

#include "index/btree/btree_index.h"
#include "index/hash/hash_index.h"
#include "metadata/stat_info.h"
#include "record/layout.h"
#include "record/schema.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The information about an index. This information is used by the query planner
 * in order to estimate the costs of using the index and to obtain the layout of
 * the index records.
 */
class IndexInfo {
 public:
  /**
   * @brief Create an `IndexInfo` object for the specified index
   * @param index_name name of the index
   * @param field_name name of the indexed field
   * @param table_schema schema of the table
   * @param txn the calling transaction
   * @param stat_info statistics of the table
   */
  IndexInfo(std::string_view index_name, std::string_view field_name,
            Schema& table_schema, Transaction& txn, StatInfo& stat_info);

  /**
   * @brief Open the index described by this object
   * @return the Index object associated with this information
   */
  std::unique_ptr<Index> Open() const {
    return std::make_unique<BTreeIndex>(txn_, index_name_, index_layout_);
  }

  /**
   * @brief Estimate the number of block accesses required to find all index
   * records having a particular search key. The method uses the table's
   * metadata to estimate the size of the index file and the number of index
   * records per block. It then passes this information to the `SearchCost`
   * method of the appropriate index type, which provides the estimate.
   * @return the number of block accesses required to traverse the index
   */
  int BlocksAccessed() const noexcept;

  /**
   * @brief Return the estimated number of records having a search key. This
   * value is the same as doing a select query; that is, it is the number of
   * records in the table divided by the number of distinct values of the
   * indexed field.
   * @return the estimated number of records having a search key
   */
  int RecordsOutput() const;

  /**
   * @brief Return the number of distinct values for a specified field in the
   * underlying table or 1 for the indexed field
   * @param field_name the specified field
   * @return the number of distinct values
   */
  int DistinctValues(std::string_view field_name) const;

 private:
  /**
   * @brief Return the layout of the index records. The schema consists of the
   * `DataRID` (which is represented as two integers, the block number and the
   * record ID) and the `data_val` (which is the indexed field). Schema
   * information about the indexed field is obtained via the table's schema.
   * @return the layout of the index records
   */
  Layout CreateIndexLayout();

  std::string index_name_;
  std::string field_name_;
  Transaction& txn_;
  Schema& table_schema_;
  Layout index_layout_;
  StatInfo& stat_info_;
};
}  // namespace simpledb
