#include "index/btree/btree_directory.h"

namespace simpledb {
void BTreeDirectory::Close() { contents_->Close(); }

int BTreeDirectory::Search(const Constant& search_key) {
  auto child_block = FindChildBlock(search_key);
  while (contents_->GetFlag() > 0) {
    contents_->Close();
    contents_.reset(new BTreePage(txn_, child_block, layout_));
    child_block = FindChildBlock(search_key);
  }

  return child_block.BlockNumber();
}

void BTreeDirectory::MakeNewRoot(const DirectoryEntry& entry) {
  auto first_key = contents_->GetKey(0);
  auto level = contents_->GetFlag();
  auto new_block =
      contents_->Split(0, level);  // i.e., transfer all the records
  DirectoryEntry old_root{first_key, new_block.BlockNumber()};
  InsertEntry(old_root);
  InsertEntry(entry);
  contents_->SetFlag(level + 1);
}

std::optional<DirectoryEntry> BTreeDirectory::Insert(
    const DirectoryEntry& entry) {
  if (contents_->GetFlag() == 0) {
    return InsertEntry(entry);
  }

  auto child_block = FindChildBlock(entry.Key());
  BTreeDirectory child{txn_, child_block, layout_};
  auto my_entry = child.Insert(entry);
  child.Close();
  return my_entry;
}

std::optional<DirectoryEntry> BTreeDirectory::InsertEntry(
    const DirectoryEntry& entry) {
  auto new_slot = 1 + contents_->FindSlotBefore(entry.Key());
  contents_->InsertDirectory(new_slot, entry.Key(), entry.BlockNumber());
  if (!contents_->IsFull()) {
    return {};
  }
  // else page is full, so split it
  auto level = contents_->GetFlag();
  auto split_pos = contents_->GetNumRecords() / 2;
  auto split_key = contents_->GetKey(split_pos);
  auto new_block = contents_->Split(split_pos, level);
  return DirectoryEntry{split_key, new_block.BlockNumber()};
}

BlockId BTreeDirectory::FindChildBlock(const Constant& search_key) {
  auto slot = contents_->FindSlotBefore(search_key);
  if (contents_->GetKey(slot + 1) == search_key) {
    slot++;
  }
  auto block_num = contents_->GetChildNum(slot);
  return BlockId{filename_, block_num};
}
}  // namespace simpledb
