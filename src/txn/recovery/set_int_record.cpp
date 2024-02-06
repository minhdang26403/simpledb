#include "txn/recovery/set_int_record.h"

#include <memory>
#include <sstream>

#include "txn/recovery/log_record.h"
#include "txn/transaction.h"

namespace simpledb {
SetIntRecord::SetIntRecord(const Page& page) {
  int txn_pos = sizeof(int);
  int file_pos = txn_pos + sizeof(int);
  int block_pos = file_pos + Page::StringLength(page.GetString(file_pos));
  int offset_pos = block_pos + sizeof(int);
  int val_pos = offset_pos + sizeof(int);

  txn_id_ = page.GetInt(txn_pos);
  block_ = BlockId{page.GetString(file_pos), page.GetInt(block_pos)};
  offset_ = page.GetInt(offset_pos);
  val_ = page.GetInt(val_pos);
}

std::string SetIntRecord::ToString() const {
  std::stringstream output;
  output << "<SETINT> " << txn_id_ << ' ' << block_.ToString() << ' ' << offset_
         << ' ' << val_ << '>';

  return output.str();
}

void SetIntRecord::Undo(Transaction& txn) {
  txn.Pin(block_);
  txn.SetInt(block_, offset_, val_, false);  // don't log the undo!
  txn.Unpin(block_);
}

int SetIntRecord::WriteToLog(LogManager& log_manager, int txn_id,
                             const BlockId& block, int offset, int val) {
  int txn_pos = sizeof(int);
  int file_pos = txn_pos + sizeof(int);
  int block_pos = file_pos + Page::StringLength(block.Filename());
  int offset_pos = block_pos + sizeof(int);
  int val_pos = offset_pos + sizeof(int);

  size_t record_size = val_pos + sizeof(int);
  auto record = std::make_unique<char[]>(record_size);
  Page page{record.get(), record_size};
  page.SetInt(0, static_cast<int>(LogType::SETINT));
  page.SetInt(txn_pos, txn_id);
  page.SetString(file_pos, block.Filename());
  page.SetInt(block_pos, block.BlockNumber());
  page.SetInt(offset_pos, offset);
  page.SetInt(val_pos, val);

  return log_manager.Append(std::span{record.get(), record_size});
}
}  // namespace simpledb
