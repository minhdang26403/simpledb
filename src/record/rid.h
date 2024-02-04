#include <sstream>
#include <string>

namespace simpledb {
/**
 * An identifier for a record within a file. A RID consists of the block number
 * in the file and the location of the record in that block.
 */
class RID {
 public:
  /**
   * @brief Create a RID for the record having the specified location in the
   * specified block.
   * @param block_num the block number where the record lives
   * @param slot the record's location
   */
  RID(int block_num, int slot) : block_num_(block_num), slot_(slot) {}

  /**
   * @brief Return the block number associated with this RID.
   * @return the block number
   */
  int BlockNumber() const noexcept { return block_num_; }

  /**
   * @brief Return the location of the record associated with this RID.
   * @return the record's location
   */
  int Slot() const noexcept { return slot_; }

  /**
   * @brief Return whether two RIDs identify the same record
   * @param other the other RID to compare
   * @return true if the two RIDs point to the same record; otherwise, false
   */
  bool operator==(const RID& other) const noexcept {
    return block_num_ == other.BlockNumber() && slot_ == other.Slot();
  }

  /**
   * @brief Return a string representation of the RID
   * @return the string representation
   */
  std::string ToString() {
    std::stringstream output;
    output << "[" << block_num_ << ", " << slot_ << "]";
    return output.str();
  }

 private:
  int block_num_{};
  int slot_{};
};
}  // namespace simpledb
