#include "parse/lexer.h"

#include <string>

namespace simpledb {
Lexer::Lexer(const std::string& s)
    : keywords_({"select", "from", "where", "and", "insert", "into", "values",
                 "delete", "update", "set", "create", "table", "int", "varchar",
                 "view", "as", "index", "on"}),
      tokenizer_(s) {
  tokenizer_.OrdinaryChar('.');    // disallow "." in identifiers
  tokenizer_.WordChars('_', '_');  // allow "_" in identifiers
  tokenizer_.LowerCaseMode(true);  // ids and keywords are converted
  NextToken();
}

bool Lexer::MatchDelim(char d) const noexcept {
  return d == static_cast<char>(tokenizer_.ttype);
}

bool Lexer::MatchIntConstant() const noexcept {
  return tokenizer_.ttype == StreamTokenizer::TT_NUMBER;
}

bool Lexer::MatchStringConstant() const noexcept {
  return '\'' == static_cast<char>(tokenizer_.ttype);
}

bool Lexer::MatchKeyword(const std::string& w) const noexcept {
  return tokenizer_.ttype == StreamTokenizer::TT_WORD && tokenizer_.sval == w;
}

bool Lexer::MatchID() const noexcept {
  return tokenizer_.ttype == StreamTokenizer::TT_WORD &&
         !keywords_.contains(tokenizer_.sval);
}

void Lexer::EatDelim(char d) {
  if (!MatchDelim(d)) {
    throw BadSyntaxException();
  }
  NextToken();
}

int Lexer::EatIntConstant() {
  if (!MatchIntConstant()) {
    throw BadSyntaxException();
  }
  int i = tokenizer_.nval;
  NextToken();
  return i;
}

std::string Lexer::EatStringConstant() {
  if (!MatchStringConstant()) {
    throw BadSyntaxException();
  }
  std::string s = tokenizer_.sval;  // constants are not converted to lower case
  NextToken();
  return s;
}

void Lexer::EatKeyword(const std::string& w) {
  if (!MatchKeyword(w)) {
    throw BadSyntaxException();
  }
  NextToken();
}

std::string Lexer::EatID() {
  if (!MatchID()) {
    throw BadSyntaxException();
  }
  std::string s = tokenizer_.sval;
  NextToken();
  return s;
}

void Lexer::NextToken() {
  try {
    tokenizer_.NextToken();
  } catch (const std::ios_base::failure& e) {
    throw BadSyntaxException();
  }
}
}  // namespace simpledb
