#pragma once

#include "query/constant.h"
#include "record/rid.h"

namespace simpledb {
/**
 * This interface contains methods to traverse an index
 */
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
  virtual RID GetRID() = 0;

  /**
   * @brief Insert an index record having the specified key and RID values
   * @param key the key of the new index record
   * @param rid the RID of the new index record
   */
  virtual void Insert(const Constant& key, const RID& rid) = 0;

  /**
   * @brief Delete the index record having the specified key and RID values
   * @param key the key of the deleted index record
   * @param rid the RID of the deleted index record
   */
  virtual void Delete(const Constant& key, const RID& rid) = 0;

  /**
   * @brief Close the index
   */
  virtual void Close() = 0;
};
}  // namespace simpledb
