#pragma once

#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace simpledb {
// helper constant for the visitor
template <class>
inline constexpr bool always_false_v = false;

/**
 * The class that denotes values stored in the database
 */
class Constant {
 public:
  /**
   * @brief Construct an integer constant
   * @param ival value of the integer
   */
  explicit Constant(int ival) : val_(ival) {}

  /**
   * @brief Construct a string constant
   * @param sval value of the string
   */
  template <typename Str>
    requires std::is_constructible_v<std::string, std::decay_t<Str>>
  explicit Constant(Str&& sval)
      : val_(std::in_place_index<1>, std::forward<Str>(sval)) {}

  /**
   * @brief Evaluate the constant as an integer
   * @return the integer value of the constant
   */
  int AsInt() const { return std::get<0>(val_); }

  /**
   * @brief Evaluate the constant as a string
   * @return the string value of the constant
   */
  std::string_view AsString() const { return std::get<1>(val_); }

  /**
   * @brief Return whether two constants are equal. Two constants are equal if
   * they have the same type and the same value
   * @param other the other constant to compare with
   * @return true if the two constants are equal; otherwise, false
   */
  bool operator==(const Constant& other) const noexcept {
    return val_ == other.val_;
  }

  /**
   * @brief Return the string representation of this constant
   * @return the string representation
   */
  std::string ToString() const {
    return std::visit(
        [this](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, int>) {
            return std::to_string(AsInt());
          } else if constexpr (std::is_same_v<T, std::string>) {
            return std::string{AsString()};
          } else {
            static_assert(always_false_v<T>, "non-exhaustive visitor!");
          }
        },
        val_);
  }

 private:
  std::variant<int, std::string> val_;
};
}  // namespace simpledb
