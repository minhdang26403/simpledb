add_library(
  simpledb_file
  OBJECT
  block_id.cpp
  file_manager.cpp
  page.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_file>
  PARENT_SCOPE)