set(
  TEST_FILES
  buffer_file_test
  buffer_manager_test
  buffer_test
  concurrency_test
  file_test
  layout_test
  log_test
  record_test
  recovery_test
  table_scan_test
  transaction_test
)

foreach(file ${TEST_FILES})
  add_executable(${file} ${file}.cpp)

  target_link_libraries(${file} simpledb)
endforeach()