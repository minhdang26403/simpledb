#pragma once

#include <span>  // NOLINT(build/include_order)
#include <string_view>

namespace simpledb {
/**
 * A Page object holds the contents of a disk block.
 */
class Page {
 public:
  /**
   * @brief Construct a new Page object by allocating new heap memory. This
   * constructor is used by the buffer manager.
   * @param block_size size of a disk block
   */
  explicit Page(int block_size)
      : byte_buffer_(new char[block_size]()),
        own_memory_(true),
        size_(block_size) {}

  /**
   * @brief Construct a new Page object that gets its memory from another
   * buffer. This constructor is used primarily by the log manager.
   * @param buffer pointer to the buffer
   * @param size size of a disk block
   */
  Page(char* buffer, size_t size) : byte_buffer_(buffer), size_(size) {}

  /**
   * @brief Destructor. Only deallocate memory if this page owns memory
   */
  ~Page();

  /**
   * @brief Get an integer from the page at the specified offset
   * @param offset the offset to retrieve value from
   * @return the integer at the specified offset
   */
  int GetInt(int offset) const noexcept;

  /**
   * @brief Save an integer in the page at the specified offset. Clients of the
   * `Page` are responsible for knowing what values have been stored where.
   * @param offset the offset to save value at
   * @param num the integer to save
   */
  void SetInt(int offset, int num) noexcept;

  /**
   * @brief Get a blob (a chunk of bytes) from the page at the specified offset
   * @param offset the offset to retrieve the blob from
   * @return a non-owning view (but modifiable) of the blob
   */
  std::span<char> GetBytes(int offset) const noexcept;

  /**
   * @brief Save a blob in the page at the specified offset. Clients of the
   * `Page` are responsible for knowing what values have been stored where.
   * @param offset the offset to save value at
   * @param bytes the blob to save
   */
  void SetBytes(int offset, std::span<char> bytes) noexcept;

  /**
   * @brief Get a string from the page at the specified offset
   * @param offset the offset to retrieve the string from
   * @return a constant non-owning view of the string
   */
  std::string_view GetString(int offset) const noexcept;

  /**
   * @brief Save a string in the page at the specified offset. Clients of the
   * `Page` are responsible for knowing what values have been stored where.
   * @param offset the offset to save value at
   * @param s the string to save
   */
  void SetString(int offset, std::string_view s) noexcept;

  /**
   * @brief Get the number of bytes that a string actually occupies in a page
   * @param str_len length of the string
   * @return the actual space the string occupies
   */
  static int StringLength(std::string_view s) noexcept {
    return sizeof(int) + s.size() * sizeof(CharSet);
  }

  /**
   * @brief Retrieve the content of the entire page
   * @return a non-owning view (but modifiable) of the page
   */
  std::span<char> Contents() const noexcept;

 private:
  // Use ASCII as the character encoding scheme for this in-memory page
  using CharSet = char;

  char* byte_buffer_{};
  bool own_memory_{};
  size_t size_{};
};
}  // namespace simpledb
