#include "plan/better_query_planner.h"

#include <utility>
#include <vector>

#include "parse/parser.h"
#include "plan/product_plan.h"
#include "plan/project_plan.h"
#include "plan/select_plan.h"
#include "plan/table_plan.h"

namespace simpledb {
std::shared_ptr<Plan> BetterQueryPlanner::CreatePlan(const QueryData& data,
                                                     Transaction& txn) {
  // Step 1: Create a plan for each mentioned table or view
  std::vector<std::shared_ptr<Plan>> plans;
  for (const auto& table_name : data.GetTables()) {
    auto view_def_opt = metadata_manager_.GetViewDef(table_name, txn);
    if (view_def_opt.has_value()) {
      Parser parser{view_def_opt.value()};
      auto view_data = parser.ParseQuery();
      plans.push_back(CreatePlan(view_data, txn));
    } else {
      plans.push_back(
          std::make_shared<TablePlan>(txn, table_name, metadata_manager_));
    }
  }

  // Step 2: Create the product of all table plans
  auto plan = std::move(plans.front());
  plans.erase(plans.begin());
  for (const auto& next_plan : plans) {
    auto choice1 = std::make_shared<ProductPlan>(next_plan, plan);
    auto choice2 = std::make_shared<ProductPlan>(plan, next_plan);
    if (choice1->BlocksAccessed() < choice2->BlocksAccessed()) {
      plan = choice1;
    } else {
      plan = choice2;
    }
  }

  // Step 3: Add a selection plan for the predicate
  plan = std::make_shared<SelectPlan>(plan, data.GetPredicate());

  // Step 4: Project on the field names
  plan = std::make_shared<ProjectPlan>(plan, data.GetFields());

  return plan;
}
}  // namespace simpledb
