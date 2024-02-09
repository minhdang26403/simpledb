#include "metadata/index_info.h"

#include <utility>

namespace simpledb {
IndexInfo::IndexInfo(std::string_view index_name, std::string_view field_name,
                     Schema& table_schema, Transaction& txn,
                     StatInfo& stat_info)
    : index_name_(index_name),
      field_name_(field_name),
      txn_(txn),
      table_schema_(table_schema),
      stat_info_(stat_info) {
  index_layout_ = CreateIndexLayout();
}

int IndexInfo::BlocksAccessed() const noexcept {
  int records_per_block = txn_.BlockSize() / index_layout_.SlotSize();
  int num_blocks = stat_info_.RecordsOutput() / records_per_block;
  return HashIndex::SearchCost(num_blocks, records_per_block);
}

int IndexInfo::RecordsOutput() const {
  return stat_info_.RecordsOutput() / stat_info_.DistinctValues(field_name_);
}

int IndexInfo::DistinctValues(std::string_view field_name) const {
  return (field_name_ == field_name) ? 1
                                     : stat_info_.DistinctValues(field_name_);
}

Layout IndexInfo::CreateIndexLayout() {
  Schema schema;
  schema.AddIntField("block");
  schema.AddIntField("id");
  if (table_schema_.Type(field_name_) == INTEGER) {
    schema.AddIntField("data_val");
  } else {
    int field_len = table_schema_.Length(field_name_);
    schema.AddStringField("data_val", field_len);
  }
  return Layout{std::move(schema)};
}
}  // namespace simpledb
