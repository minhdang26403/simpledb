#pragma once

#include "record/layout.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The table manager. There are methods to create a table, save the metadata in
 * the catalog, and obtain the metadata of a previously-created table.
 */
class TableManager {
 public:
  /**
   * @brief Create a new catalog manager for the database system. If the
   * database is new, the two catalog tables are created.
   * @param is_new whether the database is new
   * @param txn the startup transaction
   */
  TableManager(bool is_new, Transaction& txn);

  /**
   * @brief Create a new table having the specified name and schema
   * @param table_name name of the new table
   * @param schema the table's schema
   * @param txn the transaction creating the table
   */
  void CreateTable(std::string_view table_name, const Schema& schema,
                   Transaction& txn);

  /**
   * @brief Retrieve the layout of the specified table from the catalog
   * @param table_name name of the table
   * @param txn the transaction retrieving the layout
   * @return layout of the specified table
   */
  Layout GetLayout(std::string_view table_name, Transaction& txn);

  static constexpr int MAX_NAME_LEN = 16;

 private:
  Layout table_catalog_layout_;
  Layout field_catalog_layout_;
};
}  // namespace simpledb
