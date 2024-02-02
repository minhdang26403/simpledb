add_library(
  simpledb_record
  OBJECT
  layout.cpp
  record_page.cpp
  schema.cpp
  table_scan.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_record>
  PARENT_SCOPE)