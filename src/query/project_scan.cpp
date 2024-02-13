#include "query/project_scan.h"

#include <string>

namespace simpledb {
ProjectScan::ProjectScan(Scan& scan, const StringSet& field_list)
    : scan_(scan), field_list_(field_list) {}

void ProjectScan::BeforeFirst() { scan_.BeforeFirst(); }

bool ProjectScan::Next() { return scan_.Next(); }

int ProjectScan::GetInt(std::string_view field_name) {
  if (HasField(field_name)) {
    return scan_.GetInt(field_name);
  }

  throw std::runtime_error(ErrorMessage(field_name));
}

std::string_view ProjectScan::GetString(std::string_view field_name) {
  if (HasField(field_name)) {
    return scan_.GetString(field_name);
  }

  throw std::runtime_error(ErrorMessage(field_name));
}

Constant ProjectScan::GetVal(std::string_view field_name) {
  if (HasField(field_name)) {
    return scan_.GetVal(field_name);
  }

  throw std::runtime_error(ErrorMessage(field_name));
}

bool ProjectScan::HasField(std::string_view field_name) {
  return field_list_.contains(field_name);
}

void ProjectScan::Close() { scan_.Close(); }

std::string ProjectScan::ErrorMessage(std::string_view field_name) const {
  std::string err_msg;
  err_msg.append("field ");
  err_msg.append(field_name);
  err_msg.append(" not found");

  return err_msg;
}
}  // namespace simpledb
