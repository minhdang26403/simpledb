#include "txn/buffer_list.h"

#include "buffer/buffer.h"
#include "file/block_id.h"

namespace simpledb {
Buffer* BufferList::GetBuffer(const BlockId& block) const noexcept {
  if (!buffers_.contains(block)) {
    return nullptr;
  }

  return buffers_.at(block);
}

void BufferList::Pin(const BlockId& block) {
  Buffer* buffer = buffer_manager_.Pin(block);
  if (buffer == nullptr) {
    throw std::runtime_error("No available buffer!");
  }
  buffers_[block] = buffer;
}

void BufferList::Unpin(const BlockId& block) {
  Buffer* buffer = buffers_.at(block);
  buffer_manager_.Unpin(buffer);
  buffers_.erase(block);
}

void BufferList::UnpinAll() {
  for (const auto& [_, buffer] : buffers_) {
    buffer_manager_.Unpin(buffer);
  }
  buffers_.clear();
}
}  // namespace simpledb
