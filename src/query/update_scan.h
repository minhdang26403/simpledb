#pragma once

#include <string_view>

#include "query/constant.h"
#include "query/scan.h"
#include "record/rid.h"

namespace simpledb {
/**
 * The interface implemented by all updatable scans
 */
class UpdateScan : public Scan {
 public:
  /**
   * @brief Define a virtual destructor for proper cleanup
   */
  virtual ~UpdateScan() = default;

  /**
   * @brief Modify the field value of the current record
   * @param field_name name of the field
   * @param val the new integer value
   */
  virtual void SetInt(std::string_view field_name, int val) = 0;

  /**
   * @brief Modify the field value of the current record
   * @param field_name name of the field
   * @param val the new string value
   */
  virtual void SetString(std::string_view field_name, std::string_view val) = 0;

  /**
   * @brief Modify the field value of the current record
   * @param field_name name of the field
   * @param val the new value, expressed as a `Constant`
   */
  virtual void SetVal(std::string_view field_name, const Constant& val) = 0;

  /**
   * @brief Insert a new record somewhere in the scan
   */
  virtual void Insert() = 0;

  /**
   * @brief Delete the current record from the scan
   */
  virtual void Delete() = 0;

  /**
   * @brief Position the scan so that the current record has the specified id
   * @param rid id of the desired record
   */
  virtual void MoveToRID(const RID& rid) = 0;

  /**
   * @brief Return the id of the current record
   * @return id of the current record
   */
  virtual RID GetRID() const = 0;
};
}  // namespace simpledb
