#include "record/table_scan.h"

#include <iostream>

#include "record/layout.h"
#include "record/schema.h"
#include "server/simpledb.h"
#include "txn/transaction.h"

namespace simpledb {
void TableScanTest() {
  SimpleDB db{"tabletest", 400, 8};
  Transaction txn{db.GetFileManager(), db.GetLogManager(),
                  db.GetBufferManager()};

  Schema schema;
  schema.AddIntField("A");
  schema.AddStringField("B", 9);
  Layout layout{schema};

  for (const auto& field_name : layout.GetSchema().Fields()) {
    int offset = layout.GetOffset(field_name);
    std::cout << field_name << " has offset " << offset << '\n';
  }

  std::cout << "Filling the table with 50 random records.\n";
  TableScan ts{txn, "T", layout};
  for (int i = 0; i < 50; i++) {
    ts.Insert();
    int n = rand() % 50;  // NOLINT(runtime/threadsafe_fn)
    ts.SetInt("A", n);
    ts.SetString("B", "rec" + std::to_string(n));
    std::cout << "inserting into slot " << ts.GetRID().ToString() << ": {" << n
              << ", "
              << "rec" << n << "}\n";
  }

  std::cout << "Deleting these records, whose A-values are less than 25.\n";
  int count = 0;
  ts.BeforeFirst();
  while (ts.Next()) {
    int a = ts.GetInt("A");
    std::string_view b = ts.GetString("B");
    if (a < 25) {
      count++;
      std::cout << "slot " << ts.GetRID().ToString() << ": {" << a << ", " << b
                << "}\n";
      ts.Delete();
    }
  }
  std::cout << count << " values under 25 were deleted.\n\n";

  std::cout << "Here are the remaining records.\n";
  ts.BeforeFirst();
  while (ts.Next()) {
    int a = ts.GetInt("A");
    std::string_view b = ts.GetString("B");
    std::cout << "slot " << ts.GetRID().ToString() << ": {" << a << ", " << b
              << "}\n";
  }
  ts.Close();
  txn.Commit();
}
}  // namespace simpledb

int main() {
  simpledb::TableScanTest();

  return 0;
}
