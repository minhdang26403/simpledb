#include "record/table_scan.h"

namespace simpledb {
TableScan::TableScan(Transaction& txn, std::string_view table_name,
                     Layout& layout)
    : txn_(txn), layout_(layout), filename_(std::string(table_name) + ".tbl") {
  if (txn_.Size(filename_) == 0) {
    MoveToNewBlock();
  } else {
    MoveToBlock(0);
  }
}

void TableScan::BeforeFirst() { MoveToBlock(0); }

bool TableScan::Next() {
  current_slot_ = record_page_.value().NextAfter(current_slot_);
  while (current_slot_ < 0) {
    if (AtLastBlock()) {
      return false;
    }
    MoveToBlock(record_page_.value().Block().BlockNumber() + 1);
    current_slot_ = record_page_.value().NextAfter(current_slot_);
  }
  return true;
}

int TableScan::GetInt(std::string_view field_name) {
  return record_page_.value().GetInt(current_slot_, field_name);
}

std::string_view TableScan::GetString(std::string_view field_name) {
  return record_page_.value().GetString(current_slot_, field_name);
}

Constant TableScan::GetVal(std::string_view field_name) {
  if (layout_.GetSchema().Type(field_name) == INTEGER) {
    return Constant{GetInt(field_name)};
  } else {
    return Constant{GetString(field_name)};
  }
}

bool TableScan::HasField(std::string_view field_name) noexcept {
  return layout_.GetSchema().HasField(field_name);
}

void TableScan::Close() {
  if (record_page_.has_value()) {
    txn_.Unpin(record_page_.value().Block());
  }
}

void TableScan::SetInt(std::string_view field_name, int val) {
  record_page_.value().SetInt(current_slot_, field_name, val);
}

void TableScan::SetString(std::string_view field_name, std::string_view val) {
  record_page_.value().SetString(current_slot_, field_name, val);
}

void TableScan::SetVal(std::string_view field_name, const Constant& val) {
  if (layout_.GetSchema().Type(field_name) == INTEGER) {
    SetInt(field_name, val.AsInt());
  } else {
    SetString(field_name, val.AsString());
  }
}

void TableScan::Insert() {
  current_slot_ = record_page_.value().InsertAfter(current_slot_);
  while (current_slot_ < 0) {
    if (AtLastBlock()) {
      MoveToNewBlock();
    } else {
      MoveToBlock(record_page_.value().Block().BlockNumber() + 1);
    }
    current_slot_ = record_page_.value().InsertAfter(current_slot_);
  }
}

void TableScan::Delete() { record_page_.value().Delete(current_slot_); }

void TableScan::MoveToRid(const RID& rid) {
  Close();
  BlockId block{filename_, rid.BlockNumber()};
  record_page_.emplace(txn_, block, layout_);
  current_slot_ = rid.Slot();
}

void TableScan::MoveToBlock(int block_num) {
  Close();
  BlockId block{filename_, block_num};
  record_page_.emplace(txn_, block, layout_);
  current_slot_ = -1;
}

void TableScan::MoveToNewBlock() {
  Close();
  BlockId block = txn_.Append(filename_);
  record_page_.emplace(txn_, block, layout_);
  record_page_.value().Format();
  current_slot_ = -1;
}
}  // namespace simpledb
