#include "record/layout.h"

#include <utility>

namespace simpledb {
int Layout::GetOffset(std::string_view field_name) const {
  auto entry = offsets_.find(field_name);
  if (entry == offsets_.end()) {
    throw std::runtime_error("The field does not exist in this schema");
  }
  return entry->second;
}

int Layout::LengthInBytes(std::string_view field_name) const {
  int field_type = schema_.Type(field_name);
  if (field_type == INTEGER) {
    return sizeof(int);
  } else {  // field_type == VARCHAR
    return schema_.Length(field_name) + sizeof(int);
  }
}

void Layout::CreateLayout() {
  int pos = sizeof(int);  // leave space for the empty/inuse flag
  for (const auto& field_name : schema_.Fields()) {
    offsets_.emplace(field_name, pos);
    pos += LengthInBytes(field_name);
  }
  slot_size_ = pos;
}
}  // namespace simpledb
