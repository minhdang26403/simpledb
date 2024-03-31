#pragma once

#include <optional>
#include <string>

#include "index/index.h"
#include "query/constant.h"
#include "record/layout.h"
#include "record/rid.h"
#include "record/table_scan.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * A static hash implementation of the `Index` interface. A fixed number of
 * buckets is allocated (currently, 100), and each bucket is implemented as a
 * file of index records.
 */
class HashIndex : public Index {
 public:
  /**
   * @brief Open a hash index for the specified index
   * @param txn the calling transaction
   * @param index_name name of the index
   * @param layout schema of the index records
   */
  HashIndex(Transaction& txn, std::string_view index_name, const Layout& layout)
      : txn_(txn), index_name_(index_name), layout_(layout) {}

  /**
   * @brief Position the index before the first index record having the
   * specified search key.
   * The method hashes the search key to determine the bucket and then opens a
   * table scan on the file corresponding to the bucket.
   * The table scan for the previous bucket (if any) is closed.
   * @param search_key the search key value
   */
  void BeforeFirst(const Constant& search_key) override;

  /**
   * @brief Move to the next record having the search key. The method loops
   * through the table scan for the bucket to look for a matching record
   * @return false if there are no more such index records
   */
  bool Next() override;

  /**
   * @brief Return the RID from the current record in the table scan for the
   * bucket
   * @return the RID stored in the current index record
   */
  RID GetRID() override;

  /**
   * @brief Insert a new record into the table scan for the bucket
   * @param key the key of the new index record
   * @param rid the RID of the new index record
   */
  void Insert(const Constant& key, const RID& rid) override;

  /**
   * @brief Delete the specified record form the table scan for the bucket. The
   * method starts at the beginning of the scan, and loops through the records
   * until the specified record is found
   * @param key the key of the deleted index record
   * @param rid the RID of the deleted index record
   */
  void Delete(const Constant& key, const RID& rid) override;

  /**
   * @brief Close the index by closing the current table scan
   */
  void Close() override;

  /**
   * @brief Return the cost of searching an index file having the specified
   * number of blocks. The method assumes that all buckets are about the same
   * size, so the cost is simply the size of the bucket.
   * @param num_blocks number of blocks of index records
   * @param records_per_block number of records per block (not used here)
   * @return the cost of traversing the index
   */
  static int SearchCost(int num_blocks,
                        [[maybe_unused]] int records_per_block) noexcept {
    return num_blocks / NUM_BUCKETS;
  }

 private:
  size_t KeyToBucketID(const Constant& key) const {
    return std::hash<std::string>{}(key.ToString()) % NUM_BUCKETS;
  }

  static constexpr int NUM_BUCKETS = 100;

  Transaction& txn_;
  std::string index_name_;
  Layout layout_;
  std::optional<Constant> search_key_;
  std::optional<TableScan> table_scan_;
};
}  // namespace simpledb
