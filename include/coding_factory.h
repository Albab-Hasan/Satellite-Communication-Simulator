#ifndef CODING_FACTORY_H
#define CODING_FACTORY_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "coding_strategy.h"

/**
 * @brief Factory class for creating error correction coding strategies
 */
class CodingFactory
{
public:
  /**
   * @brief Get the singleton instance of the factory
   *
   * @return CodingFactory& The singleton instance
   */
  static CodingFactory &getInstance();

  /**
   * @brief Register a coding strategy with the factory
   *
   * @param name The name to register the strategy under
   * @param strategy The coding strategy
   */
  void registerStrategy(const std::string &name, std::shared_ptr<CodingStrategy> strategy);

  /**
   * @brief Create a coding strategy by name
   *
   * @param name The name of the strategy to create
   * @return std::shared_ptr<CodingStrategy> The created strategy
   */
  std::shared_ptr<CodingStrategy> createStrategy(const std::string &name);

  /**
   * @brief Get the list of available strategy names
   *
   * @return std::vector<std::string> The list of strategy names
   */
  std::vector<std::string> getStrategyNames() const;

  /**
   * @brief Get a strategy description by name
   *
   * @param name The name of the strategy
   * @return std::string The description of the strategy
   */
  std::string getStrategyDescription(const std::string &name) const;

private:
  // Private constructor to prevent direct instantiation
  CodingFactory();

  // Map of registered strategies
  std::map<std::string, std::shared_ptr<CodingStrategy>> strategies;
};

#endif // CODING_FACTORY_H
