#ifndef METRICS_H
#define METRICS_H

#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <iostream>

/**
 * @brief Class for tracking and reporting communication performance metrics
 */
class Metrics
{
public:
  /**
   * @brief Construct a new Metrics object
   */
  Metrics();

  /**
   * @brief Reset all metrics
   */
  void reset();

  /**
   * @brief Record the start time of an operation
   *
   * @param operation The name of the operation
   */
  void startOperation(const std::string &operation);

  /**
   * @brief Record the end time of an operation
   *
   * @param operation The name of the operation
   */
  void endOperation(const std::string &operation);

  /**
   * @brief Record bit error information
   *
   * @param originalBits Original bits before transmission
   * @param receivedBits Received bits after transmission
   */
  void recordErrors(const std::vector<bool> &originalBits,
                    const std::vector<bool> &receivedBits);

  /**
   * @brief Record information about error correction
   *
   * @param errorsDetected Number of errors detected
   * @param errorsCorrected Number of errors corrected
   */
  void recordErrorCorrection(int errorsDetected, int errorsCorrected);

  /**
   * @brief Record information about a transmission
   *
   * @param messageBits Number of message bits
   * @param encodedBits Number of encoded bits (including redundancy)
   */
  void recordTransmission(int messageBits, int encodedBits);

  /**
   * @brief Get the bit error rate
   *
   * @return double The bit error rate
   */
  double getBitErrorRate() const;

  /**
   * @brief Get the error correction rate
   *
   * @return double The error correction rate
   */
  double getErrorCorrectionRate() const;

  /**
   * @brief Get the code rate (message bits / encoded bits)
   *
   * @return double The code rate
   */
  double getCodeRate() const;

  /**
   * @brief Get the operation duration in milliseconds
   *
   * @param operation The name of the operation
   * @return double Duration in milliseconds
   */
  double getOperationTime(const std::string &operation) const;

  /**
   * @brief Print a summary of all metrics
   *
   * @param os The output stream to write to (default: std::cout)
   */
  void printSummary(std::ostream &os = std::cout) const;

private:
  // Error statistics
  int totalBits;
  int errorBits;
  int errorsDetected;
  int errorsCorrected;

  // Transmission statistics
  int messageBitsTotal;
  int encodedBitsTotal;

  // Timing information
  std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> startTimes;
  std::map<std::string, double> durations;
};

#endif // METRICS_H
