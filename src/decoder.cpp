#include "../include/decoder.h"
#include "../include/utils.h"

std::pair<std::string, int> Decoder::decodeToString(const std::vector<bool> &encodedBits)
{
  // Decode the bits
  auto [decodedBits, errorsFixed] = decode(encodedBits);

  // Convert decoded bits to string
  std::string message = Utils::bitsToString(decodedBits);

  return {message, errorsFixed};
}

std::pair<std::vector<bool>, int> Decoder::decode(const std::vector<bool> &encodedBits)
{
  std::vector<bool> decoded;
  int totalErrorsFixed = 0;

  // Ensure the encoded data is a multiple of 7 bits
  if (encodedBits.size() % 7 != 0)
  {
    return {{}, 0}; // Invalid encoded data
  }

  // Reserve space for the decoded data
  decoded.reserve(encodedBits.size() * 4 / 7); // Each 7 bits become 4 bits

  // Process 7 bits at a time
  for (size_t i = 0; i < encodedBits.size(); i += 7)
  {
    std::vector<bool> encodedBlock(7);
    for (int j = 0; j < 7; j++)
    {
      encodedBlock[j] = encodedBits[i + j];
    }

    // Decode the 7-bit block using Hamming(7,4)
    auto [decodedBlock, errorFixed] = decodeHamming74(encodedBlock);

    // Add the decoded block to the result
    decoded.insert(decoded.end(), decodedBlock.begin(), decodedBlock.end());

    // Update error count
    if (errorFixed)
    {
      totalErrorsFixed++;
    }
  }

  return {decoded, totalErrorsFixed};
}

std::pair<std::vector<bool>, bool> Decoder::decodeHamming74(const std::vector<bool> &encodedBlock)
{
  // Hamming(7,4) decoding
  // encodedBlock: [p1, p2, d1, p3, d2, d3, d4]
  // where p1, p2, p3 are parity bits

  std::vector<bool> decoded(4);
  bool errorFixed = false;

  // Extract received data bits
  bool receivedD1 = encodedBlock[2];
  bool receivedD2 = encodedBlock[4];
  bool receivedD3 = encodedBlock[5];
  bool receivedD4 = encodedBlock[6];

  // Extract received parity bits
  bool receivedP1 = encodedBlock[0];
  bool receivedP2 = encodedBlock[1];
  bool receivedP3 = encodedBlock[3];

  // Calculate parity checks
  bool p1Check = receivedP1 ^ receivedD1 ^ receivedD2 ^ receivedD4;
  bool p2Check = receivedP2 ^ receivedD1 ^ receivedD3 ^ receivedD4;
  bool p3Check = receivedP3 ^ receivedD2 ^ receivedD3 ^ receivedD4;

  // Calculate syndrome
  int syndrome = (p3Check ? 4 : 0) | (p2Check ? 2 : 0) | (p1Check ? 1 : 0);

  // Copy received data bits initially
  decoded[0] = receivedD1;
  decoded[1] = receivedD2;
  decoded[2] = receivedD3;
  decoded[3] = receivedD4;

  // Apply error correction based on syndrome
  if (syndrome != 0)
  {
    errorFixed = true;

    switch (syndrome)
    {
    case 1: // Error in p1
      // No data bit affected
      break;
    case 2: // Error in p2
      // No data bit affected
      break;
    case 3: // Error in d1
      decoded[0] = !receivedD1;
      break;
    case 4: // Error in p3
      // No data bit affected
      break;
    case 5: // Error in d2
      decoded[1] = !receivedD2;
      break;
    case 6: // Error in d3
      decoded[2] = !receivedD3;
      break;
    case 7: // Error in d4
      decoded[3] = !receivedD4;
      break;
    }
  }

  return {decoded, errorFixed};
}
