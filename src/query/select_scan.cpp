#include "query/select_scan.h"

namespace simpledb {
SelectScan::SelectScan(Scan& scan, Predicate& predicate)
    : scan_(scan), predicate_(predicate) {}

void SelectScan::BeforeFirst() { scan_.BeforeFirst(); }

bool SelectScan::Next() {
  while (scan_.Next()) {
    if (predicate_.IsSatisfied(scan_)) {
      return true;
    }
  }
  return false;
}

int SelectScan::GetInt(std::string_view field_name) {
  return scan_.GetInt(field_name);
}

std::string_view SelectScan::GetString(std::string_view field_name) {
  return scan_.GetString(field_name);
}

Constant SelectScan::GetVal(std::string_view field_name) {
  return scan_.GetVal(field_name);
}

bool SelectScan::HasField(std::string_view field_name) {
  return scan_.HasField(field_name);
}

void SelectScan::Close() { scan_.Close(); }

// UpdateScan methods
void SelectScan::SetInt(std::string_view field_name, int val) {
  dynamic_cast<UpdateScan&>(scan_).SetInt(field_name, val);
}

void SelectScan::SetString(std::string_view field_name, std::string_view val) {
  dynamic_cast<UpdateScan&>(scan_).SetString(field_name, val);
}

void SelectScan::SetVal(std::string_view field_name, const Constant& val) {
  dynamic_cast<UpdateScan&>(scan_).SetVal(field_name, val);
}

void SelectScan::Delete() { dynamic_cast<UpdateScan&>(scan_).Delete(); }

void SelectScan::Insert() { dynamic_cast<UpdateScan&>(scan_).Insert(); }

RID SelectScan::GetRID() const {
  return dynamic_cast<UpdateScan&>(scan_).GetRID();
}

void SelectScan::MoveToRID(const RID& rid) {
  dynamic_cast<UpdateScan&>(scan_).MoveToRID(rid);
}
}  // namespace simpledb