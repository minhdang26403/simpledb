#include "metadata/table_manager.h"

#include <iostream>

#include "server/simpledb.h"

namespace simpledb {
void TableManagerTest() {
  SimpleDB db{"tblmgrtest", 400, 8};
  Transaction txn{db.GetFileManager(), db.GetLogManager(),
                  db.GetBufferManager()};
  TableManager table_manager{true, txn};

  Schema schema;
  schema.AddIntField("A");
  schema.AddStringField("B", 9);
  table_manager.CreateTable("MyTable", schema, txn);

  Layout layout = table_manager.GetLayout("MyTable", txn);
  int size = layout.SlotSize();
  Schema& schema2 = layout.GetSchema();
  std::cout << "MyTable has slot size " << size << '\n';
  std::cout << "Its fields are:";
  for (const auto& field_name : schema2.Fields()) {
    std::string type;
    if (schema2.Type(field_name) == INTEGER) {
      type = "int";
    } else {
      int str_len = schema2.Length(field_name);
      type = "varchar(" + std::to_string(str_len) + ")";
    }
    std::cout << field_name << ": " << type << '\n';
  }
  txn.Commit();
}
}  // namespace simpledb

int main() {
  simpledb::TableManagerTest();

  return 0;
}
