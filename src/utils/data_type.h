#pragma once

#include <functional>

#include "unordered_map"
#include "utils/hash_util.h"

namespace simpledb {
enum Type { INTEGER, VARCHAR };

template <typename Key, typename Value>
using HashMap = std::unordered_map<Key, Value, string_hash, std::equal_to<>>;

}  // namespace simpledb
