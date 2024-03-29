#pragma once

#include <string_view>
#include <utility>

#include "record/schema.h"
#include "utils/data_type.h"

namespace simpledb {
/**
 * Description of the structure of a record. It contains the name, type, length
 * and offset of each field of the table.
 */
class Layout {
 public:
  /**
   * @brief Default constructor
   */
  Layout() = default;

  /**
   * @brief This constructor creates a Layout object from a schema. This
   * constructor is used when a table is created. It determines the physical
   * offset of each field within each record.
   * @param schema schema of the table's records
   */
  template <typename Sch>
    requires std::is_same_v<std::decay_t<Sch>, Schema>
  explicit Layout(Sch&& schema) : schema_(std::forward<Sch>(schema)) {
    CreateLayout();
  }

  /**
   * @brief Create a Layout object from the specified metadata. This constructor
   * is used when the metadata is retrieved from the catalog.
   * @param schema schema of the table's records
   * @param offsets the already-calculated offsets of the fields within a record
   * @param slot_size the already-calculated length of each record
   */
  Layout(const Schema& schema, StringHashMap<int>& offsets, int slot_size)
      : schema_(schema), offsets_(offsets), slot_size_(slot_size) {}

  /**
   * @brief Return the schema of the table's records
   * @return the table's record schema
   */
  Schema& GetSchema() & noexcept { return schema_; }

  // Overload for const lvalue
  const Schema& GetSchema() const & noexcept { return schema_; }

  // Overload for rvalue `Layout` object
  Schema GetSchema() && noexcept { return std::move(schema_); }

  /**
   * @brief Return the offset of a specified field within a record
   * @param field_name name of the field
   * @return offset of that field within a record
   */
  int GetOffset(std::string_view field_name) const;

  /**
   * @brief Return the size of a record (in bytes)
   * @return size of a record
   */
  int SlotSize() const noexcept { return slot_size_; }

 private:
  /**
   * @brief Return the physical length of a field
   * @param field_name name of the field
   * @return the physical length (in bytes) of a field
   */
  int LengthInBytes(std::string_view field_name) const;

  /**
   * @brief Initialize a hash map that maps each field name to its corresponding
   * offset within a record
   */
  void CreateLayout();

  Schema schema_;
  StringHashMap<int> offsets_;
  int slot_size_{};
};
}  // namespace simpledb
