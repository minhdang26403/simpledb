#include "query/select_scan.h"

#include <iostream>
#include <utility>

namespace simpledb {
SelectScan::SelectScan(std::unique_ptr<Scan> scan, Predicate& predicate)
    : scan_(std::move(scan)), predicate_(predicate) {}

void SelectScan::BeforeFirst() { scan_->BeforeFirst(); }

bool SelectScan::Next() {
  while (scan_->Next()) {
    if (predicate_.IsSatisfied(*scan_)) {
      return true;
    }
  }
  return false;
}

int SelectScan::GetInt(std::string_view field_name) {
  return scan_->GetInt(field_name);
}

std::string_view SelectScan::GetString(std::string_view field_name) {
  return scan_->GetString(field_name);
}

Constant SelectScan::GetVal(std::string_view field_name) {
  return scan_->GetVal(field_name);
}

bool SelectScan::HasField(std::string_view field_name) {
  return scan_->HasField(field_name);
}

void SelectScan::Close() { scan_->Close(); }

// UpdateScan methods
void SelectScan::SetInt(std::string_view field_name, int val) {
  auto update_scan = dynamic_cast<UpdateScan*>(scan_.get());
  if (update_scan == nullptr) {
    std::cout << "SetInt: the underlying scan is not an update scan\n";
    return;
  }
  update_scan->SetInt(field_name, val);
}

void SelectScan::SetString(std::string_view field_name, std::string_view val) {
  auto update_scan = dynamic_cast<UpdateScan*>(scan_.get());
  if (update_scan == nullptr) {
    std::cout << "SetString: the underlying scan is not an update scan\n";
    return;
  }
  update_scan->SetString(field_name, val);
}

void SelectScan::SetVal(std::string_view field_name, const Constant& val) {
  auto update_scan = dynamic_cast<UpdateScan*>(scan_.get());
  if (update_scan == nullptr) {
    std::cout << "SetVal: the underlying scan is not an update scan\n";
    return;
  }
  update_scan->SetVal(field_name, val);
}

void SelectScan::Delete() {
  auto update_scan = dynamic_cast<UpdateScan*>(scan_.get());
  if (update_scan == nullptr) {
    std::cout << "Delete: the underlying scan is not an update scan\n";
    return;
  }
  update_scan->Delete();
}

void SelectScan::Insert() {
  auto update_scan = dynamic_cast<UpdateScan*>(scan_.get());
  if (update_scan == nullptr) {
    std::cout << "Insert: the underlying scan is not an update scan\n";
    return;
  }
  update_scan->Insert();
}

RID SelectScan::GetRID() const {
  auto update_scan = dynamic_cast<UpdateScan*>(scan_.get());
  if (update_scan == nullptr) {
    std::cout << "GetRID: the underlying scan is not an update scan\n";
    return RID{};
  }
  return update_scan->GetRID();
}

void SelectScan::MoveToRID(const RID& rid) {
  auto update_scan = dynamic_cast<UpdateScan*>(scan_.get());
  if (update_scan == nullptr) {
    std::cout << "MoveToRID: the underlying scan is not an update scan\n";
    return;
  }
  update_scan->MoveToRID(rid);
}
}  // namespace simpledb
