#pragma once

#include <chrono>  // NOLINT(build/c++11)
#include <condition_variable>  // NOLINT(build/c++11)
#include <mutex>  // NOLINT(build/c++11)
#include <vector>

#include "buffer/buffer.h"
#include "file/block_id.h"
#include "file/file_manager.h"
#include "log/log_manager.h"

namespace simpledb {
using namespace std::chrono;  // NOLINT(build/namespaces)
/**
 * Manage the pinning and unpinning of buffers to blocks.
 */
class BufferManager {
 public:
  /**
   * @brief Create a buffer manager having the specified number of buffer slots.
   * @param file_manager file manager of the database engine
   * @param log_manager log manager of the database engine
   * @param num_buffs number of buffer slots to allocate
   */
  BufferManager(FileManager& file_manager, LogManager& log_manager,
                int num_buffs);

  /**
   * @brief Return the number of available (i.e. unpinned) buffers
   * @return the number of available buffers
   */
  int Available() const;

  /**
   * @brief Flush the dirty buffers modified by the specified transaction
   * @param txn_id id of the modifying transaction
   */
  void FlushAll(int txn_id);

  /**
   * @brief Unpin the specified data buffer. If its pin count goes to zero, then
   * notify any waiting threads.
   * @param buffer the buffer to unpin
   */
  void Unpin(Buffer* buffer);

  /**
   * @brief Pin a buffer to the specfied block, potentially waiting until a
   * buffer becomes available. If no buffer becomes available within a fixed
   * time period, return `nullptr` to indicate that a buffer request could not
   * be satisfied.
   * @param block a reference to a disk block
   * @return the buffer pinned to that block
   */
  Buffer* Pin(const BlockId& block);

 private:
  /**
   * @brief Return whether `MAX_TIME` milliseconds has passed since the
   * specified start time
   * @param start_time start time of the duration
   * @return true if `MAX_TIME` milliseconds has passed; otherwise, false
   */
  bool WaitingTooLong(
      const time_point<system_clock>& start_time) const noexcept;

  /**
   * @brief Try to pin a buffer to the specified block. If there is already a
   * buffer assigned to that block then that buffer is used; otherwise, an
   * unpinned buffer from the pool is chosen. Return a null pointer if there are
   * no available buffers.
   * @param block a reference to a disk block
   * @return the pinned buffer
   */
  Buffer* TryToPin(const BlockId& block);

  /**
   * @brief Find a buffer that is already assigned to the specifed block
   * @param block a reference to the block that some buffer may be pinned to it
   * @return the pinned buffer
   */
  Buffer* FindExistingBuffer(const BlockId& block) noexcept;

  /**
   * @brief Find an unpinned (available) buffer to allocate for some disk block
   * @return the unpinned buffer
   */
  Buffer* ChooseUnpinnedBuffer() noexcept;

  std::vector<Buffer> buffer_pool_;
  int num_available_{};
  static constexpr milliseconds MAX_TIME = 10000ms;
  mutable std::mutex mutex_;
  std::condition_variable cv_;
};
}  // namespace simpledb
