#include "parse/lexer.h"

#include <iostream>
#include <string>

namespace simpledb {
/**
 * Will successfully read in lines of text denoting an SQL expression of the
 * form "id = c" or "c = id"
 */
void LexerTest() {
  std::string s;
  while (std::getline(std::cin, s)) {
    Lexer lexer{s};
    std::string x;
    int y;
    if (lexer.MatchID()) {
      x = lexer.EatID();
      lexer.EatDelim('=');
      y = lexer.EatIntConstant();
    } else {
      y = lexer.EatIntConstant();
      lexer.EatDelim('=');
      x = lexer.EatID();
    }
    std::cout << x << " equals " << y << '\n';
  }
}
}  // namespace simpledb

int main() {
  simpledb::LexerTest();

  return 0;
}
