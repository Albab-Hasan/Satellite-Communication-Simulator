#ifndef CODING_STRATEGY_H
#define CODING_STRATEGY_H

#include <vector>
#include <string>
#include <memory>
#include <utility>

/**
 * @brief Abstract base class for error correction coding strategies
 */
class CodingStrategy
{
public:
  virtual ~CodingStrategy() = default;

  /**
   * @brief Get the name of the coding strategy
   *
   * @return std::string The name of the coding strategy
   */
  virtual std::string getName() const = 0;

  /**
   * @brief Get the description of the coding strategy
   *
   * @return std::string The description of the coding strategy
   */
  virtual std::string getDescription() const = 0;

  /**
   * @brief Encode a binary message
   *
   * @param input The input bitstream
   * @return std::vector<bool> The encoded bitstream
   */
  virtual std::vector<bool> encode(const std::vector<bool> &input) = 0;

  /**
   * @brief Decode a received bitstream
   *
   * @param input The received bitstream
   * @return std::pair<std::vector<bool>, int> The decoded bitstream and number of corrected errors
   */
  virtual std::pair<std::vector<bool>, int> decode(const std::vector<bool> &input) = 0;
};

#endif // CODING_STRATEGY_H
