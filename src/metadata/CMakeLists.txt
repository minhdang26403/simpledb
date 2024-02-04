add_library(
  simpledb_metadata
  OBJECT
  table_manager.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_metadata>
  PARENT_SCOPE)