#include "metadata/index_manager.h"

#include "record/table_scan.h"
#include "utils/data_type.h"

namespace simpledb {
IndexManager::IndexManager(bool is_new, TableManager& table_manager,
                           StatManager& stat_manager, Transaction& txn)
    : table_manager_(table_manager), stat_manager_(stat_manager) {
  if (is_new) {
    Schema schema;
    schema.AddStringField("index_name", TableManager::MAX_NAME_LEN);
    schema.AddStringField("table_name", TableManager::MAX_NAME_LEN);
    schema.AddStringField("field_name", TableManager::MAX_NAME_LEN);
    table_manager_.CreateTable("index_catalog", schema, txn);
  }
  layout_ = table_manager_.GetLayout("index_catalog", txn);
}

void IndexManager::CreateIndex(std::string_view index_name,
                               std::string_view table_name,
                               std::string_view field_name, Transaction& txn) {
  TableScan index_catalog{txn, "index_catalog", layout_};
  index_catalog.Insert();
  index_catalog.SetString("index_name", index_name);
  index_catalog.SetString("table_name", table_name);
  index_catalog.SetString("field_name", field_name);
  index_catalog.Close();
}

StringHashMap<IndexInfo> IndexManager::GetIndexInfo(std::string_view table_name,
                                                    Transaction& txn) {
  StringHashMap<IndexInfo> result;
  TableScan index_catalog{txn, "index_catalog", layout_};

  while (index_catalog.Next()) {
    if (index_catalog.GetString("table_name") == table_name) {
      auto index_name = index_catalog.GetString("index_name");
      auto field_name = index_catalog.GetString("field_name");
      auto table_layout = table_manager_.GetLayout(table_name, txn);
      auto table_stat_info =
          stat_manager_.GetStatInfo(table_name, table_layout, txn);
      result.emplace(std::piecewise_construct,
                     std::forward_as_tuple(field_name),
                     std::forward_as_tuple(index_name, field_name,
                                           table_layout.GetSchema(), txn,
                                           table_stat_info));
    }
  }
  index_catalog.Close();

  return result;
}
}  // namespace simpledb
