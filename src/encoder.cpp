#include "../include/encoder.h"
#include "../include/utils.h"

std::vector<bool> Encoder::encode(const std::string &message)
{
  // Convert string to bits
  std::vector<bool> messageBits = Utils::stringToBits(message);

  // Encode the bits
  return encode(messageBits);
}

std::vector<bool> Encoder::encode(const std::vector<bool> &bits)
{
  std::vector<bool> encoded;

  // Pad the input if necessary to make it a multiple of 4
  std::vector<bool> paddedBits = bits;
  while (paddedBits.size() % 4 != 0)
  {
    paddedBits.push_back(false);
  }

  // Reserve space for the encoded data
  encoded.reserve(paddedBits.size() * 7 / 4); // Each 4 bits become 7 bits

  // Process 4 bits at a time
  for (size_t i = 0; i < paddedBits.size(); i += 4)
  {
    std::vector<bool> dataBlock(4);
    for (int j = 0; j < 4; j++)
    {
      dataBlock[j] = paddedBits[i + j];
    }

    // Encode the 4-bit block using Hamming(7,4)
    std::vector<bool> encodedBlock = encodeHamming74(dataBlock);

    // Add the encoded block to the result
    encoded.insert(encoded.end(), encodedBlock.begin(), encodedBlock.end());
  }

  return encoded;
}

std::vector<bool> Encoder::encodeHamming74(const std::vector<bool> &dataBits)
{
  // Hamming(7,4) encoding
  // dataBits: [d1, d2, d3, d4]
  // encoded: [p1, p2, d1, p3, d2, d3, d4]
  // where p1, p2, p3 are parity bits

  std::vector<bool> encoded(7);

  // Data bits
  encoded[2] = dataBits[0]; // d1
  encoded[4] = dataBits[1]; // d2
  encoded[5] = dataBits[2]; // d3
  encoded[6] = dataBits[3]; // d4

  // Calculate parity bits
  // p1 covers bits 1, 3, 5, 7 (0-based: 0, 2, 4, 6)
  encoded[0] = encoded[2] ^ encoded[4] ^ encoded[6];

  // p2 covers bits 2, 3, 6, 7 (0-based: 1, 2, 5, 6)
  encoded[1] = encoded[2] ^ encoded[5] ^ encoded[6];

  // p3 covers bits 4, 5, 6, 7 (0-based: 3, 4, 5, 6)
  encoded[3] = encoded[4] ^ encoded[5] ^ encoded[6];

  return encoded;
}
