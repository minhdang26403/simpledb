add_library(
  simpledb_txn_concurrency
  OBJECT
  concurrency_manager.cpp
  lock_table.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_txn_concurrency>
  PARENT_SCOPE)