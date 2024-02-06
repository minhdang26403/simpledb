#include "txn/concurrency/lock_table.h"

namespace simpledb {
void LockTable::SharedLock(const BlockId& block) {
  std::unique_lock lock{mutex_};
  auto timestamp = system_clock::now();
  while (HasExclusiveLock(block) && !WaitingTooLong(timestamp)) {
    cv_.wait_for(lock, MAX_TIME);
  }
  if (HasExclusiveLock(block)) {
    throw LockAbortException();
  }
  locks_[block]++;
}

void LockTable::ExclusiveLock(const BlockId& block) {
  std::unique_lock lock{mutex_};
  auto timestamp = system_clock::now();
  while (HasOtherSharedLocks(block) && !WaitingTooLong(timestamp)) {
    cv_.wait_for(lock, MAX_TIME);
  }
  if (HasOtherSharedLocks(block)) {
    throw LockAbortException();
  }
  locks_[block] = -1;
}

void LockTable::Unlock(const BlockId& block) {
  std::scoped_lock lock{mutex_};
  int val = GetLockVal(block);
  if (val > 1) {
    locks_[block] = val - 1;
  } else {
    locks_.erase(block);
    cv_.notify_all();
  }
}

bool LockTable::WaitingTooLong(
    const time_point<system_clock>& start_time) const noexcept {
  return duration_cast<milliseconds>(system_clock::now() - start_time) >
         MAX_TIME;
}

bool LockTable::HasExclusiveLock(const BlockId& block) const noexcept {
  return GetLockVal(block) < 0;
}

bool LockTable::HasOtherSharedLocks(const BlockId& block) const noexcept {
  return GetLockVal(block) > 1;
}

int LockTable::GetLockVal(const BlockId& block) const noexcept {
  if (!locks_.contains(block)) {
    return 0;
  }
  return locks_.at(block);
}
}  // namespace simpledb
