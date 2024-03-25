#include <iostream>

#include "buffer/buffer_manager.h"
#include "file/block_id.h"
#include "file/page.h"
#include "server/simpledb.h"

namespace simpledb {
void BufferFileTest() {
  SimpleDB db{"buffer_file_test", 400, 8};
  auto& buffer_manager = db.GetBufferManager();
  BlockId block{"test_file", 2};
  int pos1 = 88;

  auto buffer1 = buffer_manager.Pin(block);
  auto& page1 = buffer1->Contents();
  page1.SetString(pos1, "abcdefghijklm");
  int size = Page::StringLength("abcdefghijklm");
  int pos2 = pos1 + size;
  page1.SetInt(pos2, 345);
  buffer1->SetModified(1, 0);
  buffer_manager.Unpin(buffer1);

  auto buffer2 = buffer_manager.Pin(block);
  auto& page2 = buffer2->Contents();
  std::cout << "offset " << pos2 << " contains " << page2.GetInt(pos2) << '\n';
  std::cout << "offset " << pos1 << " contains " << page2.GetString(pos1)
            << '\n';
  buffer_manager.Unpin(buffer2);
}
}  // namespace simpledb

int main() {
  simpledb::BufferFileTest();

  return 0;
}
