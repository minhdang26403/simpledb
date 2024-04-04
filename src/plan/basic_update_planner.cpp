#include "plan/basic_update_planner.h"

#include <memory>
#include <utility>

#include "plan/select_plan.h"
#include "plan/table_plan.h"

namespace simpledb {

int BasicUpdatePlanner::ExecuteInsert(const InsertData& data,
                                      Transaction& txn) {
  std::unique_ptr<Plan> plan =
      std::make_unique<TablePlan>(txn, data.TableName(), metadata_manager_);
  auto scan = plan->Open();

  auto update_scan = dynamic_cast<UpdateScan*>(scan.get());
  if (update_scan == nullptr) {
    throw std::bad_cast{};
  }

  update_scan->Insert();
  const auto& fields = data.Fields();
  const auto& values = data.Values();
  assert(fields.size() == values.size());
  for (size_t i = 0; i < fields.size(); i++) {
    update_scan->SetVal(fields[i], values[i]);
  }
  update_scan->Close();

  return 1;
}

int BasicUpdatePlanner::ExecuteDelete(const DeleteData& data,
                                      Transaction& txn) {
  std::shared_ptr<Plan> plan =
      std::make_shared<TablePlan>(txn, data.TableName(), metadata_manager_);
  plan =
      std::make_shared<SelectPlan>(std::move(plan), data.DeletionPredicate());
  auto scan = plan->Open();

  auto update_scan = dynamic_cast<UpdateScan*>(scan.get());
  if (update_scan == nullptr) {
    throw std::bad_cast{};
  }

  int count = 0;
  while (update_scan->Next()) {
    update_scan->Delete();
    count++;
  }
  update_scan->Close();

  return count;
}

int BasicUpdatePlanner::ExecuteModify(const ModifyData& data,
                                      Transaction& txn) {
  std::shared_ptr<Plan> plan =
      std::make_shared<TablePlan>(txn, data.TableName(), metadata_manager_);
  plan = std::make_shared<SelectPlan>(std::move(plan),
                                      data.ModificationPredicate());
  auto scan = plan->Open();

  auto update_scan = dynamic_cast<UpdateScan*>(scan.get());
  if (update_scan == nullptr) {
    throw std::bad_cast{};
  }

  int count = 0;
  while (update_scan->Next()) {
    update_scan->SetVal(data.TargetField(),
                        data.NewValue().Evaluate(*update_scan));
    count++;
  }
  update_scan->Close();

  return count;
}

int BasicUpdatePlanner::ExecuteCreateTable(const CreateTableData& data,
                                           Transaction& txn) {
  metadata_manager_.CreateTable(data.TableName(), data.NewSchema(), txn);

  return 0;
}

int BasicUpdatePlanner::ExecuteCreateView(const CreateViewData& data,
                                          Transaction& txn) {
  metadata_manager_.CreateView(data.ViewName(), data.ViewDefinition(), txn);

  return 0;
}

int BasicUpdatePlanner::ExecuteCreateIndex(const CreateIndexData& data,
                                           Transaction& txn) {
  metadata_manager_.CreateIndex(data.IndexName(), data.TableName(),
                                data.FieldName(), txn);

  return 0;
}
}  // namespace simpledb
