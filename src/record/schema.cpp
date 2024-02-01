#include "record/schema.h"
#include "utils/data_type.h"

namespace simpledb {
void Schema::AddField(std::string_view field_name, int type, int length) {
  fields_.emplace_back(field_name);
  info_.emplace(std::piecewise_construct, std::forward_as_tuple(field_name),
                std::forward_as_tuple(type, length));
}

void Schema::AddIntField(std::string_view field_name) {
  AddField(field_name, INTEGER, 0);
}

void Schema::AddStringField(std::string_view field_name, int length) {
  AddField(field_name, VARCHAR, length);
}

void Schema::Add(std::string_view field_name, const Schema& schema) {
  int type = schema.Type(field_name);
  int length = schema.Length(field_name);
  AddField(field_name, type, length);
}

void Schema::AddAll(const Schema& schema) {
  for (const auto& field_name : schema.Fields()) {
    Add(field_name, schema);
  }
}

bool Schema::HasField(std::string_view field_name) const noexcept {
  return info_.contains(field_name);
}

int Schema::Type(std::string_view field_name) const {
  return GetFieldInfo(field_name).type_;
}

int Schema::Length(std::string_view field_name) const {
  return GetFieldInfo(field_name).length_;
}

const Schema::FieldInfo& Schema::GetFieldInfo(
    std::string_view field_name) const {
  auto entry = info_.find(field_name);
  if (entry == info_.end()) {
    throw std::runtime_error("The field does not exist in this schema");
  }
  return entry->second;
}
}  // namespace simpledb
