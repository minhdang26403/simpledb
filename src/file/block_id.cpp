#include "file/block_id.h"
#include <sstream>

namespace simpledb {
bool BlockId::Equals(const BlockId& other) const noexcept {
  return *this == other;
}

std::string BlockId::ToString() const {
  std::stringstream output;
  output << "[file " << Filename() << ", block " << BlockNumber() << ']';

  return output.str();
}

bool BlockId::operator==(const BlockId& other) const noexcept {
  return Filename() == other.Filename() && BlockNumber() == other.BlockNumber();
}

bool BlockId::operator!=(const BlockId& other) const noexcept {
  return !(*this == other);
}
}  // namespace simpledb
