#pragma once

#include <chrono>              // NOLINT(build/c++11)
#include <condition_variable>  // NOLINT(build/c++11)
#include <exception>
#include <mutex>  // NOLINT(build/c++11)
#include <unordered_map>

#include "file/block_id.h"

namespace simpledb {
/**
 * A runtime exception indicating that the transaction needs to abort because a
 * lock could not be obtained
 */
class LockAbortException : public std::exception {};

using namespace std::chrono;  // NOLINT(build/namespaces)

/**
 * The lock table, which provides methods to lock and unlock blocks. If a
 * transaction requests a lock that causes a conflict with an existing lock,
 * then that transaction is placed on a wait list. There is only one wait list
 * for all blocks. When the last lock on a block is unlocked, then all
 * transactions are removed from the wait list and rescheduled. If one of those
 * transactions discovers that the lock it is waiting for is still locked, it
 * will place itself back on the wait list.
 */
class LockTable {
 public:
  /**
   * @brief Grant a SharedLock on the specified block. If an ExclusiveLock
   * exists when the method is called, then the calling thread will be placed on
   * a wait list until the lock is released. If the thread remains on the wait
   * list for a certain amount of time (currently 10 seconds), then an exception
   * is thrown.
   * @param block a reference to the disk block
   */
  void SharedLock(const BlockId& block);

  /**
   * @brief Grant an ExclusiveLock on the specified block. If a lock of any type
   * exists when the method is called, then the calling thread will placed on a
   * wait list until the locks are released. If the thread remains on the wait
   * list for a certain amount of time (currently 10 seconds), then an exception
   * is thrown.
   * Note: If the number of shared locks granted is 1, it's this calling thread
   * holding that shared lock.
   * @param block a reference to the disk block
   */
  void ExclusiveLock(const BlockId& block);

  /**
   * @brief Release a lock on the specified block. If this lock is the last lock
   * on that block, then the waiting transactions are notified.
   * @param block a reference to the disk block
   */
  void Unlock(const BlockId& block);

 private:
  /**
   * @brief Return whether `MAX_TIME` milliseconds has passed since the
   * specified start time
   * @param start_time start time of the duration
   * @return true if `MAX_TIME` milliseconds has passed; otherwise, false
   */
  bool WaitingTooLong(
      const time_point<system_clock>& start_time) const noexcept;

  /**
   * @brief Return whether an exclusive lock is currently assigned to the
   * specified block
   * @param block a reference to the disk block
   * @return true if an exclusive lock is assigned to the block; otherwise,
   * false
   */
  bool HasExclusiveLock(const BlockId& block) const noexcept;

  /**
   * @brief Return whether there are other shared locks also assigned to the
   * specified block. The thread calling this method is currently holding a
   * shared lock. It wants to know whether other threads also hold shared locks.
   * @param block a reference to the disk block
   * @return true if there are more than one shared locks assigned; otherwise,
   * false
   */
  bool HasOtherSharedLocks(const BlockId& block) const noexcept;

  /**
   * @brief Return the lock value associated with the specified block: a value
   * of -1 denotes an exclusive lock; 0 denotes no lock is currently assigned; a
   * positive value denotes the number of shared locks assigned.
   * @param block a reference to the disk block
   * @return a value indicating the locking status of the specified block
   */
  int GetLockVal(const BlockId& block) const noexcept;

  static constexpr milliseconds MAX_TIME = 10000ms;
  std::unordered_map<BlockId, int> locks_{};
  mutable std::mutex mutex_;
  std::condition_variable cv_;
};
}  // namespace simpledb
