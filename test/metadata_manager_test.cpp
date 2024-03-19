#include <iostream>

#include "record/table_scan.h"
#include "server/simpledb.h"

namespace simpledb {
void MetadataManagerTest() {
  SimpleDB db{"metadata_manager_test"};
  Transaction txn = db.NewTxn();
  auto& metadata_manager = db.GetMetadataManager();

  Schema schema;
  schema.AddIntField("A");
  schema.AddStringField("B", 9);

  // Part 1: Table Metadata
  metadata_manager.CreateTable("MyTable", schema, txn);
  Layout layout = metadata_manager.GetLayout("MyTable", txn);
  int size = layout.SlotSize();
  Schema& schema2 = layout.GetSchema();
  std::cout << "MyTable has slot size " << size << '\n';
  std::cout << "Its fields are:\n";
  for (const auto& field_name : schema2.Fields()) {
    std::string type;
    if (schema2.Type(field_name) == INTEGER) {
      type = "int";
    } else {
      int str_len = schema2.Length(field_name);
      type = "varchar(" + std::to_string(str_len) + ')';
    }
    std::cout << field_name << ": " << type << '\n';
  }

  // Part 2: Statistics Metadata
  TableScan ts{txn, "MyTable", layout};
  for (int i = 0; i < 50; i++) {
    ts.Insert();
    int n = arc4random() % 50;  // NOLINT(runtime/threadsafe_fn)
    ts.SetInt("A", n);
    ts.SetString("B", "rec" + std::to_string(n));
  }
  auto stat_info = metadata_manager.GetStatInfo("MyTable", layout, txn);
  std::cout << "B(MyTable) = " << stat_info.BlocksAccessed() << '\n';
  std::cout << "R(MyTable) = " << stat_info.RecordsOutput() << '\n';
  std::cout << "V(MyTable, A) = " << stat_info.DistinctValues("A") << '\n';
  std::cout << "V(MyTable, B) = " << stat_info.DistinctValues("B") << '\n';

  // Part 3: View Metadata
  auto view_def = "select B from MyTable where A = 1";
  metadata_manager.CreateView("viewA", view_def, txn);
  auto v = metadata_manager.GetViewDef("viewA", txn);
  std::cout << "View def = " << v.value_or("") << '\n';

  // Part 4: Index Metadata
  metadata_manager.CreateIndex("indexA", "MyTable", "A", txn);
  metadata_manager.CreateIndex("indexB", "MyTable", "B", txn);
  auto index_map = metadata_manager.GetIndexInfo("MyTable", txn);

  IndexInfo index_info_a = index_map.at("A");
  std::cout << "B(indexA) = " << index_info_a.BlocksAccessed() << '\n';
  std::cout << "R(indexA) = " << index_info_a.RecordsOutput() << '\n';
  std::cout << "V(indexA, A) = " << index_info_a.DistinctValues("A") << '\n';
  std::cout << "V(indexA, B) = " << index_info_a.DistinctValues("B") << '\n';

  IndexInfo index_info_b = index_map.at("B");
  std::cout << "B(indexA) = " << index_info_b.BlocksAccessed() << '\n';
  std::cout << "R(indexA) = " << index_info_b.RecordsOutput() << '\n';
  std::cout << "V(indexA, A) = " << index_info_b.DistinctValues("A") << '\n';
  std::cout << "V(indexA, B) = " << index_info_b.DistinctValues("B") << '\n';

  txn.Commit();
}
}  // namespace simpledb

int main() {
  simpledb::MetadataManagerTest();

  return 0;
}
