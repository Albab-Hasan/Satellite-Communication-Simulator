#ifndef ENCODER_H
#define ENCODER_H

#include <vector>
#include <string>

/**
 * @brief Class responsible for encoding data using Hamming code for error correction
 */
class Encoder
{
public:
  /**
   * @brief Encode a string message into a bitstream with error correction
   *
   * @param message The string message to encode
   * @return std::vector<bool> Encoded bitstream with error correction
   */
  std::vector<bool> encode(const std::string &message);

  /**
   * @brief Encode a binary message into a bitstream with error correction
   *
   * @param bits The binary message as a vector of bits
   * @return std::vector<bool> Encoded bitstream with error correction
   */
  std::vector<bool> encode(const std::vector<bool> &bits);

private:
  /**
   * @brief Apply Hamming(7,4) encoding to a 4-bit data block
   *
   * @param dataBits 4-bit data block
   * @return std::vector<bool> 7-bit encoded block with error correction capability
   */
  std::vector<bool> encodeHamming74(const std::vector<bool> &dataBits);
};

#endif // ENCODER_H
