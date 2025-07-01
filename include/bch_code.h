#ifndef BCH_CODE_H
#define BCH_CODE_H

#include "coding_strategy.h"
#include <vector>
#include <cstdint>

/**
 * @brief Simple BCH error correction code implementation
 */
class BCHCode : public CodingStrategy
{
public:
  /**
   * @brief Construct a new BCH Code object
   *
   * @param m Galois field parameter (GF(2^m))
   * @param t Error correction capability
   */
  BCHCode(int m = 4, int t = 2);

  std::string getName() const override
  {
    return "BCH(" + std::to_string(n) + "," + std::to_string(k) + "," + std::to_string(t) + ")";
  }

  std::string getDescription() const override
  {
    return "BCH codes are powerful cyclic error-correcting codes used in satellite communication and storage systems";
  }

  std::vector<bool> encode(const std::vector<bool> &input) override;
  std::pair<std::vector<bool>, int> decode(const std::vector<bool> &input) override;

private:
  int m; // Galois field parameter (GF(2^m))
  int t; // Error correction capability
  int n; // Codeword length (2^m - 1)
  int k; // Message length (n - mt)

  // Galois field operations
  std::vector<uint16_t> alphaTo; // index->polynomial form conversion table
  std::vector<int> indexOf;      // polynomial->index form conversion table

  // Generator polynomial
  std::vector<uint16_t> g;

  // Initialize tables
  void initTables();

  // Generate generator polynomial
  void generatePolynomial();

  // Compute syndrome for a received word
  std::vector<uint16_t> computeSyndrome(const std::vector<bool> &received);

  // Find error locations using Berlekamp-Massey algorithm
  std::vector<int> findErrorLocations(const std::vector<uint16_t> &syndrome);
};

#endif // BCH_CODE_H
