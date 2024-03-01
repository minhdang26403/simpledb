#include "parse/parser.h"

#include <iostream>
#include <string>

namespace simpledb {
void ParserTest() {
  std::cout << "Enter an SQL statement: ";
  std::string s;
  while (std::getline(std::cin, s)) {
    Parser parser{s};
    try {
      if (s.starts_with("select")) {
        parser.ParseQuery();
      } else {
        parser.ParseUpdateCommand();
      }
      std::cout << "Your statement is legal\n";
    } catch (const BadSyntaxException& ex) {
      std::cout << "Your statement is illegal\n";
    }
    std::cout << "Enter an SQL statement: ";
  }
}
}  // namespace simpledb

int main() {
  simpledb::ParserTest();

  return 0;
}
