#pragma once

#include <string>
#include <utility>

#include "parse/query_data.h"

namespace simpledb {
/**
 * Data for the SQL `create view` statement
 */
class CreateViewData {
 public:
  /**
   * @brief Save the view name and its definition
   * @param view_name the view name
   * @param query_data the view definition
   */
  template <typename Str, typename QD>
  CreateViewData(Str&& view_name, QD&& query_data)
      : view_name_(std::forward<Str>(view_name)),
        query_data_(std::forward<QD>(query_data)) {}

  /**
   * @brief Return the name of the new view
   * @return name of the new view
   */
  const auto& ViewName() const & noexcept { return view_name_; }

  auto ViewName() && noexcept { return std::move(view_name_); }

  /**
   * @brief Return the definition of the new view
   * @return definition of the new view
   */
  std::string ViewDefinition() const noexcept { return query_data_.ToString(); }

 private:
  std::string view_name_;
  QueryData query_data_;
};
}  // namespace simpledb
