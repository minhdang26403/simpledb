#include <iostream>
#include <thread>  // NOLINT(build/c++11)

#include "buffer/buffer_manager.h"
#include "file/file_manager.h"
#include "log/log_manager.h"
#include "server/simpledb.h"
#include "txn/transaction.h"

using namespace std::chrono_literals;  // NOLINT(build/namespaces)

namespace simpledb {
void RunA(FileManager& file_manager, LogManager& log_manager,
          BufferManager& buffer_manager) {
  Transaction txnA{file_manager, log_manager, buffer_manager};
  BlockId block1{"test_file", 1};
  BlockId block2{"test_file", 2};
  txnA.Pin(block1);
  txnA.Pin(block2);

  std::cout << "Transaction A: request SharedLock 1\n";
  txnA.GetInt(block1, 0);
  std::cout << "Transaction A: receive SharedLock 1\n";

  std::this_thread::sleep_for(1000ms);

  std::cout << "Transaction A: request SharedLock 2\n";
  txnA.GetInt(block2, 0);
  std::cout << "Transaction A: receive SharedLock 2\n";

  txnA.Commit();
  std::cout << "Transaction A: commit\n";
}

void RunB(FileManager& file_manager, LogManager& log_manager,
          BufferManager& buffer_manager) {
  Transaction txnB{file_manager, log_manager, buffer_manager};
  BlockId block1{"test_file", 1};
  BlockId block2{"test_file", 2};
  txnB.Pin(block1);
  txnB.Pin(block2);

  std::cout << "Transaction B: request ExclusiveLock 2\n";
  txnB.SetInt(block2, 0, 0, false);
  std::cout << "Transaction B: receive ExclusiveLock 2\n";

  std::this_thread::sleep_for(1000ms);

  std::cout << "Transaction B: request SharedLock 1\n";
  txnB.GetInt(block1, 0);
  std::cout << "Transaction B: receive SharedLock 1\n";

  txnB.Commit();
  std::cout << "Transaction B: commit\n";
}

void RunC(FileManager& file_manager, LogManager& log_manager,
          BufferManager& buffer_manager) {
  Transaction txnC{file_manager, log_manager, buffer_manager};
  BlockId block1{"test_file", 1};
  BlockId block2{"test_file", 2};
  txnC.Pin(block1);
  txnC.Pin(block2);

  std::cout << "Transaction C: request ExclusiveLock 1\n";
  txnC.SetInt(block1, 0, 0, false);
  std::cout << "Transaction C: receive ExclusiveLock 1\n";

  std::this_thread::sleep_for(1000ms);

  std::cout << "Transaction C: request SharedLock 2\n";
  txnC.GetInt(block2, 0);
  std::cout << "Transaction C: receive SharedLock 2\n";

  txnC.Commit();
  std::cout << "Transaction C: commit\n";
}

void ConcurrencyTest() {
  SimpleDB db{"concurrency_test", 400, 8};
  auto& file_manager = db.GetFileManager();
  auto& log_manager = db.GetLogManager();
  auto& buffer_manager = db.GetBufferManager();

  std::thread t1(RunA, std::ref(file_manager), std::ref(log_manager),
                 std::ref(buffer_manager));
  std::thread t2(RunB, std::ref(file_manager), std::ref(log_manager),
                 std::ref(buffer_manager));
  std::thread t3(RunC, std::ref(file_manager), std::ref(log_manager),
                 std::ref(buffer_manager));

  t1.join();
  t2.join();
  t3.join();
}
}  // namespace simpledb

int main() {
  simpledb::ConcurrencyTest();

  return 0;
}
