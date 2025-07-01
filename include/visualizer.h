#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <vector>
#include <string>
#include <iostream>
#include <map>

/**
 * @brief Class for visualizing the communication process
 */
class Visualizer
{
public:
  /**
   * @brief Construct a new Visualizer object
   *
   * @param width The width of the visualization (in characters)
   * @param useColor Whether to use ANSI color codes
   */
  Visualizer(int width = 80, bool useColor = true);

  /**
   * @brief Set whether to use color in visualization
   *
   * @param useColor Whether to use ANSI color codes
   */
  void setUseColor(bool useColor);

  /**
   * @brief Visualize a bitstream
   *
   * @param bits The bitstream to visualize
   * @param label The label for the bitstream
   * @param os The output stream to write to (default: std::cout)
   * @param groupSize The size of bit groups (default: 8)
   */
  void visualizeBits(const std::vector<bool> &bits,
                     const std::string &label,
                     std::ostream &os = std::cout,
                     int groupSize = 8);

  /**
   * @brief Visualize differences between two bitstreams
   *
   * @param original The original bitstream
   * @param modified The modified bitstream
   * @param label The label for the visualization
   * @param os The output stream to write to (default: std::cout)
   * @param groupSize The size of bit groups (default: 8)
   */
  void visualizeDifferences(const std::vector<bool> &original,
                            const std::vector<bool> &modified,
                            const std::string &label,
                            std::ostream &os = std::cout,
                            int groupSize = 8);

  /**
   * @brief Visualize a transmission animation
   *
   * @param bits The bitstream to animate
   * @param errors A map of positions and new values for errors
   * @param label The label for the animation
   * @param os The output stream to write to (default: std::cout)
   * @param delay The delay between animation frames in milliseconds (default: 200)
   */
  void animateTransmission(const std::vector<bool> &bits,
                           const std::map<int, bool> &errors,
                           const std::string &label,
                           std::ostream &os = std::cout,
                           int delay = 200);

  /**
   * @brief Draw a line separator
   *
   * @param os The output stream to write to (default: std::cout)
   */
  void drawSeparator(std::ostream &os = std::cout);

private:
  int width;     // Width of visualization
  bool useColor; // Whether to use ANSI color codes

  // ANSI color codes
  static const std::string RESET;
  static const std::string RED;
  static const std::string GREEN;
  static const std::string YELLOW;
  static const std::string BLUE;

  /**
   * @brief Apply color to a string if color is enabled
   *
   * @param str The string to color
   * @param color The ANSI color code
   * @return std::string The colored string
   */
  std::string colorize(const std::string &str, const std::string &color);
};

#endif // VISUALIZER_H
