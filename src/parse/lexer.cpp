#include "parse/lexer.h"

#include <string>

namespace simpledb {
bool Lexer::MatchDelim(char d) const noexcept {
  return d == static_cast<char>(tokenizer_.ttype);
}

bool Lexer::MatchIntConstant() const noexcept {
  return tokenizer_.ttype == StreamTokenizer::TT_NUMBER;
}

bool Lexer::MatchStringConstant() const noexcept {
  return '\'' == static_cast<char>(tokenizer_.ttype);
}

bool Lexer::MatchKeyword(std::string_view w) const noexcept {
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

void Lexer::EatKeyword(std::string_view w) {
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
