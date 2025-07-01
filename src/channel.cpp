#include "../include/channel.h"
#include <random>
#include <chrono>

Channel::Channel(double errorRate) : errorRate(errorRate)
{
  // Initialize random number generator with current time as seed
  unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
  rng = std::mt19937(seed);
  dist = std::uniform_real_distribution<double>(0.0, 1.0);
}

std::vector<bool> Channel::transmit(const std::vector<bool> &input)
{
  // Create a copy of the input
  std::vector<bool> output = input;

  // Apply random bit flips based on the error rate
  for (size_t i = 0; i < output.size(); i++)
  {
    // Generate a random value between 0 and 1
    double randomValue = dist(rng);

    // If the random value is less than the error rate, flip the bit
    if (randomValue < errorRate)
    {
      output[i] = !output[i];
    }
  }

  return output;
}

void Channel::setErrorRate(double newErrorRate)
{
  // Ensure error rate is in the range [0, 1]
  if (newErrorRate < 0.0)
  {
    errorRate = 0.0;
  }
  else if (newErrorRate > 1.0)
  {
    errorRate = 1.0;
  }
  else
  {
    errorRate = newErrorRate;
  }
}

double Channel::getErrorRate() const
{
  return errorRate;
}
