add_library(
  simpledb_server
  OBJECT
  simpledb.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_server>
  PARENT_SCOPE)