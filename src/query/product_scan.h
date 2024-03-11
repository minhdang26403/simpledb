#pragma once

#include <memory>
#include <string>

#include "query/scan.h"

namespace simpledb {
class ProductScan final : public Scan {
 public:
  /**
   * @brief Create a product scan having the two underlying scans
   * @param scan1 the left-hand side scan
   * @param scan2 the right-hand side scan
   */
  ProductScan(std::unique_ptr<Scan> scan1, std::unique_ptr<Scan> scan2);

  /**
   * @brief Position the scan before its first record. In particular, the LHS
   * scan is positioned at its first record, and the RHS scan is positioned
   * before its first record.
   */
  void BeforeFirst() override;

  /**
   * @brief Move the scan to the next record. The method moves to the next RHS
   * record, if possible. Otherwise, it moves to the next LHS record and the
   * first RHS record.
   * @return false if there is no next record; otherwise, true
   */
  bool Next() override;

  /**
   * @brief Return the integer value of the specified field. The value is
   * obtained from whichever scan contains the field.
   * @param field_name name of the field
   * @return the field's integer value in the current record
   */
  int GetInt(std::string_view field_name) override;

  /**
   * @brief Return the string value of the specified field. The value is
   * obtained from whichever scan contains the field.
   * @param field_name name of the field
   * @return the field's string value in the current record
   */
  std::string_view GetString(std::string_view field_name) override;

  /**
   * @brief Return the string value of the specified field. The value is
   * obtained from whichever scan contains the field.
   * @param field_name name of the field
   * @return value of that field, expressed as a `Constant`
   */
  Constant GetVal(std::string_view field_name) override;

  /**
   * @brief Return true if the specified field is in either of the underlying
   * scans
   * @param field_name name of the field
   * @return true if the underlying scans contain the field; otherwise, false
   */
  bool HasField(std::string_view field_name) override;

  /**
   * @brief Close both underlying scans
   */
  void Close() override;

 private:
  std::unique_ptr<Scan> scan1_;
  std::unique_ptr<Scan> scan2_;
};
}  // namespace simpledb
