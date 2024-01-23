#pragma once

#include <filesystem>
#include <string_view>

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
      : file_manager_(FileManager(std::filesystem::path{dirname}, block_size)),
        log_manager_(LogManager(file_manager_, LOG_FILE)) {
    // stop compiler warning
    // TODO(DANG): remove this line
    buff_size = buff_size + 1;
  }

  /**
   * @brief Get the file manager
   * @return a reference to the file manager
   */
  FileManager& GetFileManager();

  /**
   * @brief Get the log manager
   * @return a reference to the log manager
   */
  LogManager& GetLogManager();

 private:
  static constexpr std::string_view LOG_FILE{"simpledb.log"};
  FileManager file_manager_;
  LogManager log_manager_;
};
}  // namespace simpledb
