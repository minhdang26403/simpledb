#include "metadata/stat_manager.h"

#include <string>
#include <unordered_set>

#include "metadata/stat_info.h"
#include "metadata/table_manager.h"
#include "query/constant.h"
#include "record/table_scan.h"
#include "txn/transaction.h"
#include "utils/data_type.h"

namespace simpledb {
StatManager::StatManager(TableManager& table_manager, Transaction& txn)
    : table_manager_(table_manager) {
  RefreshStatistics(txn);
}

StatInfo StatManager::GetStatInfo(std::string_view table_name, Layout& layout,
                                  Transaction& txn) {
  std::scoped_lock lock{mutex_};
  num_calls_++;
  if (num_calls_ > 100) {
    RefreshStatistics(txn);
  }
  if (!table_stats_.contains(table_name)) {
    table_stats_.emplace(table_name,
                         CalculateTableStats(table_name, layout, txn));
  }

  return table_stats_.find(table_name)->second;
}

void StatManager::RefreshStatistics(Transaction& txn) {
  table_stats_ = StringHashMap<StatInfo>{};
  num_calls_ = 0;
  Layout table_catalog_layout = table_manager_.GetLayout("table_catalog", txn);
  TableScan table_catalog{txn, "table_catalog", table_catalog_layout};

  while (table_catalog.Next()) {
    std::string table_name{table_catalog.GetString("table_name")};
    Layout layout = table_manager_.GetLayout(table_name, txn);
    StatInfo stat_info = CalculateTableStats(table_name, layout, txn);
    table_stats_.emplace(table_name, stat_info);
  }

  table_catalog.Close();
}

StatInfo StatManager::CalculateTableStats(std::string_view table_name,
                                          Layout& layout, Transaction& txn) {
  int num_blocks = 0;
  int num_records = 0;
  auto constant_hash = [](const Constant& val) {
    const auto& underlying_val = val.AsVariant();
    using variant = std::decay_t<decltype(underlying_val)>;
    return std::hash<variant>{}(underlying_val);
  };
  StringHashMap<std::unordered_set<Constant, decltype(constant_hash)>>
      distinct_values;
  Schema& schema = layout.GetSchema();
  TableScan ts{txn, table_name, layout};

  while (ts.Next()) {
    num_blocks = ts.GetRID().BlockNumber() + 1;
    num_records++;
    for (const auto& field : schema.Fields()) {
      if (schema.Type(field) == INTEGER) {
        distinct_values[field].insert(Constant{ts.GetInt(field)});
      } else {
        distinct_values[field].insert(Constant{ts.GetString(field)});
      }
    }
  }
  ts.Close();

  StringHashMap<int> num_distinct_values;
  for (const auto& [field, values] : distinct_values) {
    num_distinct_values.emplace(field, values.size());
  }

  return StatInfo{num_blocks, num_records, num_distinct_values};
}
}  // namespace simpledb
