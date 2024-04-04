add_library(
  simpledb_index_query
  OBJECT
  index_join_scan.cpp
  index_select_scan.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_index_query>
  PARENT_SCOPE)