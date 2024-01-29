add_subdirectory(concurrency)
add_subdirectory(recovery)

add_library(
  simpledb_txn
  OBJECT
  buffer_list.cpp
  transaction.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_txn>
  PARENT_SCOPE)