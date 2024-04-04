#include "index/btree/btree_page.h"

namespace simpledb {
int BTreePage::FindSlotBefore(const Constant& search_key) {
  int slot = 0;
  while (slot < GetNumRecords() && GetKey(slot) < search_key) {
    slot++;
  }
  return slot - 1;
}

void BTreePage::Close() {
  if (current_block_.has_value()) {
    txn_.Unpin(current_block_.value());
  }
  current_block_.reset();
}

BlockId BTreePage::Split(int split_pos, int flag) {
  auto new_block = AppendNew(flag);
  BTreePage new_page{txn_, new_block, layout_};
  TransferRecords(split_pos, new_page);
  new_page.SetFlag(flag);
  new_page.Close();
  return new_block;
}

BlockId BTreePage::AppendNew(int flag) {
  auto block = txn_.Append(current_block_.value().Filename());
  txn_.Pin(block);
  Format(block, flag);
  return block;
}

void BTreePage::Format(const BlockId& block, int flag) {
  txn_.SetInt(block, 0, flag, false);
  txn_.SetInt(block, sizeof(int), 0, false);  // #records = 0
  auto record_size = layout_.SlotSize();
  for (int pos = 2 * sizeof(int); pos + record_size <= txn_.BlockSize();
       pos += record_size) {
    MakeDefaultRecord(block, pos);
  }
}

void BTreePage::MakeDefaultRecord(const BlockId& block, int pos) {
  for (const auto& field_name : layout_.GetSchema().Fields()) {
    auto offset = layout_.GetOffset(field_name);
    if (layout_.GetSchema().Type(field_name) == INTEGER) {
      txn_.SetInt(block, pos + offset, 0, false);
    } else {
      txn_.SetString(block, pos + offset, "", false);
    }
  }
}

// Methods called only by BTreeDir
void BTreePage::InsertDirectory(int slot, const Constant& key, int block_num) {
  Insert(slot);
  SetVal(slot, "key", key);
  SetInt(slot, "block", block_num);
}

// Methods called only be BTreeLeaf
void BTreePage::InsertLeaf(int slot, const Constant& key, const RID& rid) {
  Insert(slot);
  SetVal(slot, "key", key);
  SetInt(slot, "block", rid.BlockNumber());
  SetInt(slot, "id", rid.Slot());
}

void BTreePage::Delete(int slot) {
  for (int i = slot + 1; i < GetNumRecords(); i++) {
    CopyRecord(i, i - 1);
  }
  SetNumRecords(GetNumRecords() - 1);
}

// Private methods
int BTreePage::GetInt(int slot, std::string_view field_name) {
  auto pos = FieldPosition(slot, field_name);
  return txn_.GetInt(current_block_.value(), pos);
}

std::string_view BTreePage::GetString(int slot, std::string_view field_name) {
  auto pos = FieldPosition(slot, field_name);
  return txn_.GetString(current_block_.value(), pos);
}

Constant BTreePage::GetVal(int slot, std::string_view field_name) {
  auto type = layout_.GetSchema().Type(field_name);
  if (type == INTEGER) {
    return Constant{GetInt(slot, field_name)};
  } else {
    return Constant{GetString(slot, field_name)};
  }
}

void BTreePage::SetInt(int slot, std::string_view field_name, int val) {
  auto pos = FieldPosition(slot, field_name);
  txn_.SetInt(current_block_.value(), pos, val, true);
}

void BTreePage::SetString(int slot, std::string_view field_name,
                          std::string_view val) {
  auto pos = FieldPosition(slot, field_name);
  txn_.SetString(current_block_.value(), pos, val, true);
}

void BTreePage::SetVal(int slot, std::string_view field_name,
                       const Constant& val) {
  auto type = layout_.GetSchema().Type(field_name);
  if (type == INTEGER) {
    SetInt(slot, field_name, val.AsInt());
  } else {
    SetString(slot, field_name, val.AsString());
  }
}

void BTreePage::Insert(int slot) {
  for (int i = GetNumRecords(); i > slot; i--) {
    CopyRecord(i - 1, i);
  }
  SetNumRecords(GetNumRecords() + 1);
}

void BTreePage::CopyRecord(int from, int to) {
  for (const auto& field_name : layout_.GetSchema().Fields()) {
    SetVal(to, field_name, GetVal(from, field_name));
  }
}

void BTreePage::TransferRecords(int slot, BTreePage& dest) {
  int dest_slot = 0;
  while (slot < GetNumRecords()) {
    dest.Insert(dest_slot);
    for (const auto& field_name : layout_.GetSchema().Fields()) {
      dest.SetVal(dest_slot, field_name, GetVal(slot, field_name));
    }
    Delete(slot);
    dest_slot++;
  }
}
}  // namespace simpledb
