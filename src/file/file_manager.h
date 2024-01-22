#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>

#include "file/block_id.h"
#include "file/page.h"

namespace simpledb {
namespace fs = std::filesystem;

struct string_hash {
  using is_transparent = void;
  [[nodiscard]] size_t operator()(const char* s) const {
    return std::hash<std::string_view>{}(s);
  }

  [[nodiscard]] size_t operator()(std::string_view s) const {
    return std::hash<std::string_view>{}(s);
  }

  [[nodiscard]] size_t operator()(const std::string& s) const {
    return std::hash<std::string>{}(s);
  }
};

class FileManager {
 public:
  FileManager(const fs::path& db_directory, int block_size);

  void Read(const BlockId& block, const Page& p);

  void Write(const BlockId& block, const Page& p);

  BlockId Append(std::string_view filename);

  int Length(std::string_view filename) const;

  bool IsNew() const noexcept { return is_new_; }

  int BlockSize() const noexcept { return block_size_; }

 private:
  std::fstream& GetFile(std::string_view filename);

  fs::path db_directory_path_;
  int block_size_{};
  bool is_new_{};
  std::unordered_map<std::string, std::fstream, string_hash, std::equal_to<>>
      open_files_;
  std::mutex mutex_;
};
}  // namespace simpledb
