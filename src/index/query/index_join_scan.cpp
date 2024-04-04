#include "index/query/index_join_scan.h"

namespace simpledb {
void IndexJoinScan::BeforeFirst() {
  lhs_->BeforeFirst();
  lhs_->Next();
  ResetIndex();
}

bool IndexJoinScan::Next() {
  while (true) {
    if (index_->Next()) {
      rhs_->MoveToRID(index_->GetRID());
      return true;
    }
    if (!lhs_->Next()) {
      return false;
    }
    ResetIndex();
  }
}

int IndexJoinScan::GetInt(std::string_view field_name) {
  if (rhs_->HasField(field_name)) {
    return rhs_->GetInt(field_name);
  } else {
    return lhs_->GetInt(field_name);
  }
}

std::string_view IndexJoinScan::GetString(std::string_view field_name) {
  if (rhs_->HasField(field_name)) {
    return rhs_->GetString(field_name);
  } else {
    return lhs_->GetString(field_name);
  }
}

Constant IndexJoinScan::GetVal(std::string_view field_name) {
  if (rhs_->HasField(field_name)) {
    return rhs_->GetVal(field_name);
  } else {
    return lhs_->GetVal(field_name);
  }
}

bool IndexJoinScan::HasField(std::string_view field_name) {
  return rhs_->HasField(field_name) || lhs_->HasField(field_name);
}

void IndexJoinScan::Close() {
  lhs_->Close();
  index_->Close();
  rhs_->Close();
}

void IndexJoinScan::ResetIndex() {
  auto search_key = lhs_->GetVal(join_field_);
  index_->BeforeFirst(search_key);
}
}  // namespace simpledb
