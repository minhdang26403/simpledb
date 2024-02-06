#include <iostream>
#include <string>

#include "buffer/buffer_manager.h"
#include "file/block_id.h"
#include "file/file_manager.h"
#include "server/simpledb.h"
#include "txn/transaction.h"

namespace simpledb {

class RecoveryTest {
 public:
  RecoveryTest(std::string_view dirname, std::string_view filename)
      : db_(dirname, 400, 8),
        test_dir_(dirname),
        test_file_(filename),
        file_manager_(db_.GetFileManager()),
        buffer_manager_(db_.GetBufferManager()),
        block0_(filename, 0),
        block1_(filename, 1) {}

  void Execute() {
    if (file_manager_.Length(test_file_) == 0) {
      Initialize();
      Modify();
    } else {
      Recover();
    }
  }

 private:
  void Initialize() {
    Transaction txn1{file_manager_, db_.GetLogManager(), buffer_manager_};
    Transaction txn2{file_manager_, db_.GetLogManager(), buffer_manager_};
    txn1.Pin(block0_);
    txn2.Pin(block1_);

    int pos = 0;
    for (int i = 0; i < 6; i++) {
      txn1.SetInt(block0_, pos, pos, false);
      txn2.SetInt(block1_, pos, pos, false);
      pos += sizeof(int);
    }

    txn1.SetString(block0_, 30, "abc", false);
    txn2.SetString(block1_, 30, "def", false);
    txn1.Commit();
    txn2.Commit();
    PrintValues("After Initialization:");
  }

  void Modify() {
    Transaction txn3{file_manager_, db_.GetLogManager(), buffer_manager_};
    Transaction txn4{file_manager_, db_.GetLogManager(), buffer_manager_};
    txn3.Pin(block0_);
    txn4.Pin(block1_);

    int pos = 0;
    for (int i = 0; i < 6; i++) {
      txn3.SetInt(block0_, pos, pos + 100, true);
      txn4.SetInt(block1_, pos, pos + 100, true);
      pos += sizeof(int);
    }

    txn3.SetString(block0_, 30, "uvw", true);
    txn4.SetString(block1_, 30, "xyz", true);
    buffer_manager_.FlushAll(3);
    buffer_manager_.FlushAll(4);
    PrintValues("After modification:");

    txn3.Rollback();
    PrintValues("After rollback:");
    // txn4 stops here without committing or rolling back,
    // so all its changes should be undone during recovery
  }

  void Recover() {
    Transaction txn{file_manager_, db_.GetLogManager(), buffer_manager_};
    txn.Recover();
    PrintValues("After recovery:");
  }

  // Print the values that made it to disk
  void PrintValues(std::string_view msg) {
    std::cout << msg << '\n';
    Page page0{file_manager_.BlockSize()};
    Page page1{file_manager_.BlockSize()};
    file_manager_.Read(block0_, page0);
    file_manager_.Read(block1_, page1);

    int pos = 0;
    for (int i = 0; i < 6; i++) {
      std::cout << page0.GetInt(pos) << ' ' << page1.GetInt(pos) << ' ';
      pos += sizeof(int);
    }
    std::cout << page0.GetString(30) << ' ' << page1.GetString(30) << '\n';
  }

  SimpleDB db_;
  std::string test_dir_;
  std::string test_file_;
  FileManager& file_manager_;
  BufferManager& buffer_manager_;
  BlockId block0_, block1_;
};
}  // namespace simpledb

int main() {
  simpledb::RecoveryTest recovery_test{"recovery_test", "test_file"};
  recovery_test.Execute();
}
