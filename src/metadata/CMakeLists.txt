add_library(
  simpledb_metadata
  OBJECT
  stat_manager.cpp
  table_manager.cpp
  view_manager.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_metadata>
  PARENT_SCOPE)