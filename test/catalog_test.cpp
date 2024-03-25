#include <iostream>

#include "metadata/table_manager.h"
#include "record/table_scan.h"
#include "server/simpledb.h"

namespace simpledb {
void CatalogTest() {
  SimpleDB db{"table_test", 400, 8};
  auto txn = db.NewTxn();
  TableManager table_manager{true, txn};
  auto table_catalog_layout = table_manager.GetLayout("table_catalog", txn);

  std::cout << "Here are all the tables and their lengths.\n";
  TableScan table_catalog{txn, "table_catalog", table_catalog_layout};
  while (table_catalog.Next()) {
    auto table_name = table_catalog.GetString("table_name");
    int slot_size = table_catalog.GetInt("slot_size");
    std::cout << table_name << ' ' << slot_size << '\n';
  }
  table_catalog.Close();

  std::cout << "\nHere are the fields for each table and their offsets\n";
  auto field_catalog_layout = table_manager.GetLayout("field_catalog", txn);
  TableScan field_catalog{txn, "field_catalog", field_catalog_layout};
  while (field_catalog.Next()) {
    auto table_name = field_catalog.GetString("table_name");
    auto field_name = field_catalog.GetString("field_name");
    int offset = field_catalog.GetInt("offset");
    std::cout << table_name << ' ' << field_name << ' ' << offset << '\n';
  }
  field_catalog.Close();
}
}  // namespace simpledb

int main() {
  simpledb::CatalogTest();

  return 0;
}
