#pragma once

#include <mutex>
#include <span>
#include <string>
#include <string_view>

#include "file/block_id.h"
#include "file/file_manager.h"
#include "file/page.h"
#include "log/log_iterator.h"

namespace simpledb {
/**
 * The log manager is responsible for writing log records into a log file. The
 * tail of the log is kept in an in-memory log buffer, which is flushed to disk
 * when needed.
 */
class LogManager {
 public:
  /**
   * @brief Create the manager for the specified log file. If the log file does
   * not yet exist, it is created with an empty first block.
   * @param file_manager file manager of the database engine
   * @param log_file name of the log file
   */
  LogManager(FileManager& file_manager, std::string_view log_file);

  /**
   * @brief Ensures that the log record corresponding to the specified LSN has
   * been written to disk. All earlier log records will also be written to disk.
   * @param lsn the LSN of a log record
   */
  void Flush(int lsn);

  /**
   * @brief Get a log iterator to traverse through log records in the current
   * block.
   * @return an iterator to the most recent log record of the current block
   */
  LogIterator Iterator();

  /**
   * @brief Append a log record to the log buffer. The record consists of an
   * arbitrary array of bytes. Log records are written right to left in the
   * buffer. The size of the record is written before the bytes. The beginning
   * of the buffer contains the location of the last-written record (the
   * "boundary"). Storing the records backwards makes it easy to read them in
   * reverse order.
   * @param log_record the log record to write
   * @return the LSN of this log record
   */
  int Append(std::span<char> log_record);

 private:
  /**
   * @brief Extend the log file by allocating a new empty disk block.
   * @return a newly allocated block
   */
  BlockId AppendNewBlock();

  /**
   * @brief Flush the in-memory page to the log file.
   */
  void Flush();

  FileManager& file_manager_;
  std::string log_file_;
  Page log_page_;
  BlockId current_block_;
  int latest_lsn_{};
  int last_saved_lsn_{};
  std::mutex mutex_;
};
}  // namespace simpledb
