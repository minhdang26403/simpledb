add_library(
  simpledb_index_hash
  OBJECT
  hash_index.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_index_hash>
  PARENT_SCOPE)