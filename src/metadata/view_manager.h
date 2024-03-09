#pragma once

#include <string_view>

#include "metadata/table_manager.h"
#include "record/layout.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The view manager. There are methods to create views and retrieve their
 * definitions when requested
 */
class ViewManager {
 public:
  /**
   * @brief Create a new view manager for the database system. If the
   * database is new, the view catalog table is created.
   * @param is_new whether the database is new
   * @param table_manager a reference to the table manager
   * @param txn the startup transaction
   */
  ViewManager(bool is_new, TableManager& table_manager, Transaction& txn);

  /**
   * @brief Create a new view having the specified name and definiton
   * @param view_name name of the new view
   * @param view_def the view's definition
   * @param txn the transaction creating view
   */
  void CreateView(std::string_view view_name, std::string_view view_def,
                  Transaction& txn);

  /**
   * @brief Retrieve the definition of the specified view from the catalog
   * @param view_name name of the view
   * @param txn the transaction retrieving view definition
   * @return definition of the view
   */
  std::string GetViewDef(std::string_view view_name, Transaction& txn);

 private:
  static constexpr int MAX_VIEW_DEF = 100;
  TableManager& table_manager_;
  Layout layout_;
};
}  // namespace simpledb
