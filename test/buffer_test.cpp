#include <iostream>

#include "buffer/buffer_manager.h"
#include "file/block_id.h"
#include "file/page.h"
#include "server/simpledb.h"

namespace simpledb {
void BufferTest() {
  SimpleDB db{"buffertest", 400, 3};
  BufferManager& buffer_manager = db.GetBufferManager();

  Buffer* buffer1 = buffer_manager.Pin(BlockId("testfile", 1));
  Page& page1 = buffer1->Contents();
  int n = page1.GetInt(80);
  page1.SetInt(80, n + 1);
  buffer1->SetModified(1, 0);  // placeholder values
  std::cout << "The new value is " << n + 1 << '\n';
  buffer_manager.Unpin(buffer1);

  // One of these pins will flush buffer1 to disk
  Buffer* buffer2 = buffer_manager.Pin(BlockId("testfile", 2));
  buffer_manager.Pin(BlockId("testfile", 3));
  buffer_manager.Pin(BlockId("testfile", 4));

  buffer_manager.Unpin(buffer2);
  buffer2 = buffer_manager.Pin(BlockId("testfile", 1));
  Page& page2 = buffer2->Contents();
  page2.SetInt(80, 9999);  // This modification won't get written to disk
  buffer2->SetModified(1, 0);
}
}  // namespace simpledb

int main() {
  simpledb::BufferTest();

  return 0;
}