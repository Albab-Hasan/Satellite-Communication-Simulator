#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <iostream>

namespace Utils
{
  /**
   * @brief Convert a string to a vector of boolean bits
   *
   * @param str The string to convert
   * @return std::vector<bool> The resulting bitstream
   */
  std::vector<bool> stringToBits(const std::string &str);

  /**
   * @brief Convert a vector of boolean bits to a string
   *
   * @param bits The bitstream to convert
   * @return std::string The resulting string
   */
  std::string bitsToString(const std::vector<bool> &bits);

  /**
   * @brief Print a bitstream as a string of 0s and 1s
   *
   * @param bits The bitstream to print
   * @param os The output stream (default: std::cout)
   * @param groupSize Optional group size for better readability (default: 8)
   */
  void printBits(const std::vector<bool> &bits,
                 std::ostream &os = std::cout,
                 int groupSize = 8);

  /**
   * @brief Compare two bitstreams and count differences
   *
   * @param bits1 First bitstream
   * @param bits2 Second bitstream
   * @return int Number of bit positions that differ
   */
  int countDifferences(const std::vector<bool> &bits1,
                       const std::vector<bool> &bits2);
}

#endif // UTILS_H
