add_library(
  simpledb_txn_recovery
  OBJECT
  checkpoint_record.cpp
  commit_record.cpp
  log_record.cpp
  recovery_manager.cpp
  rollback_record.cpp
  set_int_record.cpp
  set_string_record.cpp
  start_record.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_txn_recovery>
  PARENT_SCOPE)