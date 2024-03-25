#include <iostream>
#include <utility>

#include "record/record_page.h"
#include "server/simpledb.h"

namespace simpledb {
void RecordTest() {
  SimpleDB db{"record_test", 400, 8};
  auto txn = db.NewTxn();

  Schema schema;
  schema.AddIntField("A");
  schema.AddStringField("B", 9);
  Layout layout{std::move(schema)};
  for (const auto& field_name : layout.GetSchema().Fields()) {
    int offset = layout.GetOffset(field_name);
    std::cout << field_name << " has offset " << offset << '\n';
  }

  BlockId block{txn.Append("test_file")};
  txn.Pin(block);
  RecordPage record_page{txn, block, layout};
  record_page.Format();

  std::cout << "Filling the page with random records.\n";
  int slot = record_page.InsertAfter(-1);
  while (slot >= 0) {
    int n = arc4random() % 50;  // NOLINT(runtime/threadsafe_fn)
    record_page.SetInt(slot, "A", n);
    record_page.SetString(slot, "B", "rec" + std::to_string(n));
    std::cout << "inserting into slot " << slot << ": {" << n << ", "
              << "rec" << n << "}\n";
    slot = record_page.InsertAfter(slot);
  }

  std::cout << "Deleting these records, whose A-values are less than 25.\n";
  int count = 0;
  slot = record_page.NextAfter(-1);
  while (slot >= 0) {
    int a = record_page.GetInt(slot, "A");
    std::string_view b = record_page.GetString(slot, "B");
    if (a < 25) {
      count++;
      std::cout << "slot " << slot << ": {" << a << ", " << b << "}\n";
      record_page.Delete(slot);
    }
    slot = record_page.NextAfter(slot);
  }
  std::cout << count << " values under 25 were deleted.\n\n";

  std::cout << "Here are the remaining records.\n";
  slot = record_page.NextAfter(-1);
  while (slot >= 0) {
    int a = record_page.GetInt(slot, "A");
    std::string_view b = record_page.GetString(slot, "B");
    std::cout << "slot " << slot << ": {" << a << ", " << b << "}\n";
    slot = record_page.NextAfter(slot);
  }
  txn.Unpin(block);
  txn.Commit();
}
}  // namespace simpledb

int main() {
  simpledb::RecordTest();

  return 0;
}
