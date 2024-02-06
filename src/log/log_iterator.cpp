#include "log/log_iterator.h"

namespace simpledb {
LogIterator::LogIterator(FileManager& file_manager, const BlockId& block)
    : file_manager_(file_manager),
      block_(block),
      page_(file_manager_.BlockSize()) {
  MoveToBlock(block_);
}

std::span<char> LogIterator::Next() noexcept {
  if (current_pos_ == file_manager_.BlockSize()) {
    block_ = BlockId{block_.Filename(), block_.BlockNumber() - 1};
    MoveToBlock(block_);
  }
  auto log_record = page_.GetBytes(current_pos_);
  // a blob contains: the number of bytes and the bytes themselves
  current_pos_ += sizeof(int) + log_record.size();
  return log_record;
}

void LogIterator::MoveToBlock(const BlockId& block) {
  file_manager_.Read(block, page_);
  current_pos_ = page_.GetInt(0);
}
}  // namespace simpledb
