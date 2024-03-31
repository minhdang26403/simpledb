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
    if (ts.GetVal("data_val") == search_key_.value()) {
      return true;
    }
  }

  return false;
}

RID HashIndex::GetDataRID() {
  auto& ts = table_scan_.value();
  int block_num = ts.GetInt("block");
  int id = ts.GetInt("id");
  return RID{block_num, id};
}

void HashIndex::Insert(const Constant& data_val, const RID& data_rid) {
  BeforeFirst(data_val);
  auto& ts = table_scan_.value();
  ts.Insert();
  ts.SetInt("block", data_rid.BlockNumber());
  ts.SetInt("id", data_rid.Slot());
  ts.SetVal("data_val", data_val);
}

void HashIndex::Delete(const Constant& data_val, const RID& data_rid) {
  BeforeFirst(data_val);
  while (Next()) {
    if (GetDataRID() == data_rid) {
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
