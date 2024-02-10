#include "txn/buffer_list.h"

#include "buffer/buffer.h"
#include "file/block_id.h"

namespace simpledb {
Buffer* BufferList::GetBuffer(const BlockId& block) const noexcept {
  if (!buffers_.contains(block)) {
    return nullptr;
  }

  return buffers_.at(block).first;
}

void BufferList::Pin(const BlockId& block) {
  Buffer* buffer = buffer_manager_.Pin(block);
  if (buffer == nullptr) {
    throw std::runtime_error("No available buffer!");
  }
  if (!buffers_.contains(block)) {
    buffers_.emplace(std::piecewise_construct, std::forward_as_tuple(block),
                     std::forward_as_tuple(buffer, 0));
  }
  buffers_.at(block).second++;
}

void BufferList::Unpin(const BlockId& block) {
  auto& [buffer, pin_count] = buffers_.at(block);
  buffer_manager_.Unpin(buffer);
  pin_count--;
  if (pin_count == 0) {
    buffers_.erase(block);
  }
}

void BufferList::UnpinAll() {
  for (const auto& [_, entry] : buffers_) {
    for (int i = 0; i < entry.second; i++) {
      buffer_manager_.Unpin(entry.first);
    }
  }
  buffers_.clear();
}
}  // namespace simpledb
