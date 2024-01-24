#pragma once

#include <string_view>

#include "buffer/buffer_manager.h"
#include "file/file_manager.h"
#include "log/log_manager.h"

namespace simpledb {
/**
 * The SimpleDB database
 */
class SimpleDB {
 public:
  /**
   * @brief Construct a new Simple DB object that represents an instance of the
   * running database
   * @param dirname the directory to hold the database
   * @param block_size size of a disk block
   * @param buff_size number of buffers in the buffer pool
   */
  SimpleDB(std::string_view dirname, int block_size, int buff_size)
      : file_manager_(dirname, block_size),
        log_manager_(file_manager_, LOG_FILE),
        buffer_manager_(file_manager_, log_manager_, buff_size) {}

  explicit SimpleDB(std::string_view dirname);

  /**
   * @brief Get the file manager
   * @return a reference to the file manager
   */
  FileManager& GetFileManager() noexcept { return file_manager_; }

  /**
   * @brief Get the log manager
   * @return a reference to the log manager
   */
  LogManager& GetLogManager() noexcept { return log_manager_; }

  /**
   * @brief Get the buffer manager
   * @return a reference to the buffer manager
   */
  BufferManager& GetBufferManager() noexcept {
    return buffer_manager_;
  }

 private:
  static constexpr std::string_view LOG_FILE{"simpledb.log"};
  static constexpr int BLOCK_SIZE{400};
  static constexpr int BUFFER_SIZE{8};
  FileManager file_manager_;
  LogManager log_manager_;
  BufferManager buffer_manager_;
};
}  // namespace simpledb
