#pragma once

#include <unordered_map>
#include <utility>

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
   * @brief Construct a new buffer list that holds the transaction's
   * currently-pinned buffers
   * @param buffer_manager buffer manager of the database engine
   */
  explicit BufferList(BufferManager& buffer_manager)
      : buffer_manager_(buffer_manager) {}

  /**
   * @brief Return the buffer pinned to the specified block. The method returns
   * nullptr if the transaction has not pinned the block.
   * @param block a reference to the disk block
   * @return the buffer pinned to that block
   */
  Buffer* GetBuffer(const BlockId& block) const noexcept;

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
  std::unordered_map<BlockId, std::pair<Buffer*, int>> buffers_;
  BufferManager& buffer_manager_;
};
}  // namespace simpledb
