#include "record/layout.h"

#include <iostream>
#include <utility>

#include "record/schema.h"

namespace simpledb {
void LayoutTest() {
  Schema schema;
  schema.AddIntField("A");
  schema.AddStringField("B", 9);

  Layout layout{std::move(schema)};
  for (const auto& field_name : layout.GetSchema().Fields()) {
    int offset = layout.GetOffset(field_name);
    std::cout << field_name << " has offset " << offset << '\n';
  }
}
}  // namespace simpledb

int main() {
  simpledb::LayoutTest();
}
