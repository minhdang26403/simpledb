#pragma once

#include <filesystem>
#include <string_view>

#include "file/file_manager.h"

namespace simpledb {
class SimpleDB {
 public:
  SimpleDB(std::string_view dirname, int block_size, int buff_size)
      : file_manager_(FileManager(std::filesystem::path{dirname}, block_size)) {
    // stop compiler warning
    // TODO(DANG): remove this line
    buff_size = buff_size + 1;
  }

  FileManager& GetFileManager();

 private:
  FileManager file_manager_;
};
}  // namespace simpledb
