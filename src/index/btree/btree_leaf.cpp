#include "index/btree/btree_leaf.h"

namespace simpledb {
void BTreeLeaf::Close() { contents_->Close(); }

bool BTreeLeaf::Next() {
  current_slot_++;
  if (current_slot_ < contents_->GetNumRecords() &&
      contents_->GetKey(current_slot_) == search_key_) {
    return true;
  }
  return TryOverflow();
}

RID BTreeLeaf::GetRID() { return contents_->GetRID(current_slot_); }

void BTreeLeaf::Delete(const RID& rid) {
  while (Next()) {
    if (GetRID() == rid) {
      contents_->Delete(current_slot_);
      return;
    }
  }
}

std::optional<DirectoryEntry> BTreeLeaf::Insert(const RID& rid) {
  if (contents_->GetFlag() >= 0 && contents_->GetKey(0) > search_key_) {
    auto first_key = contents_->GetKey(0);
    auto new_block = contents_->Split(0, contents_->GetFlag());
    current_slot_ = 0;
    contents_->SetFlag(-1);
    contents_->InsertLeaf(current_slot_, search_key_, rid);
    return DirectoryEntry{first_key, new_block.BlockNumber()};
  }

  current_slot_++;
  contents_->InsertLeaf(current_slot_, search_key_, rid);
  if (!contents_->IsFull()) {
    return {};
  }
  // else page is full, so split it
  auto first_key = contents_->GetKey(0);
  auto last_key = contents_->GetKey(contents_->GetNumRecords() - 1);
  if (last_key == first_key) {
    // create an overflow block to hold all the first record
    auto new_block = contents_->Split(1, contents_->GetFlag());
    contents_->SetFlag(new_block.BlockNumber());
    return {};
  }

  auto split_pos = contents_->GetNumRecords() / 2;
  auto split_key = contents_->GetKey(split_pos);
  if (split_key == first_key) {
    // move right, looking for the next key
    while (contents_->GetKey(split_pos) == split_key) {
      split_pos++;
    }
    split_key = contents_->GetKey(split_pos);
  } else {
    // move left, looking for first entry having that key
    while (contents_->GetKey(split_pos - 1) == split_key) {
      split_pos--;
    }
  }
  auto new_block = contents_->Split(split_pos, -1);
  return DirectoryEntry{split_key, new_block.BlockNumber()};
}

bool BTreeLeaf::TryOverflow() {
  auto first_key = contents_->GetKey(0);
  auto flag = contents_->GetFlag();
  if (search_key_ != first_key || flag < 0) {
    return false;
  }
  contents_->Close();
  BlockId next_block{filename_, flag};
  contents_.reset(new BTreePage(txn_, next_block, layout_));
  current_slot_ = 0;
  return true;
}
}  // namespace simpledb
