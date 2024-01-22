#include "file/page.h"

#include <cstring>

namespace simpledb {

int Page::GetInt(int offset) const noexcept {
  int num;
  std::memcpy(&num, &byte_buffer_[offset], sizeof(int));
  return num;
}

void Page::SetInt(int offset, int num) noexcept {
  std::memcpy(&byte_buffer_[offset], &num, sizeof(int));
}

std::span<char> Page::GetBytes(int offset) const noexcept {
  // TODO(DANG): reference to the underlying bytes (span)
  // or allocate a new chunk of bytes (unique_ptr)
  char* ptr = &byte_buffer_[offset];
  int length;
  std::memcpy(&length, ptr, sizeof(int));
  ptr += sizeof(int);

  return std::span{ptr, static_cast<size_t>(length)};
}

void Page::SetBytes(int offset, std::span<char> bytes) noexcept {
  char* ptr = &byte_buffer_[offset];
  int length = bytes.size();
  std::memcpy(ptr, &length, sizeof(int));
  ptr += sizeof(int);
  std::memcpy(ptr, bytes.data(), length);
}

std::string_view Page::GetString(int offset) const noexcept {
  std::span<char> bytes = GetBytes(offset);
  return std::string_view{bytes.data(), bytes.size()};
}

void Page::SetString(int offset, std::string_view s) noexcept {
  SetBytes(offset, std::span{const_cast<char*>(s.data()), s.size()});
}

std::span<char> Page::Contents() const noexcept {
  return std::span{byte_buffer_.get(), static_cast<size_t>(size_)};
}

}  // namespace simpledb
