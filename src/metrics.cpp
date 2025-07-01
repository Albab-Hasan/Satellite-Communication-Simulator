#include "../include/metrics.h"
#include <iomanip>
#include <algorithm>

Metrics::Metrics()
{
  reset();
}

void Metrics::reset()
{
  totalBits = 0;
  errorBits = 0;
  errorsDetected = 0;
  errorsCorrected = 0;
  messageBitsTotal = 0;
  encodedBitsTotal = 0;
  startTimes.clear();
  durations.clear();
}

void Metrics::startOperation(const std::string &operation)
{
  startTimes[operation] = std::chrono::high_resolution_clock::now();
}

void Metrics::endOperation(const std::string &operation)
{
  auto endTime = std::chrono::high_resolution_clock::now();
  auto it = startTimes.find(operation);

  if (it != startTimes.end())
  {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - it->second).count() / 1000.0;
    durations[operation] = duration;
  }
}

void Metrics::recordErrors(const std::vector<bool> &originalBits,
                           const std::vector<bool> &receivedBits)
{
  size_t minSize = std::min(originalBits.size(), receivedBits.size());

  for (size_t i = 0; i < minSize; i++)
  {
    totalBits++;
    if (originalBits[i] != receivedBits[i])
    {
      errorBits++;
    }
  }
}

void Metrics::recordErrorCorrection(int detected, int corrected)
{
  errorsDetected += detected;
  errorsCorrected += corrected;
}

void Metrics::recordTransmission(int messageBits, int encodedBits)
{
  messageBitsTotal += messageBits;
  encodedBitsTotal += encodedBits;
}

double Metrics::getBitErrorRate() const
{
  if (totalBits == 0)
    return 0.0;
  return static_cast<double>(errorBits) / totalBits;
}

double Metrics::getErrorCorrectionRate() const
{
  if (errorsDetected == 0)
    return 0.0;
  return static_cast<double>(errorsCorrected) / errorsDetected;
}

double Metrics::getCodeRate() const
{
  if (encodedBitsTotal == 0)
    return 0.0;
  return static_cast<double>(messageBitsTotal) / encodedBitsTotal;
}

double Metrics::getOperationTime(const std::string &operation) const
{
  auto it = durations.find(operation);
  if (it != durations.end())
  {
    return it->second;
  }
  return 0.0;
}

void Metrics::printSummary(std::ostream &os) const
{
  os << "=== Performance Metrics ===" << std::endl;

  // Error statistics
  os << "Error Statistics:" << std::endl;
  os << "  Total bits transmitted: " << totalBits << std::endl;
  os << "  Bit errors introduced: " << errorBits << std::endl;
  os << "  Bit error rate: " << std::fixed << std::setprecision(6) << getBitErrorRate() * 100 << "%" << std::endl;
  os << "  Errors detected: " << errorsDetected << std::endl;
  os << "  Errors corrected: " << errorsCorrected << std::endl;

  if (errorsDetected > 0)
  {
    os << "  Error correction rate: " << std::fixed << std::setprecision(2)
       << getErrorCorrectionRate() * 100 << "%" << std::endl;
  }

  // Transmission statistics
  os << "Transmission Statistics:" << std::endl;
  os << "  Message bits: " << messageBitsTotal << std::endl;
  os << "  Encoded bits: " << encodedBitsTotal << std::endl;
  os << "  Code rate: " << std::fixed << std::setprecision(3) << getCodeRate() << std::endl;
  os << "  Redundancy: " << std::fixed << std::setprecision(1)
     << (1.0 - getCodeRate()) * 100 << "%" << std::endl;

  // Timing information
  os << "Timing Information:" << std::endl;
  for (const auto &[op, time] : durations)
  {
    os << "  " << op << ": " << std::fixed << std::setprecision(3) << time << " ms" << std::endl;
  }

  // Calculate total time
  double totalTime = 0.0;
  for (const auto &[op, time] : durations)
  {
    totalTime += time;
  }
  os << "  Total processing time: " << std::fixed << std::setprecision(3) << totalTime << " ms" << std::endl;

  // Efficiency metrics if we have both time and bits
  if (totalTime > 0 && encodedBitsTotal > 0)
  {
    double bitsPerMs = encodedBitsTotal / totalTime;
    os << "  Throughput: " << std::fixed << std::setprecision(3) << bitsPerMs << " bits/ms"
       << " (" << std::fixed << std::setprecision(3) << bitsPerMs * 1000 << " bits/second)" << std::endl;
  }
}
