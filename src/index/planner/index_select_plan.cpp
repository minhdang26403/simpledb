#include "index/planner/index_select_plan.h"

#include <utility>

namespace simpledb {
std::unique_ptr<Scan> IndexSelectPlan::Open() {
  auto table_scan = dynamic_cast<TableScan*>(plan_->Open().release());
  if (table_scan == nullptr) {
    throw std::bad_cast{};
  }
  auto index = index_info_->Open();
  return std::make_unique<IndexSelectScan>(
      std::unique_ptr<TableScan>(table_scan), std::move(index), value_);
}

int IndexSelectPlan::BlocksAccessed() const noexcept {
  return index_info_->BlocksAccessed() + RecordsOutput();
}

int IndexSelectPlan::RecordsOutput() const noexcept {
  return index_info_->RecordsOutput();
}

int IndexSelectPlan::DistinctValues(
    std::string_view field_name) const noexcept {
  return index_info_->DistinctValues(field_name);
}

Schema& IndexSelectPlan::GetSchema() noexcept { return plan_->GetSchema(); }
}  // namespace simpledb
