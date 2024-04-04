#include "index/query/index_select_scan.h"

namespace simpledb {
void IndexSelectScan::BeforeFirst() { index_->BeforeFirst(value_); }

bool IndexSelectScan::Next() {
  auto ok = index_->Next();
  if (ok) {
    auto rid = index_->GetRID();
    table_scan_->MoveToRID(rid);
  }
  return ok;
}

int IndexSelectScan::GetInt(std::string_view field_name) {
  return table_scan_->GetInt(field_name);
}

std::string_view IndexSelectScan::GetString(std::string_view field_name) {
  return table_scan_->GetString(field_name);
}

Constant IndexSelectScan::GetVal(std::string_view field_name) {
  return table_scan_->GetVal(field_name);
}

bool IndexSelectScan::HasField(std::string_view field_name) {
  return table_scan_->HasField(field_name);
}

void IndexSelectScan::Close() {
  index_->Close();
  table_scan_->Close();
}
}  // namespace simpledb
