#include "buffer/buffer.h"

namespace simpledb {
void Buffer::SetModified(int txn_id, int lsn) noexcept {
  txn_id_ = txn_id;
  if (lsn >= 0) {
    lsn_ = lsn;
  }
}

void Buffer::AssignToBlock(const BlockId& block) {
  Flush();
  block_opt_ = block;
  file_manager_.Read(block_opt_.value(), contents_);
  pin_count_ = 0;
}

void Buffer::Flush() {
  if (txn_id_ >= 0) {
    log_manager_.Flush(lsn_);
    file_manager_.Write(block_opt_.value(), contents_);
    txn_id_ = -1;
  }
}

}  // namespace simpledb
