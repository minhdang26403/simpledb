#include <iostream>
#include <string>
#include <unordered_set>

#include "parse/stream_tokenizer.h"

namespace simpledb {
std::string GetStringFromUser() {
  std::cout << "Enter tokens:\n";
  std::string s;
  std::getline(std::cin, s);
  return s;
}

void PrintCurrentToken(StreamTokenizer& tok,
                       const std::unordered_set<std::string>& keywords) {
  if (tok.ttype == StreamTokenizer::TT_NUMBER) {
    std::cout << "IntConstant: " << tok.nval << '\n';
  } else if (tok.ttype == StreamTokenizer::TT_WORD) {
    std::string word = tok.sval;
    if (keywords.contains(word)) {
      std::cout << "Keyword: " << word << '\n';
    } else {
      std::cout << "Id: " << word << '\n';
    }
  } else if (tok.ttype == '\'') {
    std::cout << "StringConstant: " << tok.sval << '\n';
  } else {
    std::cout << "Delimiter: " << static_cast<char>(tok.ttype) << '\n';
  }
}

void TokenizerTest() {
  const std::unordered_set<std::string> keywords = {
      "select", "from",    "where",  "and", "insert", "into",
      "values", "delete",  "update", "set", "create", "table",
      "int",    "varchar", "view",   "as",  "index",  "on"};

  StreamTokenizer tok{GetStringFromUser()};
  tok.OrdinaryChar('.');
  tok.LowerCaseMode(true);  // ids and keywords are converted to lower case
  while (tok.NextToken() != StreamTokenizer::TT_EOF) {
    PrintCurrentToken(tok, keywords);
  }
}

}  // namespace simpledb

int main() {
  simpledb::TokenizerTest();

  return 0;
}
