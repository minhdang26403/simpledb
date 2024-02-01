#include "record/record_page.h"

namespace simpledb {

int RecordPage::GetInt(int slot, std::string_view field_name) {
  int field_pos = Offset(slot) + layout_.GetOffset(field_name);
  return txn_.GetInt(block_, field_pos);
}

std::string_view RecordPage::GetString(int slot, std::string_view field_name) {
  int field_pos = Offset(slot) + layout_.GetOffset(field_name);
  return txn_.GetString(block_, field_pos);
}

void RecordPage::SetInt(int slot, std::string_view field_name, int val) {
  int field_pos = Offset(slot) + layout_.GetOffset(field_name);
  txn_.SetInt(block_, field_pos, val, true);
}

void RecordPage::SetString(int slot, std::string_view field_name,
                           std::string_view val) {
  int field_pos = Offset(slot) + layout_.GetOffset(field_name);
  txn_.SetString(block_, field_pos, val, true);
}

void RecordPage::Delete(int slot) { SetFlag(slot, EMPTY); }

void RecordPage::Format() {
  int slot = 0;
  while (IsValidSlot(slot)) {
    txn_.SetInt(block_, Offset(slot), EMPTY, false);
    auto& schema = layout_.GetSchema();
    for (const auto& field_name : schema.Fields()) {
      int field_pos = Offset(slot) + layout_.GetOffset(field_name);
      if (schema.Type(field_name) == INTEGER) {
        txn_.SetInt(block_, field_pos, 0, false);
      } else {  // VARCHAR
        txn_.SetString(block_, field_pos, "", false);
      }
    }
    slot++;
  }
}

int RecordPage::NextAfter(int slot) { return SearchAfter(slot, USED); }

int RecordPage::InsertAfter(int slot) {
  int new_slot = SearchAfter(slot, EMPTY);
  if (new_slot >= 0) {
    SetFlag(new_slot, USED);
  }
  return new_slot;
}

void RecordPage::SetFlag(int slot, int flag) {
  txn_.SetInt(block_, Offset(slot), flag, true);
}

int RecordPage::SearchAfter(int slot, int flag) {
  slot++;
  while (IsValidSlot(slot)) {
    if (txn_.GetInt(block_, Offset(slot)) == flag) {
      return slot;
    }
    slot++;
  }
  return -1;
}
}  // namespace simpledb
