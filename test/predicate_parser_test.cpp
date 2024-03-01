#include "parse/predicate_parser.h"

#include <iostream>

#include "server/simpledb.h"

namespace simpledb {
void PredicateParserTest() {
  std::cout << "Enter an SQL predicate: \n";
  std::string s;
  while (std::getline(std::cin, s)) {
    PredicateParser parser {s};
    try {
      parser.ParsePredicate();
      std::cout << "yes\n";
    } catch (const BadSyntaxException& ex) {
      std::cout << "no\n";
    }
    std::cout << "Enter an SQL predicate: \n";
  }
}
}  // namespace simpledb

int main() {
  simpledb::PredicateParserTest();

  return 0;
}
