#include "file/file_manager.h"

#include <memory>
#include <string>
#include <utility>

namespace simpledb {
FileManager::FileManager(const fs::path& db_directory_path, int block_size)
    : db_directory_path_(db_directory_path), block_size_(block_size) {
  is_new_ = !fs::directory_entry{db_directory_path_}.exists();

  // Create the directory to store the database if it is new
  if (is_new_) {
    fs::create_directories(db_directory_path_);
  }

  // Remove any leftover temporary tables
  for (const auto& directory_entry :
       fs::directory_iterator{db_directory_path_}) {
    const auto& entry_path = directory_entry.path();
    if (entry_path.filename().string().starts_with("temp")) {
      fs::remove(entry_path);
    }
  }
}

void FileManager::Read(const BlockId& block, const Page& page) {
  std::scoped_lock lock{mutex_};
  std::fstream& file = GetFile(block.Filename());
  file.seekg(block.BlockNumber() * block_size_, std::ios_base::beg);
  auto page_view = page.Contents();
  // Read data from file into the page
  file.read(page_view.data(), page_view.size());
  // When reading an empty file, end of file condition occurs
  if (file.fail()) {
    if (file.rdstate() == (std::ios_base::failbit | std::ios_base::eofbit)) {
      // Must clear all error flags since (failbit | eofbit) is set when reading
      // from an empty file
      file.clear();
      // Initialize the page with zero to indicate that it is read from an
      // empty file
      std::memset(page_view.data(), '\0', page_view.size());
    } else {
      throw std::runtime_error("Got error while reading file");
    }
  }
}

void FileManager::Write(const BlockId& block, const Page& page) {
  std::scoped_lock lock{mutex_};
  std::fstream& file = GetFile(block.Filename());
  file.seekp(block.BlockNumber() * block_size_, std::ios_base::beg);
  if (file.tellp() == -1) {
    throw std::runtime_error("A failure occurred to the output file");
  }
  auto page_view = page.Contents();
  // Write data from page to the file
  file.write(page_view.data(), page_view.size());
  if (file.bad()) {
    throw std::runtime_error("Got non-recoverable error while writing to file");
  }
  file.flush();
}

BlockId FileManager::Append(std::string_view filename) {
  std::scoped_lock lock{mutex_};
  int new_block_num = Length(filename);
  BlockId block{filename, new_block_num};
  std::fstream& file = GetFile(block.Filename());
  file.seekp(block.BlockNumber() * block_size_, std::ios_base::beg);
  if (file.tellp() == -1) {
    throw std::runtime_error("A failure occurred to the output file");
  }
  auto bytes = std::make_unique<char[]>(block_size_);
  // Write a block of zeroed bytes to the end of the file
  file.write(bytes.get(), block_size_);
  if (file.bad()) {
    throw std::runtime_error("Got non-recoverable error while writing to file");
  }
  file.flush();

  return block;
}

int FileManager::Length(std::string_view filename) {
  GetFile(filename);
  fs::path filepath{db_directory_path_ / filename};

  return fs::file_size(filepath) / block_size_;
}

std::fstream& FileManager::GetFile(std::string_view filename) {
  auto entry = open_files_.find(filename);
  if (entry != open_files_.end()) {
    return entry->second;
  }

  fs::path db_table{db_directory_path_ / filename};
  // Workaround for std::fstream C++ library
  // If a file doesn't exist, the std::fstream can't create
  // a new file as both input/output binary stream
  std::fstream file(
      db_table, std::ios_base::binary | std::ios_base::in | std::ios_base::out);

  if (!file.is_open()) {
    // Clear all the error flags
    file.clear();
    // Re-open the file in the following mode to create the file
    file.open(db_table, std::ios_base::binary | std::ios_base::in |
                            std::ios_base::out | std::ios_base::trunc);
    // Close and re-open the file with the original mode
    file.close();
    file.open(db_table, std::ios::binary | std::ios::in | std::ios::out);

    if (!file.is_open()) {
      throw std::runtime_error("Error opening file");
    }
  }

  // std::fstream is a non-copyable type, must move this local file object
  // for in-place construction
  open_files_.emplace(filename, std::move(file));
  entry = open_files_.find(filename);

  return entry->second;
}
}  // namespace simpledb
