#include "../include/hamming_code.h"

std::vector<bool> HammingCode::encode(const std::vector<bool> &input)
{
  std::vector<bool> encoded;

  // Pad the input if necessary to make it a multiple of 4
  std::vector<bool> paddedBits = input;
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

std::vector<bool> HammingCode::encodeHamming74(const std::vector<bool> &dataBits)
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

std::pair<std::vector<bool>, int> HammingCode::decode(const std::vector<bool> &input)
{
  std::vector<bool> decoded;
  int totalErrorsFixed = 0;

  // Ensure the encoded data is a multiple of 7 bits
  if (input.size() % 7 != 0)
  {
    return {{}, 0}; // Invalid encoded data
  }

  // Reserve space for the decoded data
  decoded.reserve(input.size() * 4 / 7); // Each 7 bits become 4 bits

  // Process 7 bits at a time
  for (size_t i = 0; i < input.size(); i += 7)
  {
    std::vector<bool> encodedBlock(7);
    for (int j = 0; j < 7; j++)
    {
      encodedBlock[j] = input[i + j];
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

std::pair<std::vector<bool>, bool> HammingCode::decodeHamming74(const std::vector<bool> &encodedBlock)
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

  // Calculate parity checks (XOR should be 0 if no errors)
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
