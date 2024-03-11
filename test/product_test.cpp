#include <iostream>
#include <memory>

#include "query/product_scan.h"
#include "record/layout.h"
#include "record/schema.h"
#include "record/table_scan.h"
#include "server/simpledb.h"
#include "txn/transaction.h"

namespace simpledb {
void ProductTest() {
  SimpleDB db{"product_test"};
  Transaction txn = db.NewTxn();

  Schema schema1;
  schema1.AddIntField("A");
  schema1.AddStringField("B", 9);
  Layout layout1{schema1};
  TableScan ts1{txn, "T1", layout1};

  Schema schema2;
  schema2.AddIntField("C");
  schema2.AddStringField("D", 9);
  Layout layout2{schema2};
  TableScan ts2{txn, "T2", layout2};

  int n = 10;
  ts1.BeforeFirst();
  std::cout << "Inserting " << n << " records into T1.\n";
  for (int i = 0; i < n; i++) {
    ts1.Insert();
    ts1.SetInt("A", i);
    ts1.SetString("B", "aaa" + std::to_string(i));
  }
  ts1.Close();

  ts2.BeforeFirst();
  std::cout << "Inserting " << n << " records into T2.\n";
  for (int i = 0; i < n; i++) {
    ts2.Insert();
    ts2.SetInt("C", n - i - 1);
    ts2.SetString("D", "bbb" + std::to_string(n - i - 1));
  }
  ts2.Close();

  std::unique_ptr<Scan> s1 = std::make_unique<TableScan>(txn, "T1", layout1);
  std::unique_ptr<Scan> s2 = std::make_unique<TableScan>(txn, "T2", layout2);
  std::unique_ptr<Scan> s3 = std::make_unique<ProductScan>(std::move(s1), std::move(s2));
  while (s3->Next()) {
    std::cout << s3->GetString("B") << '\n';
  }
  s3->Close();

  txn.Commit();
}
}  // namespace simpledb

int main() {
  simpledb::ProductTest();

  return 0;
}
