#include "index/btree/btree_index.h"

#include <memory>

#include "file/block_id.h"
#include "index/btree/btree_directory.h"
#include "utils/data_type.h"

namespace simpledb {
BTreeIndex::BTreeIndex(Transaction& txn, std::string_view index_name,
                       const Layout& leaf_layout)
    : txn_(txn),
      leaf_layout_(leaf_layout),
      leaf_table_(std::string(index_name) + "leaf") {
  // deal with the leaves
  if (txn_.Size(leaf_table_) == 0) {
    auto block = txn_.Append(leaf_table_);
    BTreePage node{txn_, block, leaf_layout_};
    node.Format(block, -1);
  }

  // deal with the directory
  Schema directory_schema;
  directory_schema.Add("block", leaf_layout_.GetSchema());
  directory_schema.Add("key", leaf_layout_.GetSchema());
  dir_layout_ = Layout{directory_schema};
  auto directory_table = std::string(index_name) + "directory";
  auto root_block_ = BlockId{directory_table, 0};
  if (txn_.Size(directory_table) == 0) {
    // create new root block
    txn_.Append(directory_table);
    BTreePage node{txn_, root_block_, dir_layout_};
    node.Format(root_block_, 0);
    // insert initial directory entry
    int field_type = directory_schema.Type("key");
    auto min_key = (field_type == INTEGER) ? Constant{INT_MIN} : Constant{""};
    node.InsertDirectory(0, min_key, 0);
    node.Close();
  }
}

void BTreeIndex::BeforeFirst(const Constant& search_key) {
  Close();
  BTreeDirectory root{txn_, root_block_, dir_layout_};
  int block_num = root.Search(search_key);
  root.Close();
  BlockId leaf_block{leaf_table_, block_num};
  leaf_ =
      std::make_unique<BTreeLeaf>(txn_, leaf_block, leaf_layout_, search_key);
}

bool BTreeIndex::Next() { return leaf_->Next(); }

RID BTreeIndex::GetRID() { return leaf_->GetRID(); }

void BTreeIndex::Insert(const Constant& key, const RID& rid) {
  BeforeFirst(key);
  auto entry = leaf_->Insert(rid);
  leaf_->Close();
  if (!entry.has_value()) {
    return;
  }
  BTreeDirectory root{txn_, root_block_, dir_layout_};
  auto entry2 = root.Insert(entry.value());
  if (entry2.has_value()) {
    root.MakeNewRoot(entry2.value());
  }
  root.Close();
}

void BTreeIndex::Delete(const Constant& key, const RID& rid) {
  BeforeFirst(key);
  leaf_->Delete(rid);
  leaf_->Close();
}

void BTreeIndex::Close() {
  if (leaf_ != nullptr) {
    leaf_->Close();
    leaf_.reset(nullptr);
  }
}
}  // namespace simpledb
