#include "plan/planner.h"

#include <type_traits>
#include <variant>

#include "parse/create_index_data.h"
#include "parse/create_table_data.h"
#include "parse/create_view_data.h"
#include "parse/delete_data.h"
#include "parse/insert_data.h"
#include "parse/modify_data.h"
#include "parse/parser.h"

namespace simpledb {
std::shared_ptr<Plan> Planner::CreateQueryPlan(const std::string& query,
                                               Transaction& txn) const {
  Parser parser{query};
  auto data = parser.ParseQuery();
  return query_planner_->CreatePlan(data, txn);
}

int Planner::ExecuteUpdate(const std::string& command, Transaction& txn) const {
  Parser parser{command};
  auto data = parser.ParseUpdateCommand();
  return std::visit(
      [this, &txn](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, InsertData>) {
          return update_planner_->ExecuteInsert(arg, txn);
        } else if constexpr (std::is_same_v<T, DeleteData>) {
          return update_planner_->ExecuteDelete(arg, txn);
        } else if constexpr (std::is_same_v<T, ModifyData>) {
          return update_planner_->ExecuteModify(arg, txn);
        } else if constexpr (std::is_same_v<T, CreateTableData>) {
          return update_planner_->ExecuteCreateTable(arg, txn);
        } else if constexpr (std::is_same_v<T, CreateViewData>) {
          return update_planner_->ExecuteCreateView(arg, txn);
        } else if constexpr (std::is_same_v<T, CreateIndexData>) {
          return update_planner_->ExecuteCreateIndex(arg, txn);
        } else {
          static_assert(always_false_v<T>, "non-exhaustive visitor!");
          return 0;
        }
      },
      data);
}
}  // namespace simpledb
