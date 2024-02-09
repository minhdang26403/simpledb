#pragma once

namespace simpledb {
class HashIndex {
 public:
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
  static constexpr int NUM_BUCKETS = 100;
};
}  // namespace simpledb
