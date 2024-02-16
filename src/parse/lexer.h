#pragma once

#include <exception>
#include <string>

#include "parse/stream_tokenizer.h"
#include "utils/data_type.h"

namespace simpledb {
/**
 * A runtime exception indicating that the submitted query has incorrect syntax
 */
class BadSyntaxException : public std::exception {};

/**
 * The lexical analyzer
 */
class Lexer {
 public:
  /**
   * @brief Create a new lexical analyzer for the specified SQL statement
   * @param s the SQL statement
   */
  explicit Lexer(const std::string& s);

  // Methods to check the status of the current token

  /**
   * @brief Return true if the current token is the specified delimiter
   * character
   * @param d a character denoting the delimiter
   * @return true if the delimiter is the current token
   */
  bool MatchDelim(char d) const noexcept;

  /**
   * @brief Return true if the current token is an integer
   * @return true if the current token is an integer
   */
  bool MatchIntConstant() const noexcept;

  /**
   * @brief Return true if the current token is a string
   * @return true if the current token is a string
   */
  bool MatchStringConstant() const noexcept;

  /**
   * @brief Return true if the current token is the specified keyword
   * @param w the keyword string
   * @return true if that keyword is the current token
   */
  bool MatchKeyword(const std::string& w) const noexcept;

  /**
   * @brief Return true if the current token is a legal identifier
   * @return true if the current token is an identifier
   */
  bool MatchID() const noexcept;

  // Methods to "eat" the current token

  /**
   * @brief Throw an exception if the current token is not the specified
   * delimiter. Otherwise, move to the next token.
   * @param d a character denoting the delimiter
   */
  void EatDelim(char d);

  /**
   * @brief Throw an exception if the current token is not an integer.
   * Otherwise, return that integer and move to the next token.
   * @return the integer value of the current token
   */
  int EatIntConstant();

  /**
   * @brief Throw an exception if the current token is not a string. Otherwise,
   * return that string and move to the next token.
   * @return the string value of the current token
   */
  std::string EatStringConstant();

  /**
   * @brief Throw an exception if the current token is not the specified
   * keyword. Otherwise, move to the next token.
   * @param w the keyword string
   */
  void EatKeyword(const std::string& w);

  /**
   * @brief Throw an exception if the current token is not an identifier.
   * Otherwise, return the identifier string and move to the next token.
   * @return the string value of the current token
   */
  std::string EatID();

 private:
  /**
   * @brief Move to the next token or throw an exception if an I/O library
   * failure occurred
   */
  void NextToken();

  StringSet keywords_;
  StreamTokenizer tokenizer_;
};

}  // namespace simpledb
