#pragma once

#include <string>

#include "parse/lexer.h"

namespace simpledb {
/**
 * The predicate parser
 */
class PredicateParser {
 public:
  /**
   * @brief Construct a new Predicate Parser object
   * @param s the string to parse
   */
  explicit PredicateParser(const std::string& s) : lexer_(s) {}

  /**
   * @brief Extract an identifier token and return the name of that identifier.
   * Throw exception if the next token is not an identifier token.
   */
  void ParseField() { lexer_.EatID(); }

  /**
   * @brief Extract a constant token. Throw exception if the next token is not a
   * constant
   */
  void ParseConstant() {
    if (lexer_.MatchStringConstant()) {
      lexer_.EatStringConstant();
    } else {
      lexer_.EatIntConstant();
    }
  }

  /**
   * @brief Extract an expression token. The expression can be an identifier
   * token or a constant token. Throw exception if the next token is not an
   * expression
   */
  void ParseExpression() {
    if (lexer_.MatchID()) {
      ParseField();
    } else {
      ParseConstant();
    }
  }

  /**
   * @brief Extract term tokens. A term consists of an expression token, an
   * equal-sign token, and another expression token. Throw exception if these
   * tokens do not correspond to a parse tree for term
   */
  void ParseTerm() {
    ParseExpression();
    lexer_.EatDelim('=');
    ParseExpression();
  }

  /**
   * @brief Extract predicate tokens. It eats a term token in the predicate and
   * recursively eats a predicate token if there is a keyword AND. Throw
   * exception if the lexer can't find the right token when parsing the
   * predicate
   */
  void ParsePredicate() {
    ParseTerm();
    if (lexer_.MatchKeyword("and")) {
      lexer_.EatKeyword("and");
      ParsePredicate();
    }
  }

 private:
  Lexer lexer_;
};
}  // namespace simpledb
