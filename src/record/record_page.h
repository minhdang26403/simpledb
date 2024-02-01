#pragma once

#include "record/layout.h"
#include "txn/transaction.h"

namespace simpledb {
/**
 * Store a record at a given location in a block
 */
class RecordPage {
 public:
  /**
   * @brief Create a record page for a disk block with the specified layout
   * @param txn a reference to the transaction that uses this record page
   * @param block a reference to disk block
   * @param layout the layout of records stored in this record page
   */
  RecordPage(Transaction& txn, BlockId block, Layout& layout)
      : txn_(txn), block_(block), layout_(layout) {}

  /**
   * @brief Return the integer value stored for the specified field of the
   * specified slot
   * @param slot the record slot
   * @param field_name name of the field
   * @return the integer stored in that field
   */
  int GetInt(int slot, std::string_view field_name);

  /**
   * @brief Return the string value stored for the specified field of the
   * specified slot
   * @param slot the record slot
   * @param field_name name of the field
   * @return the string stored in that field
   */
  std::string_view GetString(int slot, std::string_view field_name);

  /**
   * @brief Store an integer at the specified field of the specified slot
   * @param slot the record slot to store value at
   * @param field_name name of the field
   * @param val the integer to store
   */
  void SetInt(int slot, std::string_view field_name, int val);

  /**
   * @brief Store a string at the specified field of the specified slot
   * @param slot the record slot to store value at
   * @param field_name name of the field
   * @param val the string to store
   */
  void SetString(int slot, std::string_view field_name, std::string_view val);

  /**
   * @brief Delete a record at the specified slot
   * @param slot the slot to delete
   */
  void Delete(int slot);

  /**
   * @brief Use the layout to format a new block of records. These values should
   * not be logged because these default values are meaningless.
   */
  void Format();

  /**
   * @brief Return the next used slot after the specified slot
   * @param slot the starting slot to begin searching
   * @return the next used slot
   */
  int NextAfter(int slot);

  /**
   * @brief Insert a new slot after the specified slot
   * @param slot the starting slot to begin searching
   * @return slot of the newly-inserted record
   */
  int InsertAfter(int slot);

  /**
   * @brief Return a reference to the disk block underlying this record page
   * @return a reference to the disk block
   */
  BlockId Block() const noexcept { return block_; }

 private:
  /**
   * @brief Set the flag of the specified slot
   * @param slot the slot to set its flag
   * @param flag the flag to set
   */
  void SetFlag(int slot, int flag);

  /**
   * @brief Search for a slot with the specified flag
   * @param slot the starting slot to begin searching
   * @param flag the flag to search for
   * @return the id of the slot with the specified flag
   */
  int SearchAfter(int slot, int flag);

  /**
   * @brief Return whether the specified slot is within this block
   * @param slot the slot to check
   * @return true if the slot is in the block
   */
  bool IsValidSlot(int slot) const noexcept {
    return Offset(slot + 1) <= txn_.BlockSize();
  }

  /**
   * @brief Return the offset of a slot
   * @param slot the slot to get its offset
   * @return the offset of a slot
   */
  int Offset(int slot) const noexcept { return slot * layout_.SlotSize(); }

  Transaction& txn_;
  BlockId block_;
  Layout& layout_;
  enum Flag { EMPTY, USED };
};
}  // namespace simpledb
