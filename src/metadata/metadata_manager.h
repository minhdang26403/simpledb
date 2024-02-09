#pragma once

#include <string_view>

#include "metadata/index_info.h"
#include "metadata/index_manager.h"
#include "metadata/stat_info.h"
#include "metadata/stat_manager.h"
#include "metadata/table_manager.h"
#include "metadata/view_manager.h"
#include "record/layout.h"
#include "record/schema.h"
#include "txn/transaction.h"

namespace simpledb {
class MetadataManager {
 public:
  MetadataManager(bool is_new, Transaction& txn)
      : table_manager_(is_new, txn),
        view_manager_(is_new, table_manager_, txn),
        stat_manager_(table_manager_, txn),
        index_manager_(is_new, table_manager_, stat_manager_, txn) {}

  void CreateTable(std::string_view table_name, const Schema& schema,
                   Transaction& txn) {
    table_manager_.CreateTable(table_name, schema, txn);
  }

  Layout GetLayout(std::string_view table_name, Transaction& txn) {
    return table_manager_.GetLayout(table_name, txn);
  }

  void CreateView(std::string_view view_name, std::string_view view_def,
                  Transaction& txn) {
    view_manager_.CreateView(view_name, view_def, txn);
  }

  std::string_view GetViewDef(std::string_view view_name, Transaction& txn) {
    return view_manager_.GetViewDef(view_name, txn);
  }

  void CreateIndex(std::string_view index_name, std::string_view table_name,
                   std::string_view field_name, Transaction& txn) {
    index_manager_.CreateIndex(index_name, table_name, field_name, txn);
  }

  StringHashMap<IndexInfo> GetIndexInfo(std::string_view table_name,
                                        Transaction& txn) {
    return index_manager_.GetIndexInfo(table_name, txn);
  }

  StatInfo GetStatInfo(std::string_view table_name, Layout& layout,
                       Transaction& txn) {
    return stat_manager_.GetStatInfo(table_name, layout, txn);
  }

 private:
  TableManager table_manager_;
  ViewManager view_manager_;
  StatManager stat_manager_;
  IndexManager index_manager_;
};
}  // namespace simpledb
