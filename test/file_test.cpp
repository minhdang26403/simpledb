#include <iostream>

#include "file/block_id.h"
#include "file/file_manager.h"
#include "file/page.h"
#include "server/simpledb.h"

namespace simpledb {
void FileTest() {
  SimpleDB db{"file_test", 400, 8};
  auto& file_manager = db.GetFileManager();
  BlockId block{"test_file", 2};
  int pos1 = 88;

  Page p1{file_manager.BlockSize()};
  std::string s1 = "abcdefghijklm";
  p1.SetString(pos1, s1);
  int pos2 = pos1 + Page::StringLength(s1);
  p1.SetInt(pos2, 345);
  file_manager.Write(block, p1);

  Page p2{file_manager.BlockSize()};
  file_manager.Read(block, p2);
  std::cout << "offset " << pos2 << " contains " << p2.GetInt(pos2) << '\n';
  std::cout << "offset " << pos1 << " contains " << p2.GetString(pos1) << '\n';
}
}  // namespace simpledb

int main() {
  simpledb::FileTest();

  return 0;
}
