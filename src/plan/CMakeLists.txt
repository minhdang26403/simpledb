add_library(
  simpledb_plan
  OBJECT
  basic_query_planner.cpp
  better_query_planner.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_plan>
  PARENT_SCOPE)