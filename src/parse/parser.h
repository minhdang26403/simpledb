#pragma once

#include <string>
#include <vector>

#include "parse/create_index_data.h"
#include "parse/create_table_data.h"
#include "parse/create_view_data.h"
#include "parse/delete_data.h"
#include "parse/insert_data.h"
#include "parse/lexer.h"
#include "parse/modify_data.h"
#include "parse/query_data.h"
#include "query/constant.h"
#include "query/expression.h"
#include "query/predicate.h"
#include "query/term.h"

namespace simpledb {
/**
 * The SimpleDB parser
 */
class Parser {
  using GenericData =
      std::variant<InsertData, DeleteData, ModifyData, CreateTableData,
                   CreateViewData, CreateIndexData>;

  using GenericCreateData =
      std::variant<CreateTableData, CreateViewData, CreateIndexData>;

 public:
  /**
   * @brief Construct a new Parser object
   * @param statement the statement to parse
   * TODO: Fix parameter type of `s`?
   */
  explicit Parser(const std::string& statement) : lexer_(statement) {}

  /**
   * @brief Extract an identifier token and return the name of that identifier.
   * Throw exception if the next token is not an identifier token.
   * @return the field name
   */
  std::string ParseField();

  /**
   * @brief Extract a constant token. Throw exception if the next token is not a
   * constant
   * @return a constant object
   */
  Constant ParseConstant();

  /**
   * @brief Extract an expression token. The expression can be an identifier
   * token or a constant token. Throw exception if the next token is not an
   * expression
   * @return an expression object
   */
  Expression ParseExpression();

  /**
   * @brief Extract term tokens. A term consists of an expression token, an
   * equal-sign token, and another expression token. Throw exception if these
   * tokens do not correspond to a parse tree for term
   * @return a term object
   */
  Term ParseTerm();

  /**
   * @brief Extract predicate tokens. This method eats a term token in the
   * predicate and recursively eats a predicate token if there is a keyword AND.
   * Throw exception if the lexer can't find the right token when parsing the
   * predicate
   * @return a predicate object
   */
  Predicate ParsePredicate();

  /**
   * @brief Parse a query to acquire field names, table names, and predicate for
   * query planner
   * @return a query data object containing field names, table names, and
   * predicate
   */
  QueryData ParseQuery();

  /**
   * @brief Parse various update commands. The method uses the initial token of
   * the string to identify the type of update.
   * @return a variant containing a data object corresponding to the specific
   * update command
   */
  GenericData ParseUpdateCommand();

  /**
   * @brief Parse a `delete` command
   * @return a delete data object containing the table name and the predicate
   * describing which records to delete
   */
  DeleteData ParseDelete();

  /**
   * @brief Parse an `insert` command
   * @return an insert data object containing the table name, the field list,
   * and the value list
   */
  InsertData ParseInsert();

  /**
   * @brief Parse an `update` command
   * @return a modify data object containing the table name, the assignment
   * information, and the predicate describing which records to update
   */
  ModifyData ParseModify();

  /**
   * @brief Parse a `create table` command
   * @return a create table data object containing the table name and the table
   * schema
   */
  CreateTableData ParseCreateTable();

  /**
   * @brief Parse a `create view` command
   * @return a create view data object containing the view name and the view
   * definition
   */
  CreateViewData ParseCreateView();

  /**
   * @brief Parse a `create index` command
   * @return a create index data object containing the index name, the indexed
   * table name, and the indexed field
   */
  CreateIndexData ParseCreateIndex();

 private:
  /**
   * @brief Parse the `select` clause of the query
   * @return a list of field names
   */
  std::vector<std::string> ParseSelectList();

  /**
   * @brief Parse the `from` clause of the query
   * @return a list of table names
   */
  std::vector<std::string> ParseTableList();

  /**
   * @brief Parse the SQL creation statement
   * @return a variant containing a creation data object corresponding to the
   * specific type of `create` statement
   */
  GenericCreateData ParseCreate();

  /**
   * @brief Parse a list of fields to insert
   * @return the field list
   */
  std::vector<std::string> ParseFieldList();

  /**
   * @brief Parse a list of values corresponding to the specified field list
   * @return the value list
   */
  std::vector<Constant> ParseConstList();

  /**
   * @brief Parse a list of field definitions
   * @return a schema containing all fields in the list
   */
  Schema ParseFieldDefs();

  /**
   * @brief Parse a field definition
   * @return a schema containing one field
   */
  Schema ParseFieldDef();

  /**
   * @brief Extract the type of the specified field
   * @param field_name the field name
   * @return a schema containing the specified field
   */
  Schema ParseFieldType(std::string_view field_name);

  Lexer lexer_;
};
}  // namespace simpledb
