#include <optional>
#include <string>

#include "query/constant.h"
#include "record/layout.h"
#include "record/record_page.h"
#include "record/rid.h"
#include "txn/transaction.h"
#include "utils/data_type.h"

namespace simpledb {
/**
 * Provide the abstraction of an arbitrarily large array of records
 */
class TableScan {
 public:
  /**
   * @brief Construct a new `TableScan` object that keeps track of a current
   * record
   * @param txn the transaction that accesses the table
   * @param table_name name of the table
   * @param layout layout of a record in this table
   */
  TableScan(Transaction& txn, std::string_view table_name, Layout& layout);

  // Methods that implement Scan

  /**
   * @brief Position the scan before its first record. A subsequent call to
   * `Next()` will return the first record.
   */
  void BeforeFirst();

  /**
   * @brief Move the scan to the next record
   * @return true if there is a next record; otherwise, false
   */
  bool Next();

  /**
   * @brief Return the value of the specified integer field in the current
   * record
   * @param field_name name of the field
   * @return the field's integer value in the current record
   */
  int GetInt(std::string_view field_name);

  /**
   * @brief Return the value of the specified string field in the current record
   * @param field_name name of the field
   * @return the field's string value in the current record
   */
  std::string_view GetString(std::string_view field_name);

  /**
   * @brief Return the value of the specified field in the current record. The
   * value is expressed as a `Constant`.
   * @param field_name name of the field
   * @return the value of that field, expressed as a `Constant`
   */
  Constant GetVal(std::string_view field_name);

  /**
   * @brief Return whether the table has the specified field
   * @param field_name name of the field
   * @return true if the table has that field; otherwise, false
   */
  bool HasField(std::string_view field_name) noexcept;

  /**
   * @brief Close the scan
   */
  void Close();

  // Methods that implement UpdateScan

  /**
   * @brief Modify the field value of the current record
   * @param field_name name of the field
   * @param val the new integer value
   */
  void SetInt(std::string_view field_name, int val);

  /**
   * @brief Modify the field value of the current record
   * @param field_name name of the field
   * @param val the new string value
   */
  void SetString(std::string_view field_name, std::string_view val);

  /**
   * @brief Modify the field value of the current record
   * @param field_name name of the field
   * @param val the new value, expressed as a `Constant`
   */
  void SetVal(std::string_view field_name, const Constant& val);

  /**
   * @brief Insert a new record somewhere in the table. This method moves the
   * scan to the placeholder record. Transaction must call `SetInt`,
   * `SetString`, or `SetVal` to actually insert the record.
   */
  void Insert();

  /**
   * @brief Delete the current record from the table
   */
  void Delete();

  /**
   * @brief Position the scan so that the current record has the specified id
   * @param rid id of the desired record
   */
  void MoveToRid(const RID& rid);

  /**
   * @brief Return the id of the current record
   * @return id of the current record
   */
  RID GetRID() const noexcept {
    return RID{record_page_.value().Block().BlockNumber(), current_slot_};
  }

 private:
  /**
   * @brief Move the scan to the specified disk block
   * @param block_num the disk block to move to
   */
  void MoveToBlock(int block_num);

  /**
   * @brief Append a new disk block to the file holding this table and move the
   * scan to the new block
   */
  void MoveToNewBlock();

  /**
   * @brief Return whether the scan is in the last block of the file
   * @return true if the scan is in the last block; otherwise, false
   */
  bool AtLastBlock() {
    return record_page_.value().Block().BlockNumber() ==
           txn_.Size(filename_) - 1;
  }

  Transaction& txn_;
  Layout& layout_;
  std::optional<RecordPage> record_page_;
  std::string filename_;
  int current_slot_{-1};
};
}  // namespace simpledb
