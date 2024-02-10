#include "txn/transaction.h"

#include <iostream>

#include "buffer/buffer.h"
#include "file/block_id.h"

namespace simpledb {
int Transaction::next_txn_id_ = 0;

void Transaction::Commit() {
  recovery_manager_.Commit();
  std::cout << "Transaction " << txn_id_ << " committed\n";
  concurrency_manager_.Release();
  my_buffers_.UnpinAll();
}

void Transaction::Rollback() {
  recovery_manager_.Rollback();
  std::cout << "Transaction " << txn_id_ << " rolled back\n";
  concurrency_manager_.Release();
  my_buffers_.UnpinAll();
}

void Transaction::Recover() {
  buffer_manager_.FlushAll(txn_id_);
  recovery_manager_.Recover();
}

int Transaction::GetInt(const BlockId& block, int offset) {
  concurrency_manager_.SharedLock(block);
  auto buffer = my_buffers_.GetBuffer(block);
  if (buffer == nullptr) {
    throw std::runtime_error(
        "GetInt: The transaction has not pinned the block");
  }
  return buffer->Contents().GetInt(offset);
}

std::string_view Transaction::GetString(const BlockId& block, int offset) {
  concurrency_manager_.SharedLock(block);
  auto buffer = my_buffers_.GetBuffer(block);
  if (buffer == nullptr) {
    throw std::runtime_error(
        "GetString: The transaction has not pinned the block");
  }
  return buffer->Contents().GetString(offset);
}

void Transaction::SetInt(const BlockId& block, int offset, int val,
                         bool OkToLog) {
  concurrency_manager_.ExclusiveLock(block);
  auto buffer = my_buffers_.GetBuffer(block);
  if (buffer == nullptr) {
    throw std::runtime_error(
        "SetInt: The transaction has not pinned the block");
  }
  int lsn = -1;
  if (OkToLog) {
    lsn = recovery_manager_.SetInt(buffer, offset);
  }
  buffer->Contents().SetInt(offset, val);
  buffer->SetModified(txn_id_, lsn);
}

void Transaction::SetString(const BlockId& block, int offset,
                            std::string_view val, bool OkToLog) {
  concurrency_manager_.ExclusiveLock(block);
  auto buffer = my_buffers_.GetBuffer(block);
  if (buffer == nullptr) {
    throw std::runtime_error(
        "SetString: The transaction has not pinned the block");
  }
  int lsn = -1;
  if (OkToLog) {
    lsn = recovery_manager_.SetString(buffer, offset);
  }
  buffer->Contents().SetString(offset, val);
  buffer->SetModified(txn_id_, lsn);
}

int Transaction::Size(std::string_view filename) {
  BlockId dummy_block{filename, END_OF_FILE};
  concurrency_manager_.SharedLock(dummy_block);
  return file_manager_.Length(filename);
}

BlockId Transaction::Append(std::string_view filename) {
  BlockId dummy_block{filename, END_OF_FILE};
  concurrency_manager_.ExclusiveLock(dummy_block);
  return file_manager_.Append(filename);
}
}  // namespace simpledb
