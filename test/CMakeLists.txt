set(
  TEST_FILES
  buffer_file_test
  buffer_manager_test
  buffer_test
  catalog_test
  concurrency_test
  file_test
  layout_test
  lexer_test
  log_test
  metadata_manager_test
  parser_test
  predicate_parser_test
  product_test
  record_test
  recovery_test
  scan_test
  table_manager_test
  table_scan_test
  tokenizer_test
  transaction_test
)

foreach(file ${TEST_FILES})
  add_executable(${file} ${file}.cpp)

  target_link_libraries(${file} simpledb)
endforeach()