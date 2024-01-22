#pragma once

#include <string>
#include <string_view>

namespace simpledb {
/**
 * A BlockId object identifies a specific physical block by its file name and
 * logical block number
 */
class BlockId {
 public:
  /**
   * @brief Constructs a BlockId object
   * @param filename name of the file that this block refers to
   * @param block_num logical block number within the file
   */
  BlockId(std::string_view filename, int block_num)
      : filename_(filename), block_num_(block_num) {}

  // TODO(DANG): string or string_view
  /**
   * @brief Retrieves the file name to which this block belongs
   * @return a non-owning view of the file name
   */
  std::string_view Filename() const noexcept {
    // TODO(DANG): return string or string_view?
    return filename_;
  }

  /**
   * @brief Retrieves the logical block number of this block within the file
   * @return logical block number
   */
  int BlockNumber() const noexcept { return block_num_; }

  /**
   * @brief Compare whether two block objects refer to the same physical block
   * @param other the other block to compare
   * @return true if two blocks are the same; otherwise, false
   */
  bool Equals(const BlockId& other) const noexcept;

  /**
   * @brief Gets the string representation of a BlockId object for debugging
   * @return a string representation of this block
   */
  std::string ToString() const;

  /**
   * @brief Operator overloading for comparing two BlockId objects
   * @param other the other to compare
   * @return true if two blocks are the same; otherwise, false
   */
  bool operator==(const BlockId& other) const noexcept;

  /**
   * @brief Operator overloading for comparing two BlockId objects
   * @param other the other compare
   * @return true if two blocks are different; otherwise, false
   */
  bool operator!=(const BlockId& other) const noexcept;

 private:
  std::string filename_;  // TODO(DANG): string copy or reference
  int block_num_{};
};
}  // namespace simpledb
