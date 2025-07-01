#include "../include/coding_factory.h"
#include "../include/hamming_code.h"
#include "../include/reed_solomon.h"
#include "../include/bch_code.h"
#include <stdexcept>

CodingFactory &CodingFactory::getInstance()
{
  static CodingFactory instance;
  return instance;
}

CodingFactory::CodingFactory()
{
  // Register default strategies
  registerStrategy("Hamming(7,4)", std::make_shared<HammingCode>());
  registerStrategy("Reed-Solomon", std::make_shared<ReedSolomon>());
  registerStrategy("BCH", std::make_shared<BCHCode>());
}

void CodingFactory::registerStrategy(const std::string &name, std::shared_ptr<CodingStrategy> strategy)
{
  strategies[name] = strategy;
}

std::shared_ptr<CodingStrategy> CodingFactory::createStrategy(const std::string &name)
{
  auto it = strategies.find(name);
  if (it == strategies.end())
  {
    throw std::invalid_argument("Unknown coding strategy: " + name);
  }
  return it->second;
}

std::vector<std::string> CodingFactory::getStrategyNames() const
{
  std::vector<std::string> names;
  for (const auto &pair : strategies)
  {
    names.push_back(pair.first);
  }
  return names;
}

std::string CodingFactory::getStrategyDescription(const std::string &name) const
{
  auto it = strategies.find(name);
  if (it == strategies.end())
  {
    throw std::invalid_argument("Unknown coding strategy: " + name);
  }
  return it->second->getDescription();
}
