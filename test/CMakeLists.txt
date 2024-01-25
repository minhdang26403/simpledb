set(
  TEST_FILES
  buffer_file_test
  buffer_manager_test
  buffer_test
  file_test
  log_test
)

foreach(file ${TEST_FILES})
  add_executable(${file} ${file}.cpp)

  target_link_libraries(${file} simpledb)
endforeach()