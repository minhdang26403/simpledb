add_library(
  simpledb_parse
  OBJECT
  lexer.cpp
  stream_tokenizer.cpp
  parser.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:simpledb_parse>
  PARENT_SCOPE)