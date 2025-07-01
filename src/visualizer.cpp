#include "../include/visualizer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>

// ANSI color codes
const std::string Visualizer::RESET = "\033[0m";
const std::string Visualizer::RED = "\033[31m";
const std::string Visualizer::GREEN = "\033[32m";
const std::string Visualizer::YELLOW = "\033[33m";
const std::string Visualizer::BLUE = "\033[34m";

Visualizer::Visualizer(int width, bool useColor)
    : width(width), useColor(useColor)
{
}

void Visualizer::setUseColor(bool useColor)
{
  this->useColor = useColor;
}

std::string Visualizer::colorize(const std::string &str, const std::string &color)
{
  if (useColor)
  {
    return color + str + RESET;
  }
  else
  {
    return str;
  }
}

void Visualizer::visualizeBits(const std::vector<bool> &bits,
                               const std::string &label,
                               std::ostream &os,
                               int groupSize)
{
  os << label << ": ";

  // Calculate how many characters will be needed
  int chars = bits.size() + (bits.size() / groupSize);
  if (chars + label.size() + 2 > width)
  {
    os << std::endl
       << "  "; // Move to next line with indentation
  }

  for (size_t i = 0; i < bits.size(); i++)
  {
    os << (bits[i] ? colorize("1", GREEN) : colorize("0", BLUE));

    // Add space after each group for readability
    if ((i + 1) % groupSize == 0 && i < bits.size() - 1)
    {
      os << " ";
    }
  }
  os << std::endl;
}

void Visualizer::visualizeDifferences(const std::vector<bool> &original,
                                      const std::vector<bool> &modified,
                                      const std::string &label,
                                      std::ostream &os,
                                      int groupSize)
{
  os << label << ":" << std::endl;

  // Ensure both vectors are the same size for comparison
  size_t minSize = std::min(original.size(), modified.size());

  // Print original bits
  os << "  Original: ";
  for (size_t i = 0; i < minSize; i++)
  {
    os << (original[i] ? "1" : "0");

    // Add space after each group for readability
    if ((i + 1) % groupSize == 0 && i < minSize - 1)
    {
      os << " ";
    }
  }
  os << std::endl;

  // Print modified bits with differences highlighted
  os << "  Modified: ";
  for (size_t i = 0; i < minSize; i++)
  {
    if (original[i] == modified[i])
    {
      os << (modified[i] ? "1" : "0");
    }
    else
    {
      os << colorize((modified[i] ? "1" : "0"), RED);
    }

    // Add space after each group for readability
    if ((i + 1) % groupSize == 0 && i < minSize - 1)
    {
      os << " ";
    }
  }
  os << std::endl;

  // Print arrow indicators for differences
  os << "  Errors:   ";
  for (size_t i = 0; i < minSize; i++)
  {
    if (original[i] != modified[i])
    {
      os << colorize("^", RED);
    }
    else
    {
      os << " ";
    }

    // Add space after each group for readability
    if ((i + 1) % groupSize == 0 && i < minSize - 1)
    {
      os << " ";
    }
  }
  os << std::endl;
}

void Visualizer::animateTransmission(const std::vector<bool> &bits,
                                     const std::map<int, bool> &errors,
                                     const std::string &label,
                                     std::ostream &os,
                                     int delay)
{
  os << label << ":" << std::endl;

  // Initial state
  std::vector<bool> currentBits = bits;
  visualizeBits(currentBits, "  Initial", os);

  // Wait before animation
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));

  // Animate each error
  int step = 1;
  for (const auto &[pos, newValue] : errors)
  {
    if (pos < 0 || pos >= static_cast<int>(currentBits.size()))
    {
      continue; // Skip out-of-range positions
    }

    // Update the bit
    bool oldValue = currentBits[pos];
    currentBits[pos] = newValue;

    // Show the change
    os << "  Step " << step << " - Bit at position " << pos
       << " changed from " << (oldValue ? "1" : "0")
       << " to " << colorize((newValue ? "1" : "0"), RED) << std::endl;

    visualizeBits(currentBits, "  Current", os);

    // Wait between steps
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    step++;
  }

  os << "  Final state after transmission:" << std::endl;
  visualizeBits(currentBits, "  Result", os);
}

void Visualizer::drawSeparator(std::ostream &os)
{
  os << colorize(std::string(width, '-'), YELLOW) << std::endl;
}
