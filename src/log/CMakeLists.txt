add_library(
  simpledb_log
  OBJECT
  log_iterator.cpp
  log_manager.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_log>
  PARENT_SCOPE)