#pragma once

#include "metadata/index_info.h"
#include "metadata/stat_manager.h"
#include "metadata/table_manager.h"
#include "record/layout.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * The index manager. The index manager has similar functionality to the table
 * manager.
 */
class IndexManager {
 public:
  /**
   * @brief Create the index manager. This constructor is called during system
   * startup. If the database is new, then the `index_catalog` table is created.
   * @param is_new indicate whether this is a new database
   * @param table_manager reference to the table manager
   * @param stat_manager reference to the statistics manager
   * @param txn the system startup transaction
   */
  IndexManager(bool is_new, TableManager& table_manager,
               StatManager& stat_manager, Transaction& txn);

  /**
   * @brief Create an index of the specified type for the specified field. A
   * unique ID is assigned to this index, and its information is stored in the
   * `index_catalog` table.
   * @param index_name name of the index
   * @param table_name name of the indexed table
   * @param field_name name of the indexed field
   * @param txn the calling transaction
   */
  void CreateIndex(std::string_view index_name, std::string_view table_name,
                   std::string_view field_name, Transaction& txn);

  /**
   * @brief Return a map containing the index info for all indexes on the
   * specified table
   * @param table_name name of the table
   * @param txn the calling transaction
   * @return a map of `IndexInfo` object, keyed by their field names
   */
  StringHashMap<IndexInfo> GetIndexInfo(std::string_view table_name,
                                        Transaction& txn);

 private:
  Layout layout_;
  TableManager& table_manager_;
  StatManager& stat_manager_;
};
}  // namespace simpledb
