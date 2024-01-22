#pragma once

#include <memory>
#include <span>
#include <string_view>
#include <utility>

namespace simpledb {
class Page {
 public:
  explicit Page(int block_size)
      : byte_buffer_(std::make_unique<char[]>(block_size)), size_(block_size) {}

  Page(std::unique_ptr<char[]> buffer, int size)
      : byte_buffer_(std::move(buffer)), size_(size) {}

  int GetInt(int offset) const noexcept;

  void SetInt(int offset, int num) noexcept;

  std::span<char> GetBytes(int offset) const noexcept;

  void SetBytes(int offset, std::span<char> bytes) noexcept;

  std::string_view GetString(int offset) const noexcept;

  void SetString(int offset, std::string_view s) noexcept;

  std::span<char> Contents() const noexcept;

 private:
  std::unique_ptr<char[]> byte_buffer_;
  int size_{};
};
}  // namespace simpledb
