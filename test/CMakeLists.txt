set(
  TEST_FILES
  file_test
  log_test
)

foreach(file ${TEST_FILES})
  add_executable(${file} ${file}.cpp)

  target_link_libraries(${file} simpledb)
endforeach()