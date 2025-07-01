#ifndef CHANNEL_H
#define CHANNEL_H

#include <vector>
#include <random>

/**
 * @brief Class that simulates a noisy communication channel
 */
class Channel
{
public:
  /**
   * @brief Construct a new Channel object with a specific bit error rate
   *
   * @param errorRate Probability of a bit flip (0.0 to 1.0)
   */
  Channel(double errorRate);

  /**
   * @brief Transmit data through the noisy channel
   *
   * @param input The bitstream to transmit
   * @return std::vector<bool> The received bitstream with possible errors
   */
  std::vector<bool> transmit(const std::vector<bool> &input);

  /**
   * @brief Set the bit error rate
   *
   * @param errorRate New error rate (0.0 to 1.0)
   */
  void setErrorRate(double errorRate);

  /**
   * @brief Get the current bit error rate
   *
   * @return double Current error rate
   */
  double getErrorRate() const;

private:
  double errorRate;                            // Probability of a bit flip (0.0 to 1.0)
  std::mt19937 rng;                            // Random number generator
  std::uniform_real_distribution<double> dist; // Distribution for random numbers
};

#endif // CHANNEL_H
