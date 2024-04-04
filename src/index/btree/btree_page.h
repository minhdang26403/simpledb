#pragma once

#include <optional>

#include "file/block_id.h"
#include "query/constant.h"
#include "record/layout.h"
#include "record/rid.h"
#include "txn/transaction.h"
#include "utils/data_type.h"

namespace simpledb {
/**
 * B-tree directory and leaf pages have many commonalities: in particular, their
 * records are stored in sorted order, and pages split when full.
 * A `BTreePage` object contains this common functionality.
 */
class BTreePage {
 public:
  BTreePage(Transaction& txn, const BlockId& current_block, Layout& layout)
      : txn_(txn), current_block_(current_block), layout_(layout) {
    txn_.Pin(current_block_.value());
  }

  /**
   * @brief Calculate the position where the first record having the specified
   * search key should be, then return the position before it
   * @param search_key the search key
   * @return the position where the search key goes
   */
  int FindSlotBefore(const Constant& search_key);

  /**
   * @brief Close the page by unpinning its buffer
   */
  void Close();

  /**
   * @brief Return true if the block is full
   * @return true if the block is full
   */
  bool IsFull() {
    return SlotPosition(GetNumRecords() + 1) >= txn_.BlockSize();
  }

  /**
   * @brief Split the page at the specified position. A new page is created, and
   * the records of the page starting at the split position are transferred to
   * the new page.
   * @param split_pos the split position
   * @param flag the initial value of the flag field
   * @return the reference to the new block
   */
  BlockId Split(int split_pos, int flag);

  /**
   * @brief Return the key of the record at the specified slot
   * @param slot the integer slot of an index record
   * @return the key of the record at that slot
   */
  Constant GetKey(int slot) { return GetVal(slot, "key"); }

  /**
   * @brief Return the value of the page's flag field
   * @return the value of the page's flag field
   */
  int GetFlag() { return txn_.GetInt(current_block_.value(), 0); }

  /**
   * @brief Set the page's flag field to the specified value
   * @param val the new value of the page flag
   */
  void SetFlag(int val) { txn_.SetInt(current_block_.value(), 0, val, true); }

  /**
   * @brief Append a new block to the end of the specified B-tree file, having
   * the specified flag value
   * @param flag the initial value of the flag
   * @return a reference to the newly-created block
   */
  BlockId AppendNew(int flag);

  /**
   * @brief Zero-initialize a block
   * @param block the block to initialize
   * @param flag the flag to initialize with
   */
  void Format(const BlockId& block, int flag);

  /**
   * @brief Create a record with default values at the given position of a block
   * @param block the block where the record is stored
   * @param pos the position of the record in the block
   */
  void MakeDefaultRecord(const BlockId& block, int pos);

  // Methods called only by BTreeDir
  /**
   * @brief Return the block number stored in the index record at the specified
   * slot
   * @param slot the slot of an index record
   * @return the block number stored in that record
   */
  int GetChildNum(int slot) { return GetInt(slot, "block"); }

  /**
   * @brief Insert a directory entry at the specified slot
   * @param slot the slot of an index record
   * @param key the key to be stored
   * @param block_num the block number to be stored
   */
  void InsertDirectory(int slot, const Constant& key, int block_num);

  // Methods called only be BTreeLeaf
  /**
   * @brief Return the RID value stored in the specified leaf index record
   * @param slot the slot of the desired index record
   * @return the RID value stored at that slot
   */
  RID GetRID(int slot) {
    return RID{GetInt(slot, "block"), GetInt(slot, "id")};
  }

  /**
   * @brief Insert a leaf index record at the specified slot
   * @param slot the slot of the desired index record
   * @param key the new key
   * @param rid the new RID
   */
  void InsertLeaf(int slot, const Constant& key, const RID& rid);

  /**
   * @brief Delete the index record at the specified slot
   * @param slot the slot of the deleted index record
   */
  void Delete(int slot);

  /**
   * @brief Return the number of index records in this page
   * @return the number of index records in this page
   */
  int GetNumRecords() {
    return txn_.GetInt(current_block_.value(), sizeof(int));
  }

 private:
  /**
   * @brief Get the integer value of the `field_name` of the record at the
   * specified slot
   * @param slot the slot of the record
   * @param field_name the name of the field within the record
   * @return the integer value of the field
   */
  int GetInt(int slot, std::string_view field_name);

  /**
   * @brief Get the string value of the `field_name` of the record at the
   * specified slot
   * @param slot the slot of the record
   * @param field_name the name of the field within the record
   * @return the string value of the field
   */
  std::string_view GetString(int slot, std::string_view field_name);

  /**
   * @brief Get the generic `Constant` value of the `field_name` of the record
   * at the specified slot
   * @param slot the slot of the record
   * @param field_name the name of the field within the record
   * @return the generic `Constant` value of the field
   */
  Constant GetVal(int slot, std::string_view field_name);

  /**
   * @brief Set the integer value of the `field_name` of the record at the
   * specified slot
   * @param slot the slot of the record
   * @param field_name the name of the field within the record
   * @param val the integer value to set to
   */
  void SetInt(int slot, std::string_view field_name, int val);

  /**
   * @brief Set the string value of the `field_name` of the record at the
   * specified slot
   * @param slot the slot of the record
   * @param field_name the name of the field within the record
   * @param val the string value to set to
   */
  void SetString(int slot, std::string_view field_name, std::string_view val);

  /**
   * @brief Set the `Constant` value of the `field_name` of the record at the
   * specified slot
   * @param slot the slot of the record
   * @param field_name the name of the field within the record
   * @param val the `Constant` value to set to
   */
  void SetVal(int slot, std::string_view field_name, const Constant& val);

  /**
   * @brief Set the number of records in a page
   * @param num_records the number of records
   */
  void SetNumRecords(int num_records) {
    txn_.SetInt(current_block_.value(), sizeof(int), num_records, true);
  }

  /**
   * @brief Create an empty slot at the specified position to insert a new
   * record into the page
   * @param slot the slot to insert a new record into
   */
  void Insert(int slot);

  /**
   * @brief Copy a record from one slot to another slot
   * @param from the source slot
   * @param to the destination slot
   */
  void CopyRecord(int from, int to);

  /**
   * @brief Transfer all records starting from `slot` until the end of that page
   * to a new page
   * @param slot the position to start transferring records
   * @param dest the destination page
   */
  void TransferRecords(int slot, BTreePage& dest);

  /**
   * @brief Calculate the physical position of the field within the specified
   * slot in this page
   * @param slot the logical slot number in the page
   * @param field_name the name of the field
   * @return the position of the field
   */
  int FieldPosition(int slot, std::string_view field_name) const {
    return SlotPosition(slot) + layout_.GetOffset(field_name);
  }

  /**
   * @brief Calculate the physical position of the specified slot in this page
   * @param slot the logical slot number in the page
   * @return the position of the specified slot
   */
  int SlotPosition(int slot) const noexcept {
    auto slot_size = layout_.SlotSize();
    return sizeof(int) + sizeof(int) + slot * slot_size;
  }

  Transaction& txn_;
  std::optional<BlockId> current_block_;
  Layout& layout_;
};
}  // namespace simpledb
