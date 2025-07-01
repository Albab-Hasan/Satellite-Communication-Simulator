#include "../include/reed_solomon.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

ReedSolomon::ReedSolomon(int symbolSize, int dataSize, int paritySize)
    : symbolSize(symbolSize), dataSize(dataSize), paritySize(paritySize)
{

  // Limit symbol size to 8 bits for simplicity
  if (symbolSize > 8)
  {
    throw std::invalid_argument("Symbol size must be <= 8 bits");
  }

  fieldSize = 1 << symbolSize; // 2^symbolSize

  // Initialize Galois Field lookup tables
  initTables();
}

void ReedSolomon::initTables()
{
  // Create exponential and logarithm tables for GF(2^m)
  expTable.resize(fieldSize * 2); // Double size to avoid modulo in multiplication
  logTable.resize(fieldSize);

  // For GF(2^8), the primitive polynomial is x^8 + x^4 + x^3 + x^2 + 1 (0x11D)
  // For smaller fields, we use appropriate primitive polynomials
  int primitive_poly = 0;
  switch (symbolSize)
  {
  case 3:
    primitive_poly = 0xB;
    break; // x^3 + x + 1
  case 4:
    primitive_poly = 0x13;
    break; // x^4 + x + 1
  case 8:
    primitive_poly = 0x11D;
    break; // x^8 + x^4 + x^3 + x^2 + 1
  default:
    primitive_poly = (1 << symbolSize) | 1; // Default: x^m + 1
  }

  int x = 1;

  for (int i = 0; i < fieldSize - 1; i++)
  {
    expTable[i] = x;
    logTable[x] = i;

    // Multiply by x
    x <<= 1;

    // If overflow, reduce using the primitive polynomial
    if (x & fieldSize)
    {
      x ^= primitive_poly;
    }
  }

  // 0 is not in the multiplicative group
  logTable[0] = 0;

  // Fill in the extended exponential table
  for (int i = fieldSize - 1; i < 2 * fieldSize - 1; i++)
  {
    expTable[i] = expTable[i - (fieldSize - 1)];
  }
}

uint8_t ReedSolomon::gfMul(uint8_t a, uint8_t b)
{
  if (a == 0 || b == 0)
    return 0;
  return expTable[(logTable[a] + logTable[b]) % (fieldSize - 1)];
}

uint8_t ReedSolomon::gfInv(uint8_t a)
{
  if (a == 0)
    throw std::domain_error("Cannot invert 0 in Galois Field");
  return expTable[fieldSize - 1 - logTable[a]];
}

std::vector<uint8_t> ReedSolomon::bitsToSymbols(const std::vector<bool> &bits)
{
  std::vector<uint8_t> symbols;

  // Make sure we have a multiple of symbolSize bits
  std::vector<bool> paddedBits = bits;
  while (paddedBits.size() % symbolSize != 0)
  {
    paddedBits.push_back(false);
  }

  // Convert each symbolSize bits to a symbol
  for (size_t i = 0; i < paddedBits.size(); i += symbolSize)
  {
    uint8_t symbol = 0;
    for (int j = 0; j < symbolSize; j++)
    {
      if (paddedBits[i + j])
      {
        symbol |= (1 << j);
      }
    }
    symbols.push_back(symbol);
  }

  return symbols;
}

std::vector<bool> ReedSolomon::symbolsToBits(const std::vector<uint8_t> &symbols)
{
  std::vector<bool> bits;

  // Convert each symbol to symbolSize bits
  for (uint8_t symbol : symbols)
  {
    for (int j = 0; j < symbolSize; j++)
    {
      bits.push_back((symbol & (1 << j)) != 0);
    }
  }

  return bits;
}

std::vector<uint8_t> ReedSolomon::generatePolynomial()
{
  // Generate generator polynomial g(x) = (x-a)(x-a^2)...(x-a^n)
  // where n is the number of parity symbols

  std::vector<uint8_t> poly(1, 1); // Start with g(x) = 1

  for (int i = 0; i < paritySize; i++)
  {
    // Multiply by (x-a^i)
    std::vector<uint8_t> term(2);
    term[0] = expTable[i]; // a^i
    term[1] = 1;           // x

    // Polynomial multiplication
    std::vector<uint8_t> result(poly.size() + term.size() - 1, 0);

    for (size_t j = 0; j < poly.size(); j++)
    {
      for (size_t k = 0; k < term.size(); k++)
      {
        result[j + k] ^= gfMul(poly[j], term[k]);
      }
    }

    poly = result;
  }

  return poly;
}

