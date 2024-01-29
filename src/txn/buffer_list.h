#pragma once

#include <unordered_map>

#include "buffer/buffer.h"
#include "buffer/buffer_manager.h"
#include "file/block_id.h"

namespace simpledb {
/**
 * Manage the transaction's currently-pinned buffers
 */
class BufferList {
 public:
  /**
   * @brief Construct a new Buffer List object that holds the transaction's
   * currently-pinned buffers
   * @param buffer_manager buffer manager of the database engine
   */
  explicit BufferList(BufferManager& buffer_manager) : buffer_manager_(buffer_manager) {}

  /**
   * @brief Return the buffer pinned to the specified block. The method returns
   * nullptr if the transaction has not pinned the block.
   * @param block a reference to the disk block
   * @return the buffer pinned to that block
   */
  Buffer* GetBuffer(const BlockId& block) const;

  /**
   * @brief Pin the block and keep track of the buffer internally
   * @param block a reference to the disk block
   */
  void Pin(const BlockId& block);

  /**
   * @brief Unpin the specified block
   * @param block a reference to the disk block
   */
  void Unpin(const BlockId& block);

  /**
   * @brief Unpin any buffers still pinned by this transaction
   */
  void UnpinAll();

 private:
  // TODO(DANG): The original implementation handles the case where a
  // transaction pins a buffer multiple times by also keeping a vector of pinned
  // buffers
  std::unordered_map<BlockId, Buffer*> buffers_;
  BufferManager& buffer_manager_;
};
}  // namespace simpledb
