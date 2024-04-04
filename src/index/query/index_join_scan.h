#pragma once

#include <memory>
#include <string>
#include <utility>

#include "index/index.h"
#include "query/scan.h"
#include "record/table_scan.h"

namespace simpledb {
/**
 * The scan class corresponding to the `index join` relational algebra operator.
 * The code is very similar to that of `ProductScan`, which makes sense because
 * an index join is essentially the product of each LHS record with the matching
 * RHS index records.
 */
class IndexJoinScan : public Scan {
 public:
  /**
   * @brief Create an index join scan for the specified LHS can and RHS index
   * @param lhs the LHS scan
   * @param index the RHS index
   * @param join_field the LHS field used for joining
   * @param rhs the RHS scan
   */
  IndexJoinScan(std::unique_ptr<Scan> lhs, std::unique_ptr<Index> index,
                std::string_view join_field, std::unique_ptr<TableScan> rhs)
      : lhs_(std::move(lhs)),
        index_(std::move(index)),
        join_field_(join_field),
        rhs_(std::move(rhs)) {
    BeforeFirst();
  }

  /**
   * @brief Position the scan before the first record. That is, the LHS scan
   * will be positioned at its first record, and the index will be positioned
   * before the first record for the join value.
   */
  void BeforeFirst() override;

  /**
   * @brief Move the scan to the next record. The method moves to the next index
   * record, if possible. Otherwise, it moves to the next LHS record and the
   * first index record.
   * @return false if there are no more LHS records
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
  /**
   * @brief Move to the first index record
   */
  void ResetIndex();

  std::unique_ptr<Scan> lhs_;
  std::unique_ptr<Index> index_;
  std::string join_field_;
  std::unique_ptr<TableScan> rhs_;
};
}  // namespace simpledb
