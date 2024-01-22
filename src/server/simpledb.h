#pragma once

#include <filesystem>
#include <string_view>

#include "file/file_manager.h"

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
   * @param block_size block size of the database
   * @param buff_size number of buffers in the buffer pool
   */
  SimpleDB(std::string_view dirname, int block_size, int buff_size)
      : file_manager_(FileManager(std::filesystem::path{dirname}, block_size)) {
    // stop compiler warning
    // TODO(DANG): remove this line
    buff_size = buff_size + 1;
  }

  /**
   * @brief Get the file manager
   * @return a reference to the file manager
   */
  FileManager& GetFileManager();

 private:
  FileManager file_manager_;
};
}  // namespace simpledb
