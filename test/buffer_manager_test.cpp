#include "buffer/buffer_manager.h"

#include <iostream>
#include <vector>

#include "file/block_id.h"
#include "file/page.h"
#include "server/simpledb.h"

namespace simpledb {
void BufferManagerTest() {
  SimpleDB db{"buffer_manager_test", 400, 3};
  BufferManager& buffer_manager = db.GetBufferManager();

  std::vector<Buffer*> buff(6);
  buff[0] = buffer_manager.Pin(BlockId("test_file", 0));
  buff[1] = buffer_manager.Pin(BlockId("test_file", 1));
  buff[2] = buffer_manager.Pin(BlockId("test_file", 2));
  buffer_manager.Unpin(buff[1]);
  buff[1] = nullptr;
  buff[3] = buffer_manager.Pin(BlockId("test_file", 0));  // block 0 pinned twice
  buff[4] = buffer_manager.Pin(BlockId("test_file", 1));  // block repinned
  std::cout << "Available buffers: " << buffer_manager.Available() << '\n';
  std::cout << "Attempting to pin block 3...\n";
  buff[5] = buffer_manager.Pin(BlockId("test_file", 3));
  if (buff[5] == nullptr) {
    std::cout << "No available buffers\n";
  }
  buffer_manager.Unpin(buff[2]);
  buff[2] = nullptr;
  buff[5] = buffer_manager.Pin(BlockId("test_file", 3));

  std::cout << "Final Buffer Allocation:\n";
  for (size_t i = 0; i < buff.size(); i++) {
    if (buff[i] != nullptr) {
      std::cout << "buff[" << i << "] pinned to block " << buff[i]->Block().value().ToString()
                << '\n';
    }
  }
}
}  // namespace simpledb

int main() {
  simpledb::BufferManagerTest();

  return 0;
}
