#ifndef HAMMING_CODE_H
#define HAMMING_CODE_H

#include "coding_strategy.h"

/**
 * @brief Hamming(7,4) error correction code implementation
 */
class HammingCode : public CodingStrategy
{
public:
  std::string getName() const override
  {
    return "Hamming(7,4)";
  }

  std::string getDescription() const override
  {
    return "Hamming(7,4) code can detect up to 2-bit errors and correct 1-bit errors per 7-bit block";
  }

  std::vector<bool> encode(const std::vector<bool> &input) override;
  std::pair<std::vector<bool>, int> decode(const std::vector<bool> &input) override;

private:
  /**
   * @brief Apply Hamming(7,4) encoding to a 4-bit data block
   *
   * @param dataBits 4-bit data block
   * @return std::vector<bool> 7-bit encoded block with error correction capability
   */
  std::vector<bool> encodeHamming74(const std::vector<bool> &dataBits);

  /**
   * @brief Decode a 7-bit Hamming(7,4) encoded block and correct errors if possible
   *
   * @param encodedBlock The 7-bit encoded block
   * @return std::pair<std::vector<bool>, bool> The decoded 4-bit data and whether error was corrected
   */
  std::pair<std::vector<bool>, bool> decodeHamming74(const std::vector<bool> &encodedBlock);
};

#endif // HAMMING_CODE_H
