#include "query/product_scan.h"

namespace simpledb {
ProductScan::ProductScan(Scan& scan1, Scan& scan2)
    : scan1_(scan1), scan2_(scan2) {
  BeforeFirst();
}

void ProductScan::BeforeFirst() {
  scan1_.BeforeFirst();
  scan1_.Next();
  scan2_.BeforeFirst();
}

bool ProductScan::Next() {
  if (scan2_.Next()) {
    return true;
  }
  scan2_.BeforeFirst();
  return scan1_.Next() && scan2_.Next();
}

int ProductScan::GetInt(std::string_view field_name) {
  if (scan1_.HasField(field_name)) {
    return scan1_.GetInt(field_name);
  }
  return scan2_.GetInt(field_name);
}

std::string_view ProductScan::GetString(std::string_view field_name) {
  if (scan1_.HasField(field_name)) {
    return scan1_.GetString(field_name);
  }
  return scan2_.GetString(field_name);
}

Constant ProductScan::GetVal(std::string_view field_name) {
  if (scan1_.HasField(field_name)) {
    return scan1_.GetVal(field_name);
  }
  return scan2_.GetVal(field_name);
}

bool ProductScan::HasField(std::string_view field_name) {
  return scan1_.HasField(field_name) || scan2_.HasField(field_name);
}

void ProductScan::Close() {
  scan1_.Close();
  scan2_.Close();
}
}  // namespace simpledb
