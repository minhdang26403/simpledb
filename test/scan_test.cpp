#include <iostream>
#include <memory>

#include "query/expression.h"
#include "query/predicate.h"
#include "query/product_scan.h"
#include "query/project_scan.h"
#include "query/select_scan.h"
#include "query/term.h"
#include "query/update_scan.h"
#include "record/schema.h"
#include "record/table_scan.h"
#include "server/simpledb.h"

namespace simpledb {
void ScanTest1() {
  SimpleDB db{"scan_test_1"};
  Transaction txn = db.NewTxn();

  Schema schema1;
  schema1.AddIntField("A");
  schema1.AddStringField("B", 9);
  Layout layout{schema1};
  std::unique_ptr<UpdateScan> scan1 =
      std::make_unique<TableScan>(txn, "T", layout);

  scan1->BeforeFirst();
  int n = 200;
  std::cout << "Inserting " << n << " random records.\n";
  for (int i = 0; i < n; i++) {
    scan1->Insert();
    int k = arc4random() % 50;
    scan1->SetInt("A", k);
    scan1->SetString("B", "rec" + std::to_string(k));
  }
  scan1->Close();

  std::unique_ptr<Scan> scan2 = std::make_unique<TableScan>(txn, "T", layout);
  // selecting all records where A = 10
  Constant constant{10};

  Expression lhs{"A"};
  Expression rhs{constant};

  // The lifetime of `lhs` and `rhs` must be greater than or equal to the
  // lifetime of `term`
  Term term{lhs, rhs};
  // Undefined behavior: dangling references
  // expression Term term {Expression{"A"}, Expression{constant}};

  Predicate predicate{term};
  std::cout << "The predicate is " << predicate.ToString() << '\n';
  std::unique_ptr<Scan> scan3 = std::make_unique<SelectScan>(*scan2, predicate);
  StringSet fields = {"B"};
  std::unique_ptr<Scan> scan4 = std::make_unique<ProjectScan>(*scan3, fields);
  while (scan4->Next()) {
    std::cout << scan4->GetString("B") << '\n';
  }
  scan4->Close();

  txn.Commit();
}

void ScanTest2() {
  SimpleDB db{"scan_test_2"};
  Transaction txn = db.NewTxn();

  int n = 200;
  Schema schema1;
  schema1.AddIntField("A");
  schema1.AddStringField("B", 9);
  Layout layout1{schema1};
  std::unique_ptr<UpdateScan> us1 =
      std::make_unique<TableScan>(txn, "T1", layout1);
  us1->BeforeFirst();
  std::cout << "Inserting " << n << " records into T1.\n";
  for (int i = 0; i < n; i++) {
    us1->Insert();
    us1->SetInt("A", i);
    us1->SetString("B", "bbb" + std::to_string(i));
  }
  us1->Close();

  Schema schema2;
  schema2.AddIntField("C");
  schema2.AddStringField("D", 9);
  Layout layout2{schema2};
  std::unique_ptr<UpdateScan> us2 =
      std::make_unique<TableScan>(txn, "T2", layout2);
  us2->BeforeFirst();
  std::cout << "Inserting " << n << " records into T2.\n";
  for (int i = 0; i < n; i++) {
    us2->Insert();
    us2->SetInt("C", n - i - 1);
    us2->SetString("D", "ddd" + std::to_string(n - i - 1));
  }
  us2->Close();

  std::unique_ptr<Scan> s1 = std::make_unique<TableScan>(txn, "T1", layout1);
  std::unique_ptr<Scan> s2 = std::make_unique<TableScan>(txn, "T2", layout2);
  std::unique_ptr<Scan> s3 = std::make_unique<ProductScan>(*s1, *s2);
  // selecting all records where A = C
  Expression lhs{"A"}, rhs{"C"};
  Term term{lhs, rhs};
  Predicate predicate{term};
  std::cout << "The predicate is " << predicate.ToString() << '\n';
  std::unique_ptr<Scan> s4 = std::make_unique<SelectScan>(*s3, predicate);

  // projecting on [B, D]
  StringSet fields = {"B", "D"};
  std::unique_ptr<Scan> s5 = std::make_unique<ProjectScan>(*s4, fields);
  while (s5->Next()) {
    std::cout << s5->GetString("B") << " " << s5->GetString("D") << '\n';
  }
  s5->Close();

  txn.Commit();
}
}  // namespace simpledb

int main() {
  simpledb::ScanTest1();
  simpledb::ScanTest2();

  return 0;
}
