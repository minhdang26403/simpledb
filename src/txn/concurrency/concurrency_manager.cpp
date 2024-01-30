#include "txn/concurrency/concurrency_manager.h"

namespace simpledb {
// Define class static variable
LockTable ConcurrencyManager::lock_table_{};

void ConcurrencyManager::SharedLock(const BlockId& block) {
  if (!locks_.contains(block)) {
    lock_table_.SharedLock(block);
    locks_.emplace(block, "S");
  }
}

void ConcurrencyManager::ExclusiveLock(const BlockId& block) {
  if (!HasExclusiveLock(block)) {
    SharedLock(block);
    lock_table_.ExclusiveLock(block);
    locks_[block] = "X";
  }
}

void ConcurrencyManager::Release() {
  for (const auto& [block, _] : locks_) {
    lock_table_.Unlock(block);
  }
  locks_.clear();
}

bool ConcurrencyManager::HasExclusiveLock(const BlockId& block) const noexcept {
  return locks_.contains(block) && locks_.at(block) == "X";
}
}  // namespace simpledb
