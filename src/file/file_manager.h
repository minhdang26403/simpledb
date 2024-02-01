#pragma once

#include <filesystem>
#include <fstream>
#include <mutex>  // NOLINT(build/c++11)
#include <string>
#include <string_view>

#include "file/block_id.h"
#include "file/page.h"
#include "utils/data_type.h"

namespace simpledb {
namespace fs = std::filesystem;

/**
 * The File Manager handles the actual interaction with the OS file system. It
 * manages OS files as a virtual disk
 */
class FileManager {
 public:
  /**
   * @brief Construct a new File Manager object.
   * @param db_directory path to the directory that store files of the database
   * @param block_size size of a disk block
   */
  FileManager(const fs::path& db_directory, int block_size);

  /**
   * @brief Read the contents of the specified block into the specified
   * page.
   * @param block the block to read from
   * @param page the page to read to
   */
  void Read(const BlockId& block, const Page& page);

  /**
   * @brief Write the contents of a page to the specified block
   * @param block the block to write to
   * @param page the page to write from
   */
  void Write(const BlockId& block, const Page& page);

  /**
   * @brief Extend a file with a block of zeroed bytes
   * @param filename the file to extend
   * @return a newly appended block
   */
  BlockId Append(std::string_view filename);

  /**
   * @brief Get the number of blocks of a file
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
  HashMap<std::string, std::fstream>
      open_files_;    // a table of open files in the database
  std::mutex mutex_;  // mutex to make FileManager thread-safe
};
}  // namespace simpledb
