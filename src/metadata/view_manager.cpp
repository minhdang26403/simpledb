#include "metadata/view_manager.h"

#include <utility>

#include "record/layout.h"
#include "record/table_scan.h"

namespace simpledb {
ViewManager::ViewManager(bool is_new, TableManager& table_manager,
                         Transaction& txn)
    : table_manager_(table_manager) {
  Schema schema;
  schema.AddStringField("view_name", TableManager::MAX_NAME_LEN);
  schema.AddStringField("view_def", MAX_VIEW_DEF);
  if (is_new) {
    table_manager_.CreateTable("view_catalog", schema, txn);
  }
  layout_ = Layout{std::move(schema)};
}

void ViewManager::CreateView(std::string_view view_name,
                             std::string_view view_def, Transaction& txn) {
  TableScan view_catalog{txn, "view_catalog", layout_};
  view_catalog.Insert();
  view_catalog.SetString("view_name", view_name);
  view_catalog.SetString("view_def", view_def);
  view_catalog.Close();
}

std::string_view ViewManager::GetViewDef(std::string_view view_name,
                                         Transaction& txn) {
  TableScan view_catalog{txn, "view_catalog", layout_};
  std::optional<std::string_view> result;
  while (view_catalog.Next()) {
    if (view_catalog.GetString("view_name") == view_name) {
      result = view_catalog.GetString("view_def");
      break;
    }
  }
  view_catalog.Close();

  if (!result.has_value()) {
    throw std::runtime_error("The view does not exist");
  }

  return result.value();
}
}  // namespace simpledb
