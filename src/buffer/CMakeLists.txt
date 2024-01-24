add_library(
  simpledb_buffer
  OBJECT
  buffer.cpp
  buffer_manager.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_buffer>
  PARENT_SCOPE)