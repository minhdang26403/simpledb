#include "file/block_id.h"

namespace simpledb {
bool BlockId::Equals(const BlockId& other) const noexcept {
  return *this == other;
}

std::string BlockId::ToString() const {
  std::string output = "[file ";
  output.append(Filename());
  output.append(", block ");
  output.append(std::to_string(BlockNumber()));
  output.append("]");

  return output;
}

bool BlockId::operator==(const BlockId& other) const noexcept {
  return Filename() == other.Filename() && BlockNumber() == other.BlockNumber();
}

bool BlockId::operator!=(const BlockId& other) const noexcept {
  return !(*this == other);
}

}  // namespace simpledb