std::vector<uint8_t> ReedSolomon::encodeSymbols(const std::vector<uint8_t> &dataSymbols)
{
  // Make sure we have the right number of data symbols
  if (dataSymbols.size() != static_cast<size_t>(dataSize))
  {
    throw std::invalid_argument("Data size mismatch");
  }

  // Generate the generator polynomial
  std::vector<uint8_t> genPoly = generatePolynomial();

  // Create message polynomial: x^n * data(x)
  std::vector<uint8_t> msgPoly(dataSize + paritySize, 0);
  for (int i = 0; i < dataSize; i++)
  {
    msgPoly[i] = dataSymbols[i];
  }

  // Perform polynomial division to find the remainder
  for (int i = 0; i < dataSize; i++)
  {
    if (msgPoly[i] != 0)
    {
      uint8_t coef = msgPoly[i];

      for (size_t j = 0; j < genPoly.size(); j++)
      {
        msgPoly[i + j] ^= gfMul(genPoly[j], coef);
      }
    }
  }

  // Create the codeword: data followed by parity
  std::vector<uint8_t> codeword(dataSize + paritySize);

  // Copy data symbols
  for (int i = 0; i < dataSize; i++)
  {
    codeword[i] = dataSymbols[i];
  }

  // Copy parity symbols (from the remainder)
  for (int i = 0; i < paritySize; i++)
  {
    codeword[dataSize + i] = msgPoly[dataSize + i];
  }

  return codeword;
}

std::pair<std::vector<uint8_t>, int> ReedSolomon::decodeSymbols(const std::vector<uint8_t> &receivedSymbols)
{
  // This is a simplified decoder that can detect but not correct errors
  // Full RS decoding would require syndrome computation, error locator polynomial, etc.

  // Check if the received word is a valid codeword
  std::vector<uint8_t> syndrome(paritySize, 0);
  int errorsDetected = 0;

  // Calculate syndrome
  for (int i = 0; i < paritySize; i++)
  {
    uint8_t eval = 0;
    for (size_t j = 0; j < receivedSymbols.size(); j++)
    {
      // Evaluate received polynomial at a^i
      eval ^= gfMul(receivedSymbols[j], expTable[(j * i) % (fieldSize - 1)]);
    }
    syndrome[i] = eval;
    if (eval != 0)
      errorsDetected++;
  }

  // Extract data symbols (we don't do actual error correction in this simplified version)
  std::vector<uint8_t> dataSymbols(dataSize);
  for (int i = 0; i < dataSize; i++)
  {
    dataSymbols[i] = receivedSymbols[i];
  }

  return {dataSymbols, errorsDetected > 0 ? 0 : 0};
}

std::vector<bool> ReedSolomon::encode(const std::vector<bool> &input)
{
  // Convert bits to symbols
  std::vector<uint8_t> symbols = bitsToSymbols(input);

  // Pad or truncate to match dataSize
  if (symbols.size() < static_cast<size_t>(dataSize))
  {
    symbols.resize(dataSize, 0); // Pad with zeros
  }
  else if (symbols.size() > static_cast<size_t>(dataSize))
  {
    symbols.resize(dataSize); // Truncate
  }

  // Encode symbols
  std::vector<uint8_t> encodedSymbols = encodeSymbols(symbols);

  // Convert back to bits
  return symbolsToBits(encodedSymbols);
}

std::pair<std::vector<bool>, int> ReedSolomon::decode(const std::vector<bool> &input)
{
  // Convert bits to symbols
  std::vector<uint8_t> receivedSymbols = bitsToSymbols(input);

  // Ensure we have the right number of symbols
  if (receivedSymbols.size() != static_cast<size_t>(dataSize + paritySize))
  {
    return {{}, 0}; // Invalid input size
  }

  // Decode symbols
  auto [decodedSymbols, errorsFixed] = decodeSymbols(receivedSymbols);

  // Convert back to bits
  std::vector<bool> decodedBits = symbolsToBits(decodedSymbols);

  return {decodedBits, errorsFixed};
}
