#pragma once

#include <functional>
#include <string>

namespace simpledb {
/**
 * Our own version of hash function object to support heterogeneous key lookup
 * for `std::unordered_map`
 */
struct string_hash {
  // need to define the `is_transparent` type to support heterogeneous key
  // lookup
  using is_transparent = void;

  /**
   * @brief Hash a string literal
   * @param s the string literal to hash
   * @return the hash value of this string
   */
  [[nodiscard]] size_t operator()(const char* s) const {
    return std::hash<std::string_view>{}(s);
  }

  /**
   * @brief Hash a string view
   * @param s the string view to hash
   * @return the hash value of this stirng
   */
  [[nodiscard]] size_t operator()(std::string_view s) const {
    return std::hash<std::string_view>{}(s);
  }

  /**
   * @brief Hash a string object
   * @param s the string object to hash
   * @return the hash value of this string
   */
  [[nodiscard]] size_t operator()(const std::string& s) const {
    return std::hash<std::string>{}(s);
  }
};
}  // namespace simpledb
