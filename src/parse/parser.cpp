#include "parse/parser.h"

#include <utility>

namespace simpledb {
std::string Parser::ParseField() { return lexer_.EatID(); }

Constant Parser::ParseConstant() {
  if (lexer_.MatchStringConstant()) {
    return Constant{lexer_.EatStringConstant()};
  }
  return Constant{lexer_.EatIntConstant()};
}

Expression Parser::ParseExpression() {
  if (lexer_.MatchID()) {
    return Expression{ParseField()};
  }
  return Expression{ParseConstant()};
}

Term Parser::ParseTerm() {
  Expression lhs{ParseExpression()};
  lexer_.EatDelim('=');
  Expression rhs{ParseExpression()};
  return Term{lhs, rhs};
}

Predicate Parser::ParsePredicate() {
  Predicate predicate{ParseTerm()};
  if (lexer_.MatchKeyword("and")) {
    lexer_.EatKeyword("and");
    predicate.ConjoinWith(ParsePredicate());
  }
  return predicate;
}

QueryData Parser::ParseQuery() {
  lexer_.EatKeyword("select");
  auto fields = ParseSelectList();
  lexer_.EatKeyword("from");
  auto tables = ParseTableList();
  Predicate predicate;
  if (lexer_.MatchKeyword("where")) {
    lexer_.EatKeyword("where");
    predicate = ParsePredicate();
  }
  return QueryData{std::move(fields), std::move(tables), std::move(predicate)};
}

std::vector<std::string> Parser::ParseSelectList() {
  std::vector<std::string> list;
  list.emplace_back(ParseField());
  if (lexer_.MatchDelim(',')) {
    lexer_.EatDelim(',');
    auto other = ParseSelectList();
    list.insert(list.end(), other.begin(), other.end());
  }
  return list;
}

std::vector<std::string> Parser::ParseTableList() {
  std::vector<std::string> list;
  list.emplace_back(lexer_.EatID());
  if (lexer_.MatchDelim(',')) {
    lexer_.EatDelim(',');
    auto other = ParseTableList();
    list.insert(list.end(), other.begin(), other.end());
  }
  return list;
}

Parser::GenericData Parser::ParseUpdateCommand() {
  if (lexer_.MatchKeyword("insert")) {
    return ParseInsert();
  } else if (lexer_.MatchKeyword("delete")) {
    return ParseDelete();
  } else if (lexer_.MatchKeyword("update")) {
    return ParseModify();
  }
  return std::visit([](auto&& arg) -> GenericData { return arg; }, ParseCreate());
}

Parser::GenericCreateData Parser::ParseCreate() {
  lexer_.EatKeyword("create");
  if (lexer_.MatchKeyword("table")) {
    return ParseCreateTable();
  } else if (lexer_.MatchKeyword("view")) {
    return ParseCreateView();
  }
  return ParseCreateIndex();
}

DeleteData Parser::ParseDelete() {
  lexer_.EatKeyword("delete");
  lexer_.EatKeyword("from");
  auto table_name = lexer_.EatID();
  Predicate predicate;
  if (lexer_.MatchKeyword("where")) {
    lexer_.EatKeyword("where");
    predicate = ParsePredicate();
  }
  return DeleteData{table_name, predicate};
}

InsertData Parser::ParseInsert() {
  lexer_.EatKeyword("insert");
  lexer_.EatKeyword("into");
  auto table_name = lexer_.EatID();
  lexer_.EatDelim('(');
  auto fields = ParseFieldList();
  lexer_.EatDelim(')');
  lexer_.EatKeyword("values");
  lexer_.EatDelim('(');
  auto values = ParseConstList();
  lexer_.EatDelim(')');
  return InsertData{table_name, fields, values};
}

std::vector<std::string> Parser::ParseFieldList() {
  std::vector<std::string> list;
  list.emplace_back(ParseField());
  if (lexer_.MatchDelim(',')) {
    lexer_.EatDelim(',');
    auto other = ParseFieldList();
    list.insert(list.end(), other.begin(), other.end());
  }
  return list;
}

std::vector<Constant> Parser::ParseConstList() {
  std::vector<Constant> list;
  list.emplace_back(ParseConstant());
  if (lexer_.MatchDelim(',')) {
    lexer_.EatDelim(',');
    auto other = ParseConstList();
    list.insert(list.end(), other.begin(), other.end());
  }
  return list;
}

ModifyData Parser::ParseModify() {
  lexer_.EatKeyword("update");
  auto table_name = lexer_.EatID();
  lexer_.EatKeyword("set");
  auto field_name = ParseField();
  lexer_.EatDelim('=');
  auto new_val = ParseExpression();
  Predicate predicate;
  if (lexer_.MatchKeyword("where")) {
    lexer_.EatKeyword("where");
    predicate = ParsePredicate();
  }
  return ModifyData{table_name, field_name, new_val, predicate};
}

CreateTableData Parser::ParseCreateTable() {
  lexer_.EatKeyword("table");
  auto table_name = lexer_.EatID();
  lexer_.EatDelim('(');
  Schema schema = ParseFieldDefs();
  lexer_.EatDelim(')');

  return CreateTableData{table_name, schema};
}

Schema Parser::ParseFieldDefs() {
  Schema schema = ParseFieldDef();
  if (lexer_.MatchDelim(',')) {
    lexer_.EatDelim(',');
    Schema schema2 = ParseFieldDefs();
    schema.AddAll(schema2);
  }
  return schema;
}

Schema Parser::ParseFieldDef() {
  auto field_name = ParseField();
  return ParseFieldType(field_name);
}

Schema Parser::ParseFieldType(std::string_view field_name) {
  Schema schema;
  if (lexer_.MatchKeyword("int")) {
    lexer_.EatKeyword("int");
    schema.AddIntField(field_name);
  } else {
    lexer_.EatKeyword("varchar");
    lexer_.EatDelim('(');
    int length = lexer_.EatIntConstant();
    lexer_.EatDelim(')');
    schema.AddStringField(field_name, length);
  }
  return schema;
}

CreateViewData Parser::ParseCreateView() {
  lexer_.EatKeyword("view");
  auto view_name = lexer_.EatID();
  lexer_.EatKeyword("as");
  QueryData view_def = ParseQuery();

  return CreateViewData{view_name, view_def};
}

CreateIndexData Parser::ParseCreateIndex() {
  lexer_.EatKeyword("index");
  auto index_name = lexer_.EatID();
  lexer_.EatKeyword("on");
  auto table_name = lexer_.EatID();
  lexer_.EatDelim('(');
  auto field_name = ParseField();
  lexer_.EatDelim(')');

  return CreateIndexData{index_name, table_name, field_name};
}
}  // namespace simpledb
