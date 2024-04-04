#include "index/planner/index_update_planner.h"

#include <memory>
#include <utility>

#include "plan/select_plan.h"
#include "plan/table_plan.h"
#include "query/update_scan.h"

namespace simpledb {
int IndexUpdatePlanner::ExecuteInsert(const InsertData& data,
                                      Transaction& txn) {
  auto table_name = data.TableName();
  std::unique_ptr<Plan> plan =
      std::make_unique<TablePlan>(txn, table_name, metadata_manager_);

  // first, insert the record
  auto scan = plan->Open();
  auto update_scan = dynamic_cast<UpdateScan*>(scan.get());
  if (update_scan == nullptr) {
    throw std::bad_cast{};
  }
  update_scan->Insert();
  auto rid = update_scan->GetRID();

  // then modify each field, inserting an index record if appropriate
  auto indexes = metadata_manager_.GetIndexInfo(table_name, txn);
  const auto& fields = data.Fields();
  const auto& values = data.Values();
  assert(fields.size() == values.size());
  for (size_t i = 0; i < fields.size(); i++) {
    const auto& field_name = fields[i];
    const auto& value = values[i];
    update_scan->SetVal(field_name, value);

    if (indexes.contains(field_name)) {
      auto index_info = indexes.at(field_name);
      auto index = index_info.Open();
      index->Insert(value, rid);
      index->Close();
    }
  }
  update_scan->Close();
  return 1;
}

int IndexUpdatePlanner::ExecuteDelete(const DeleteData& data,
                                      Transaction& txn) {
  auto table_name = data.TableName();
  std::unique_ptr<Plan> plan =
      std::make_unique<TablePlan>(txn, table_name, metadata_manager_);
  plan =
      std::make_unique<SelectPlan>(std::move(plan), data.DeletionPredicate());
  auto indexes = metadata_manager_.GetIndexInfo(table_name, txn);

  auto scan = plan->Open();
  auto update_scan = dynamic_cast<UpdateScan*>(scan.get());
  if (update_scan == nullptr) {
    throw std::bad_cast{};
  }

  int count = 0;
  while (update_scan->Next()) {
    // first, delete the record's RID from every index
    auto rid = update_scan->GetRID();
    for (const auto& [field_name, _] : indexes) {
      auto value = update_scan->GetVal(field_name);
      auto index = indexes.at(field_name).Open();
      index->Delete(value, rid);
      index->Close();
    }
    // then delete the record
    update_scan->Delete();
    count++;
  }

  update_scan->Close();
  return count;
}

int IndexUpdatePlanner::ExecuteModify(const ModifyData& data,
                                      Transaction& txn) {
  auto table_name = data.TableName();
  auto field_name = data.TargetField();
  std::unique_ptr<Plan> plan =
      std::make_unique<TablePlan>(txn, table_name, metadata_manager_);
  plan = std::make_unique<SelectPlan>(std::move(plan),
                                      data.ModificationPredicate());

  auto indexes = metadata_manager_.GetIndexInfo(table_name, txn);
  std::unique_ptr<Index> index{nullptr};
  if (indexes.contains(field_name)) {
    auto index_info = indexes.at(field_name);
    index = index_info.Open();
  }

  auto scan = plan->Open();
  auto update_scan = dynamic_cast<UpdateScan*>(scan.get());
  if (update_scan == nullptr) {
    throw std::bad_cast{};
  }

  int count = 0;
  while (update_scan->Next()) {
    // first, update the record
    auto new_val = data.NewValue().Evaluate(*update_scan);
    auto old_val = update_scan->GetVal(field_name);
    update_scan->SetVal(data.TargetField(), new_val);

    // then update the appropriate index, if it exists
    if (index != nullptr) {
      auto rid = update_scan->GetRID();
      index->Delete(old_val, rid);
      index->Insert(new_val, rid);
    }
    count++;
  }
  if (index != nullptr) {
    index->Close();
  }
  update_scan->Close();
  return count;
}

int IndexUpdatePlanner::ExecuteCreateTable(const CreateTableData& data,
                                           Transaction& txn) {
  metadata_manager_.CreateTable(data.TableName(), data.NewSchema(), txn);
  return 0;
}

int IndexUpdatePlanner::ExecuteCreateView(const CreateViewData& data,
                                          Transaction& txn) {
  metadata_manager_.CreateView(data.ViewName(), data.ViewDefinition(), txn);
  return 0;
}

int IndexUpdatePlanner::ExecuteCreateIndex(const CreateIndexData& data,
                                           Transaction& txn) {
  metadata_manager_.CreateIndex(data.IndexName(), data.TableName(),
                                data.FieldName(), txn);
  return 0;
}
}  // namespace simpledb
