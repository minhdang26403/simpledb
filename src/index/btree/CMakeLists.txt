add_library(
  simpledb_index_btree
  OBJECT
  btree_directory.cpp
  btree_index.cpp
  btree_leaf.cpp
  btree_page.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_index_btree>
  PARENT_SCOPE)