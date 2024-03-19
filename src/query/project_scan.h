#pragma once

#include <memory>
#include <string>

#include "query/scan.h"
#include "utils/data_type.h"

namespace simpledb {
/**
 * The scan class corresponding to the project relational algebra operator. All
 * methods except `HasField` delegate their work to the underlying scan.
 */
class ProjectScan final : public Scan {
 public:
  /**
   * @brief Create a project scan having the specified underlying scan and field
   * list
   * @param scan the underlying scan
   * @param field_list the list of field names
   */
  ProjectScan(std::unique_ptr<Scan> scan, const StringSet& field_list);

  void BeforeFirst() override;

  bool Next() override;

  int GetInt(std::string_view field_name) override;

  std::string_view GetString(std::string_view field_name) override;

  Constant GetVal(std::string_view field_name) override;

  bool HasField(std::string_view field_name) override;

  void Close() override;

 private:
  std::string ErrorMessage(std::string_view field_name) const;

  std::unique_ptr<Scan> scan_;
  StringSet field_list_;
};
}  // namespace simpledb
