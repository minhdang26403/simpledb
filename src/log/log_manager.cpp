#include "log/log_manager.h"

#include "file/block_id.h"
#include "file/file_manager.h"
#include "log/log_iterator.h"

namespace simpledb {
LogManager::LogManager(FileManager& file_manager, std::string_view log_file)
    : file_manager_(file_manager),
      log_file_(log_file),
      log_page_(file_manager_.BlockSize()) {
  int log_size = file_manager_.Length(log_file_);
  if (log_size == 0) {
    current_block_ = AppendNewBlock();
  } else {
    current_block_ = BlockId{log_file_, log_size - 1};
    file_manager_.Read(current_block_, log_page_);
  }
}

void LogManager::Flush(int lsn) {
  if (lsn > last_saved_lsn_) {
    Flush();
  }
}

LogIterator LogManager::Iterator() {
  Flush();

  return LogIterator{file_manager_, current_block_};
}

int LogManager::Append(std::span<char> log_record) {
  std::scoped_lock lock{mutex_};
  int boundary = log_page_.GetInt(0);
  int record_size = log_record.size();
  int len_size = sizeof(int);
  int bytes_needed = record_size + len_size;

  // the log record doesn't fit
  if (boundary - bytes_needed < len_size) {
    Flush();
    current_block_ = AppendNewBlock();
    boundary = log_page_.GetInt(0);
  }

  int record_pos = boundary - bytes_needed;
  log_page_.SetBytes(record_pos, log_record);
  log_page_.SetInt(0, record_pos);  // the new boundary
  latest_lsn_++;

  return latest_lsn_;
}

BlockId LogManager::AppendNewBlock() {
  auto block = file_manager_.Append(log_file_);
  log_page_.SetInt(0, file_manager_.BlockSize());
  file_manager_.Write(block, log_page_);

  return block;
}

void LogManager::Flush() {
  file_manager_.Write(current_block_, log_page_);
  last_saved_lsn_ = latest_lsn_;
}
}  // namespace simpledb
