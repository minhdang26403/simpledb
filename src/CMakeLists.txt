add_subdirectory(buffer)
add_subdirectory(file)
add_subdirectory(log)
add_subdirectory(server)
add_subdirectory(txn)

add_library(simpledb STATIC ${ALL_OBJECT_FILES})

set(SIMPLEDB_LIBS
  simpledb_buffer
  simpledb_file
  simpledb_log
  simpledb_server
  simpledb_txn
  simpledb_txn_recovery
)

target_link_libraries(
  simpledb
  ${SIMPLEDB_LIBS})

target_include_directories(
  simpledb PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)