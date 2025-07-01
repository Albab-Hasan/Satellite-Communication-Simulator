#ifndef REED_SOLOMON_H
#define REED_SOLOMON_H

#include "coding_strategy.h"
#include <vector>
#include <cstdint>

/**
 * @brief Simple Reed-Solomon error correction code implementation
 */
class ReedSolomon : public CodingStrategy
{
public:
  /**
   * @brief Construct a new Reed Solomon object with specific parameters
   *
   * @param symbolSize Symbol size in bits
   * @param dataSize Data size in symbols
   * @param paritySize Parity size in symbols
   */
  ReedSolomon(int symbolSize = 8, int dataSize = 8, int paritySize = 8);

  std::string getName() const override
  {
    return "Reed-Solomon(" + std::to_string(dataSize) + "," +
           std::to_string(dataSize + paritySize) + ")";
  }

  std::string getDescription() const override
  {
    return "Reed-Solomon code can detect and correct multiple symbol errors, commonly used in CDs, DVDs, and deep-space communication";
  }

  std::vector<bool> encode(const std::vector<bool> &input) override;
  std::pair<std::vector<bool>, int> decode(const std::vector<bool> &input) override;

private:
  int symbolSize; // Symbol size in bits
  int dataSize;   // Number of data symbols
  int paritySize; // Number of parity symbols
  int fieldSize;  // Field size (2^symbolSize)

  // Galois Field arithmetic operations
  uint8_t gfMul(uint8_t a, uint8_t b);
  uint8_t gfInv(uint8_t a);

  // Conversion between bits and symbols
  std::vector<uint8_t> bitsToSymbols(const std::vector<bool> &bits);
  std::vector<bool> symbolsToBits(const std::vector<uint8_t> &symbols);

  // Reed-Solomon encoding/decoding on symbols
  std::vector<uint8_t> encodeSymbols(const std::vector<uint8_t> &dataSymbols);
  std::pair<std::vector<uint8_t>, int> decodeSymbols(const std::vector<uint8_t> &receivedSymbols);

  // Generate generator polynomial for encoding
  std::vector<uint8_t> generatePolynomial();

  // Lookup tables for faster operations
  std::vector<uint8_t> expTable;
  std::vector<uint8_t> logTable;

  // Initialize lookup tables
  void initTables();
};

#endif // REED_SOLOMON_H
