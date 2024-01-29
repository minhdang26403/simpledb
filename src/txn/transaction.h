#pragma once

#include "file/block_id.h"
#include <string_view>
#include <string>

namespace simpledb {
class Transaction {
 public:
  void Pin(const BlockId& block);
  void Unpin(const BlockId& block);
  void SetInt(const BlockId& block, int offset, int val, bool OkToLog);
  void SetString(const BlockId& block, int offset, std::string_view string, bool OkToLog);
};
}  // namespace simpledb
