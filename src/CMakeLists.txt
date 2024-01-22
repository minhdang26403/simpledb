add_subdirectory(file)
add_subdirectory(server)

add_library(simpledb STATIC ${ALL_OBJECT_FILES})

set(SIMPLEDB_LIBS
  simpledb_file
  simpledb_server
)

target_link_libraries(
  simpledb
  ${SIMPLEDB_LIBS})

target_include_directories(
  simpledb PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)