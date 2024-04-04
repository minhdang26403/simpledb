add_library(
  simpledb_index_planner
  OBJECT
  index_join_plan.cpp
  index_select_plan.cpp
  index_update_planner.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_index_planner>
  PARENT_SCOPE)