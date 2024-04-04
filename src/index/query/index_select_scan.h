#pragma once

#include <memory>
#include <utility>

#include "index/index.h"
#include "query/scan.h"
#include "record/table_scan.h"

namespace simpledb {
/**
 * The scan class corresponding to the `select` relational algebra operator
 */
class IndexSelectScan : public Scan {
 public:
  /**
   * @brief Create an index select scan for the specified index and selection
   * constant
   * @param table_scan the scan of the underlying query
   * @param index the index
   * @param value the selection constant
   */
  IndexSelectScan(std::unique_ptr<TableScan> table_scan,
                  std::unique_ptr<Index> index, const Constant& value)
      : table_scan_(std::move(table_scan)),
        index_(std::move(index)),
        value_(value) {}

  /**
   * Position the scan before the first record, which in this case means
   * positioning the index before the first instance of the selection constant
   */
  void BeforeFirst() override;

  /**
   * @brief Move to the next record, which in this case means moving the index
   * to the next record satisfying the selection constant.
   * If there is a next record, the method moves the `table_scan_` to the
   * corresponding data record
   * @return false if there are no more such index records.
   */
  bool Next() override;

  /**
   * @brief Return the value of the field of the current data record
   * @param field_name name of the field
   * @return the field's integer value in the current record
   */
  int GetInt(std::string_view field_name) override;

  /**
   * @brief Return the value of the field of the current data record
   * @param field_name name of the field
   * @return the field's string value in the current record
   */
  std::string_view GetString(std::string_view field_name) override;

  /**
   * @brief Return the value of the specified field in the current record. This
   * value is expressed as a `Constant`.
   * @param field_name name of the field
   * @return value of that field, expressed as a `Constant`
   */
  Constant GetVal(std::string_view field_name) override;

  /**
   * @brief Return whether the data record has the specified field
   * @param field_name name of the field
   * @return true if the scan has that field
   */
  bool HasField(std::string_view field_name) override;

  /**
   * @brief Close the scan by closing the index and the `table_scan_`
   */
  void Close() override;

 private:
  std::unique_ptr<TableScan> table_scan_;
  std::unique_ptr<Index> index_;
  Constant value_;
};
}  // namespace simpledb
