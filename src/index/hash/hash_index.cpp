#include "index/hash/hash_index.h"

namespace simpledb {
void HashIndex::BeforeFirst(const Constant& search_key) {
  Close();
  search_key_.emplace(search_key);
  size_t bucket = KeyToBucketID(search_key_.value());
  auto table_name = index_name_ + std::to_string(bucket);
  table_scan_.emplace(txn_, table_name, layout_);
}

bool HashIndex::Next() {
  auto& ts = table_scan_.value();
  while (ts.Next()) {
    if (ts.GetVal("key") == search_key_.value()) {
      return true;
    }
  }

  return false;
}

RID HashIndex::GetRID() {
  auto& ts = table_scan_.value();
  int block_num = ts.GetInt("block");
  int id = ts.GetInt("id");
  return RID{block_num, id};
}

void HashIndex::Insert(const Constant& key, const RID& rid) {
  BeforeFirst(key);
  auto& ts = table_scan_.value();
  ts.Insert();
  ts.SetInt("block", rid.BlockNumber());
  ts.SetInt("id", rid.Slot());
  ts.SetVal("key", key);
}

void HashIndex::Delete(const Constant& key, const RID& rid) {
  BeforeFirst(key);
  while (Next()) {
    if (GetRID() == rid) {
      table_scan_.value().Delete();
      return;
    }
  }
}

void HashIndex::Close() {
  if (table_scan_.has_value()) {
    table_scan_.value().Close();
  }
}
}  // namespace simpledb
