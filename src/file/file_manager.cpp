#include "file/file_manager.h"

#include <memory>
#include <tuple>

namespace simpledb {
FileManager::FileManager(const fs::path& db_directory_path, int block_size)
    : db_directory_path_(db_directory_path), block_size_(block_size) {
  is_new_ = !fs::directory_entry{db_directory_path_}.exists();

  if (is_new_) {
    fs::create_directories(db_directory_path_);
  }

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
  file.seekp(block.BlockNumber() * block_size_, std::ios_base::beg);
  std::span<char> page_view = page.Contents();
  file.read(page_view.data(), page_view.size());
}

void FileManager::Write(const BlockId& block, const Page& page) {
  std::scoped_lock lock{mutex_};
  std::fstream& file = GetFile(block.Filename());
  file.seekp(block.BlockNumber() * block_size_, std::ios_base::beg);
  std::span<char> page_view = page.Contents();
  file.write(page_view.data(), page_view.size());
  file.flush();
}

BlockId FileManager::Append(std::string_view filename) {
  std::scoped_lock lock{mutex_};
  int new_block_num = Length(filename);
  BlockId block{filename, new_block_num};
  std::fstream& file = GetFile(block.Filename());
  file.seekp(block.BlockNumber() * block_size_);
  auto bytes = std::make_unique<char[]>(block_size_);
  file.write(bytes.get(), block_size_);
  file.flush();
  return block;
}

int FileManager::Length(std::string_view filename) const {
  fs::path filepath{db_directory_path_ / filename};
  return fs::file_size(filepath) / block_size_;
}

std::fstream& FileManager::GetFile(std::string_view filename) {
  auto entry = open_files_.find(filename);
  if (entry == open_files_.end()) {
    fs::path db_table{db_directory_path_ / filename};
    // Workaround for std::fstream C++ library
    // If a file doesn't exist, the std::fstream can't create
    // a new file as both input/output binary stream
    // Create the file as output binary stream first
    open_files_.emplace(
        std::piecewise_construct, std::forward_as_tuple(filename),
        std::forward_as_tuple(db_table,
                              std::ios_base::out | std::ios_base::binary));
    entry = open_files_.find(filename);
    if (entry->second.fail() || entry->second.bad()) {
      throw std::runtime_error("Error opening file\n");
    }
    // Close the file to clear all flags
    entry->second.close();
    // Reopen the file as input/output binary stream
    entry->second.open(db_table, std::ios_base::in | std::ios_base::out |
                                     std::ios_base::binary);
  }
  return entry->second;
}

}  // namespace simpledb
