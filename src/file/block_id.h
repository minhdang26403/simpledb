#pragma once

#include <string>
#include <string_view>

namespace simpledb {
class BlockId {
 public:
  BlockId(std::string_view filename, int block_num)
      : filename_(filename), block_num_(block_num) {}

  // TODO(DANG): string or string_view
  std::string_view Filename() const noexcept { return filename_; }

  int BlockNumber() const noexcept { return block_num_; }

  bool Equals(const BlockId& other) const noexcept;

  std::string ToString() const;

  bool operator==(const BlockId& other) const noexcept;

  bool operator!=(const BlockId& other) const noexcept;

 private:
  std::string filename_;  // TODO(DANG): string copy or reference
  int block_num_{};
};
}  // namespace simpledb
