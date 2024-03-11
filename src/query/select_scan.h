#pragma once

#include "query/predicate.h"
#include "query/scan.h"
#include "query/update_scan.h"

namespace simpledb {
/**
 * The scan class corresponding to the `select` relational algebra operator. All
 * methods except `Next` delegate their work to the underlying scan
 */
class SelectScan final : public UpdateScan {
 public:
  /**
   * @brief Create a select scan having the specified underlying scan and
   * predicate
   * @param scan the scan of the underlying query
   * @param predicate the selection predicate
   */
  SelectScan(std::unique_ptr<Scan> scan, Predicate& predicate);

  // Scan methods
  void BeforeFirst() override;

  bool Next() override;

  int GetInt(std::string_view field_name) override;

  std::string_view GetString(std::string_view field_name) override;

  Constant GetVal(std::string_view field_name) override;

  bool HasField(std::string_view field_name) override;

  void Close() override;

  // UpdateScan methods
  void SetInt(std::string_view field_name, int val) override;

  void SetString(std::string_view field_name, std::string_view val) override;

  void SetVal(std::string_view field_name, const Constant& val) override;

  void Delete() override;

  void Insert() override;

  RID GetRID() const override;

  void MoveToRID(const RID& rid) override;

 private:
  std::unique_ptr<Scan> scan_;
  Predicate& predicate_;
};
}  // namespace simpledb
