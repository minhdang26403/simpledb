#pragma once

#include <string_view>

#include "query/constant.h"

namespace simpledb {
/**
 * The interface will be implemented by each query scan. There is a
 * `Scan` class for each relational algebra operator.
 */
class Scan {
 public:
  /**
   * @brief Define a virtual destructor for proper cleanup 
   */
  virtual ~Scan() = default;

  /**
   * @brief Position the scan before its first record. A subsequent call to
   * `Next` will return the first record.
   */
  virtual void BeforeFirst() = 0;

  /**
   * @brief Move the scan to the next record
   * @return false if there is no next record; otherwise, true
   */
  virtual bool Next() = 0;

  /**
   * @brief Return the value of the specified integer field in the current
   * record
   * @param field_name name of the field
   * @return the field's integer value in the current record
   */
  virtual int GetInt(std::string_view field_name) = 0;

  /**
   * @brief Return the value of the specified string field in the current record
   * @param field_name name of the field
   * @return the field's string value in the current record
   */
  virtual std::string_view GetString(std::string_view field_name) = 0;

  /**
   * @brief Return the value of the specified field in the current record. This
   * value is expressed as a `Constant`.
   * @param field_name name of the field
   * @return value of that field, expressed as a `Constant`
   */
  virtual Constant GetVal(std::string_view field_name) = 0;

  /**
   * @brief Return true if the scan has the specified field
   * @param field_name name of the field
   * @return true if the scan has that field
   */
  virtual bool HasField(std::string_view field_name) = 0;

  /**
   * @brief Close the scan and its subscans, if any
   */
  virtual void Close() = 0;
};
}  // namespace simpledb
