#include "../include/utils.h"
#include <bitset>
#include <iomanip>

namespace Utils
{

  std::vector<bool> stringToBits(const std::string &str)
  {
    std::vector<bool> bits;
    // Reserve space for all bits (8 bits per character)
    bits.reserve(str.length() * 8);

    // Convert each character to its bit representation
    for (char c : str)
    {
      std::bitset<8> bitset(c);
      for (int i = 0; i < 8; i++)
      {
        bits.push_back(bitset[i]); // Push bits starting from LSB
      }
    }

    return bits;
  }

  std::string bitsToString(const std::vector<bool> &bits)
  {
    std::string result;

    // Make sure we have complete bytes
    if (bits.size() % 8 != 0)
    {
      return result; // Return empty string if bits don't form complete bytes
    }

    // Convert each 8 bits to a character
    for (size_t i = 0; i < bits.size(); i += 8)
    {
      char c = 0;
      for (int j = 0; j < 8; j++)
      {
        if (bits[i + j])
        {
          c |= (1 << j); // Set the bit if true
        }
      }
      result.push_back(c);
    }

    return result;
  }

  void printBits(const std::vector<bool> &bits,
                 std::ostream &os,
                 int groupSize)
  {

    for (size_t i = 0; i < bits.size(); i++)
    {
      os << (bits[i] ? '1' : '0');

      // Add space after each group for readability
      if ((i + 1) % groupSize == 0 && i < bits.size() - 1)
      {
        os << ' ';
      }
    }
  }

  int countDifferences(const std::vector<bool> &bits1,
                       const std::vector<bool> &bits2)
  {

    if (bits1.size() != bits2.size())
    {
      return -1; // Error: different sizes
    }

    int differences = 0;
    for (size_t i = 0; i < bits1.size(); i++)
    {
      if (bits1[i] != bits2[i])
      {
        differences++;
      }
    }

    return differences;
  }

} // namespace Utils
