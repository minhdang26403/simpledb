#include "index/planner/index_join_plan.h"

#include <utility>

#include "index/query/index_join_scan.h"
#include "record/table_scan.h"

namespace simpledb {
IndexJoinPlan::IndexJoinPlan(const std::shared_ptr<Plan>& plan1,
                             const std::shared_ptr<Plan>& plan2,
                             const std::shared_ptr<IndexInfo>& index_info,
                             std::string_view join_field)
    : plan1_(plan1),
      plan2_(plan2),
      index_info_(index_info),
      join_field_(join_field) {
  schema_.AddAll(plan1_->GetSchema());
  schema_.AddAll(plan2_->GetSchema());
}

std::unique_ptr<Scan> IndexJoinPlan::Open() {
  auto scan = plan1_->Open();
  auto table_scan = dynamic_cast<TableScan*>(plan2_->Open().release());
  if (table_scan == nullptr) {
    throw std::bad_cast{};
  }
  auto index = index_info_->Open();
  return std::make_unique<IndexJoinScan>(
      std::move(scan), std::move(index), join_field_,
      std::unique_ptr<TableScan>(table_scan));
}

int IndexJoinPlan::BlocksAccessed() const noexcept {
  return plan1_->BlocksAccessed() +
         plan1_->RecordsOutput() * index_info_->BlocksAccessed() +
         RecordsOutput();
}

int IndexJoinPlan::RecordsOutput() const noexcept {
  return plan1_->RecordsOutput() * index_info_->RecordsOutput();
}

int IndexJoinPlan::DistinctValues(std::string_view field_name) const noexcept {
  if (plan1_->GetSchema().HasField(field_name)) {
    return plan1_->DistinctValues(field_name);
  } else {
    return plan2_->DistinctValues(field_name);
  }
}

/**
 * @brief Return the schema of the index join
 * @return the index join's schema
 */
Schema& IndexJoinPlan::GetSchema() noexcept { return schema_; }
}  // namespace simpledb
