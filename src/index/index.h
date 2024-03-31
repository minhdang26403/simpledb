#pragma once

#include "query/constant.h"
#include "record/rid.h"

namespace simpledb {
class Index {
 public:
  virtual ~Index() = default;

  /**
   * @brief Position the index before the first record having the specified
   * search key
   * @param search_key the search key value
   */
  virtual void BeforeFirst(const Constant& search_key) = 0;

  /**
   * @brief Move the index to the next record having the search key specified in
   * the `BeforeFirst` method
   * @return false if there are no more such index records
   */
  virtual bool Next() = 0;

  /**
   * @brief Return the RID stored in the current index record
   * @return the RID stored in the current index record
   */
  virtual RID GetDataRID() = 0;

  /**
   * @brief Insert an index record having the specified data (search key) and
   * RID values
   * @param data_val the data value in the new index record
   * @param data_rid the RID in the new index record
   */
  virtual void Insert(const Constant& data_val, const RID& data_rid) = 0;

  /**
   * @brief Delete the index record having the specified data (search key) and
   * RID values
   * @param data_val the data value of the index record to delete
   * @param data_rid the RID of the index record to delete
   */
  virtual void Delete(const Constant& data_val, const RID& data_rid) = 0;

  /**
   * @brief Close the index
   */
  virtual void Close() = 0;
};
}  // namespace simpledb
