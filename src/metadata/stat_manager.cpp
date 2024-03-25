#include "metadata/stat_manager.h"

#include <string>

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
  auto table_catalog_layout = table_manager_.GetLayout("table_catalog", txn);
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
  TableScan ts{txn, table_name, layout};

  while (ts.Next()) {
    num_blocks = ts.GetRID().BlockNumber() + 1;
    num_records++;
  }
  ts.Close();

  return StatInfo{num_blocks, num_records};
}
}  // namespace simpledb
