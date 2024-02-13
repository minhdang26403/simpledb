add_library(
  simpledb_query
  OBJECT
  predicate.cpp
  product_scan.cpp
  project_scan.cpp
  select_scan.cpp
  term.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_query>
  PARENT_SCOPE)