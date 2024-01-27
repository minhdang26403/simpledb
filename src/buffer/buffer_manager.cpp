#include "buffer/buffer_manager.h"

#include <chrono>
#include <mutex>
#include <optional>

#include "file/block_id.h"
#include "file/file_manager.h"
#include "log/log_manager.h"

namespace simpledb {
BufferManager::BufferManager(FileManager& file_manager, LogManager& log_manager,
                             int num_buffs)
    : num_available_(num_buffs) {
  buffer_pool_.reserve(num_buffs);
  for (int i = 0; i < num_buffs; i++) {
    buffer_pool_.emplace_back(file_manager, log_manager);
  }
}

int BufferManager::Available() const {
  std::scoped_lock lock{mutex_};
  return num_available_;
}

void BufferManager::FlushAll(int txn_id) {
  std::scoped_lock lock{mutex_};
  for (auto& buffer : buffer_pool_) {
    if (buffer.ModifyingTxn() == txn_id) {
      buffer.Flush();
    }
  }
}

void BufferManager::Unpin(Buffer* buffer) {
  std::scoped_lock lock{mutex_};
  buffer->Unpin();
  if (!buffer->IsPinned()) {
    num_available_++;
    cv_.notify_all();
  }
}

Buffer* BufferManager::Pin(const BlockId& block) {
  std::unique_lock lock{mutex_};
  auto timestamp = system_clock::now();
  Buffer* buffer = TryToPin(block);

  while (buffer == nullptr && !WaitingTooLong(timestamp)) {
    cv_.wait_for(lock, MAX_TIME);
    buffer = TryToPin(block);
  }

  return buffer;
}

bool BufferManager::WaitingTooLong(
    const time_point<system_clock>& start_time) const noexcept {
  return duration_cast<milliseconds>(system_clock::now() - start_time) >
         MAX_TIME;
}

Buffer* BufferManager::TryToPin(const BlockId& block) {
  Buffer* buffer = FindExistingBuffer(block);
  if (buffer == nullptr) {
    buffer = ChooseUnpinnedBuffer();
    if (buffer == nullptr) {
      return nullptr;
    }
    buffer->AssignToBlock(block);
  }

  if (!buffer->IsPinned()) {
    num_available_--;
  }
  buffer->Pin();

  return buffer;
}

Buffer* BufferManager::FindExistingBuffer(const BlockId& block) noexcept {
  for (auto& buffer : buffer_pool_) {
    std::optional<BlockId> block_opt = buffer.Block();
    if (block_opt.has_value() && block_opt.value() == block) {
      return &buffer;
    }
  }

  return nullptr;
}

Buffer* BufferManager::ChooseUnpinnedBuffer() noexcept {
  for (auto& buffer : buffer_pool_) {
    if (!buffer.IsPinned()) {
      return &buffer;
    }
  }

  return nullptr;
}
}  // namespace simpledb
