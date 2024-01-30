#pragma once

#include <string>
#include <unordered_map>

#include "txn/concurrency/lock_table.h"

namespace simpledb {
/**
 * The concurrency manager for the transaction. Each transaction has its own
 * concurrency manager. The concurrency manager keeps track of which locks the
 * transaction currently has, and interacts with the global lock table as
 * needed.
 */
class ConcurrencyManager {
 public:
  /**
   * @brief Obtain a SharedLock on the block, if necessary. The method will ask
   * the lock table for a SharedLock if the transaction currently has no locks
   * on that block.
   * @param block a reference to the disk block
   */
  void SharedLock(const BlockId& block);

  /**
   * @brief Obtain an ExclusiveLock on the block, if necessary. If the
   * transaction does not have an ExclusiveLock on that block, then the method
   * first gets a SharedLock on that block (if necessary) and then upgrades it
   * to an ExclusiveLock.
   * @param block a reference to the disk block
   */
  void ExclusiveLock(const BlockId& block);

  /**
   * @brief Release all locks held by the transaction
   */
  void Release();

 private:
  /**
   * @brief Return whether the transaction is holding an exclusive lock
   * @param block a reference to the disk block
   * @return true if the transaction holds the exclusive lock; otherwise, false
   */
  bool HasExclusiveLock(const BlockId& block) const noexcept;

  // The global lock table. This variable is static because all transactions
  // share the same table.
  static LockTable lock_table_;
  std::unordered_map<BlockId, std::string> locks_;
};
}  // namespace simpledb
