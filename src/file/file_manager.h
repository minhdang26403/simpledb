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

/**
 * Our own version of hash function object to support heterogeneous key lookup
 * for `std::unordered_map`
 */
struct string_hash {
  // need to define the `is_transparent` type to support heterogeneous key
  // lookup
  using is_transparent = void;

  /**
   * @brief Hash a string literal
   * @param s the string literal to hash
   * @return the hash value of this string
   */
  [[nodiscard]] size_t operator()(const char* s) const {
    return std::hash<std::string_view>{}(s);
  }

  /**
   * @brief Hash a string view
   * @param s the string view to hash
   * @return the hash value of this stirng
   */
  [[nodiscard]] size_t operator()(std::string_view s) const {
    return std::hash<std::string_view>{}(s);
  }

  /**
   * @brief Hash a string object
   * @param s the string object to hash
   * @return the hash value of this string
   */
  [[nodiscard]] size_t operator()(const std::string& s) const {
    return std::hash<std::string>{}(s);
  }
};

/**
 * The File Manager handles the actual interaction with the OS file system. It
 * manages OS files as a virtual disk
 */
class FileManager {
 public:
  /**
   * @brief Construct a new File Manager object.
   * @param db_directory path to the directory containing the database
   * @param block_size size of a disk block
   */
  FileManager(const fs::path& db_directory, int block_size);

  /**
   * @brief Read the contents of the specified block into the specified
   * page.
   * @param block the block to read from
   * @param p the page to read to
   */
  void Read(const BlockId& block, const Page& p);

  /**
   * @brief Write the contents of a page to the specified block
   * @param block the block to write to
   * @param p the page to write from
   */
  void Write(const BlockId& block, const Page& p);

  /**
   * @brief Extend a file with a block of zeroed bytes
   * @param filename the file to extend
   * @return a newly appended block
   */
  BlockId Append(std::string_view filename);

  /**
   * @brief Calculate how many blocks does a file contain
   * @param filename the filename to get its number of blocks
   * @return the number of blocks in the file
   */
  int Length(std::string_view filename);

  /**
   * @brief Check whether this FileManager object holds a newly created database
   * @return true or false
   */
  bool IsNew() const noexcept { return is_new_; }

  /**
   * @brief Get the size of a disk block
   * @return size of a disk block
   */
  int BlockSize() const noexcept { return block_size_; }

 private:
  /**
   * @brief Get the file with the specified filename
   * @param filename name of the file to get
   * @return a file stream representing the specified file
   */
  std::fstream& GetFile(std::string_view filename);

  fs::path db_directory_path_;
  int block_size_{};
  bool is_new_{};
  std::unordered_map<std::string, std::fstream, string_hash, std::equal_to<>>
      open_files_;    // a table of open files in the database
  std::mutex mutex_;  // mutex to make FileManager thread-safe
};
}  // namespace simpledb
