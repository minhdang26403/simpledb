#pragma once

#include <string>
#include <utility>
#include <vector>

#include "utils/data_type.h"

namespace simpledb {
/**
 * @brief The record schema of a table. A schema contains the name and type of
 * each field of the table as well as the length of each varchar field.
 */
class Schema {
 private:
  struct FieldInfo {
    int type_{};
    int length_{};

    FieldInfo(int type, int length) : type_(type), length_(length) {}
  };

 public:
  /**
   * @brief Add a field to the schema having a specified name, type, and length.
   * If the field type is `integer`, then the length value is irrelevant.
   * @param field_name name of the field
   * @param type type of the field
   * @param length conceptual length of a string field
   */
  void AddField(std::string_view field_name, int type, int length);

  /**
   * @brief Add an integer field to the schema
   * @param field_name name of the field
   */
  void AddIntField(std::string_view field_name);

  /**
   * @brief Add a string field to the schema. The length is the conceptual
   * length of the field. For example, if the field is defined as varchar(8),
   * then its length is 8.
   * @param field_name name of the field
   * @param length number of chars in the varchar definition
   */
  void AddStringField(std::string_view field_name, int length);

  /**
   * @brief Add a field to the schema having the same type and length as the
   * corresponding field in another schema
   * @param field_name name of the field
   * @param schema the other schema
   */
  void Add(std::string_view field_name, const Schema& schema);

  /**
   * @brief Add all of the fields in the specified schema to the current schema
   * @param schema the other schema
   */
  void AddAll(const Schema& schema);

  /**
   * @brief Return a collection containing the name of each field in the schema
   * @return the collection of the schema's field names
   */
  const auto& Fields() const & noexcept { return fields_; }

  // Overload for rvalue
  auto Fields() && noexcept { return std::move(fields_); }

  /**
   * @brief Return true if the specified field is in the schema
   * @param field_name name of the field
   * @return true if the field is in the schema; otherwise, false
   */
  bool HasField(std::string_view field_name) const noexcept;

  /**
   * @brief Return the type of the specified field
   * @param field_name name of the field
   * @return the type of the field
   */
  int Type(std::string_view field_name) const;

  /**
   * @brief Return the conceptual length of the specified field. If the field is
   * not a string field, then the return value is undefined.
   * @param field_name name of the field
   * @return the conceptual length of the field
   */
  int Length(std::string_view field_name) const;

 private:
  /**
   * @brief Get the FieldInfo struct (type, length) of the specified field name
   * @param field_name name of the field
   * @return a constant reference to the FieldInfo struct
   */
  const FieldInfo& GetFieldInfo(std::string_view field_name) const;

  std::vector<std::string> fields_;
  StringHashMap<FieldInfo> info_;
};
}  // namespace simpledb
