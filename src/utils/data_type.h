#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "utils/hash_util.h"

namespace simpledb {
enum Type { INTEGER, VARCHAR };

template <typename Value>
using StringHashMap =
    std::unordered_map<std::string, Value, string_hash, std::equal_to<>>;

using StringSet = std::unordered_set<std::string, string_hash, std::equal_to<>>;

}  // namespace simpledb
