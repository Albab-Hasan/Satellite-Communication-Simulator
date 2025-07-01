#ifndef DECODER_H
#define DECODER_H

#include <vector>
#include <string>
#include <utility>

/**
 * @brief Class responsible for decoding data and correcting errors using Hamming code
 */
class Decoder
{
public:
  /**
   * @brief Decode a bitstream into a string message with error correction
   *
   * @param encodedBits The encoded bitstream received from the channel
   * @return std::pair<std::string, int> The decoded message and number of errors corrected
   */
  std::pair<std::string, int> decodeToString(const std::vector<bool> &encodedBits);

  /**
   * @brief Decode a bitstream into binary data with error correction
   *
   * @param encodedBits The encoded bitstream received from the channel
   * @return std::pair<std::vector<bool>, int> The decoded binary data and number of errors corrected
   */
  std::pair<std::vector<bool>, int> decode(const std::vector<bool> &encodedBits);

private:
  /**
   * @brief Decode a 7-bit Hamming(7,4) encoded block and correct errors if possible
   *
   * @param encodedBlock The 7-bit encoded block
   * @return std::pair<std::vector<bool>, bool> The decoded 4-bit data and whether error was corrected
   */
  std::pair<std::vector<bool>, bool> decodeHamming74(const std::vector<bool> &encodedBlock);
};

#endif // DECODER_H
