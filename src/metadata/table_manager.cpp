#include "metadata/table_manager.h"

#include <exception>
#include <memory>
#include <string>
#include <utility>

#include "record/layout.h"
#include "record/table_scan.h"
#include "utils/data_type.h"

namespace simpledb {
TableManager::TableManager(bool is_new, Transaction& txn) {
  Schema table_catalog_schema;
  table_catalog_schema.AddStringField("table_name", MAX_NAME_LEN);
  table_catalog_schema.AddIntField("slot_size");
  table_catalog_layout_ = Layout{table_catalog_schema};

  Schema field_catalog_schema;
  field_catalog_schema.AddStringField("table_name", MAX_NAME_LEN);
  field_catalog_schema.AddStringField("field_name", MAX_NAME_LEN);
  field_catalog_schema.AddIntField("type");
  field_catalog_schema.AddIntField("length");
  field_catalog_schema.AddIntField("offset");
  field_catalog_layout_ = Layout{field_catalog_schema};

  if (is_new) {
    CreateTable("table_catalog", table_catalog_schema, txn);
    CreateTable("field_catalog", field_catalog_schema, txn);
  }
}

void TableManager::CreateTable(std::string_view table_name,
                               const Schema& schema, Transaction& txn) {
  Layout layout{schema};
  // insert one record into `table_catalog`
  TableScan table_catalog{txn, "table_catalog", table_catalog_layout_};
  table_catalog.Insert();
  table_catalog.SetString("table_name", table_name);
  table_catalog.SetInt("slot_size", layout.SlotSize());
  table_catalog.Close();

  // insert a record into `field_catalog` for each field
  TableScan field_catalog{txn, "field_catalog", field_catalog_layout_};
  for (const auto& field_name : schema.Fields()) {
    field_catalog.Insert();
    field_catalog.SetString("table_name", table_name);
    field_catalog.SetString("field_name", field_name);
    field_catalog.SetInt("type", schema.Type(field_name));
    field_catalog.SetInt("length", schema.Length(field_name));
    field_catalog.SetInt("offset", layout.GetOffset(field_name));
  }
  field_catalog.Close();
}

Layout TableManager::GetLayout(std::string_view table_name, Transaction& txn) {
  int slot_size = -1;
  TableScan table_catalog{txn, "table_catalog", table_catalog_layout_};

  while (table_catalog.Next()) {
    if (table_catalog.GetString("table_name") == table_name) {
      slot_size = table_catalog.GetInt("slot_size");
      break;
    }
  }
  table_catalog.Close();

  if (slot_size == -1) {
    throw std::runtime_error("The specified table does not exist");
  }

  Schema schema;
  HashMap<std::string, int> field_offsets;
  TableScan field_catalog{txn, "field_catalog", field_catalog_layout_};

  while (field_catalog.Next() &&
         field_catalog.GetString("table_name") != table_name) {
  }

  while (field_catalog.GetString("table_name") == table_name) {
    std::string_view field_name = field_catalog.GetString("field_name");
    int field_type = field_catalog.GetInt("type");
    int field_length = field_catalog.GetInt("length");
    int field_offset = field_catalog.GetInt("offset");
    schema.AddField(field_name, field_type, field_length);
    field_offsets.emplace(field_name, field_offset);
    if (!field_catalog.Next()) {
      break;
    }
  }
  field_catalog.Close();

  return Layout{std::move(schema), field_offsets, slot_size};
}
}  // namespace simpledb
