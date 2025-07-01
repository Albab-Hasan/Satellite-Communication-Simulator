#include "../include/bch_code.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

BCHCode::BCHCode(int m, int t) : m(m), t(t)
{
  // Compute code parameters
  n = (1 << m) - 1; // Codeword length
  k = n - m * t;    // Message length

  // Initialize tables
  initTables();

  // Generate generator polynomial
  generatePolynomial();
}

void BCHCode::initTables()
{
  // Initialize tables for Galois field operations
  int fieldSize = 1 << m;
  alphaTo.resize(fieldSize, 0);
  indexOf.resize(fieldSize, 0);

  // Choose primitive polynomial based on m
  int p = 0;
  switch (m)
  {
  case 2:
    p = 0x7;
    break; // x^2 + x + 1
  case 3:
    p = 0xB;
    break; // x^3 + x + 1
  case 4:
    p = 0x13;
    break; // x^4 + x + 1
  case 5:
    p = 0x25;
    break; // x^5 + x^2 + 1
  case 6:
    p = 0x43;
    break; // x^6 + x + 1
  case 7:
    p = 0x89;
    break; // x^7 + x^3 + 1
  case 8:
    p = 0x11D;
    break; // x^8 + x^4 + x^3 + x^2 + 1
  default:
    throw std::invalid_argument("Unsupported field size");
  }

  // Generate field elements using the primitive polynomial
  int x = 1;
  alphaTo[m] = 0;

  for (int i = 0; i < m; i++)
  {
    alphaTo[i] = x;
    indexOf[alphaTo[i]] = i;

    x <<= 1;
    if (x & (1 << m))
    {
      x ^= p;
    }
  }

  indexOf[0] = -1; // Invalid index for 0

  // Continue generating field elements
  for (int i = m; i < n; i++)
  {
    if (alphaTo[i - 1] >= (1 << (m - 1)))
    {
      alphaTo[i] = (alphaTo[i - 1] << 1) ^ p;
    }
    else
    {
      alphaTo[i] = alphaTo[i - 1] << 1;
    }
    indexOf[alphaTo[i]] = i;
  }
}

void BCHCode::generatePolynomial()
{
  // Initialize generator polynomial g(x) = 1
  g.resize(n - k + 1, 0);
  g[0] = 1;

  // Find minimal polynomials and multiply them
  for (int i = 1; i <= 2 * t; i += 2)
  {
    // Minimal polynomial for a^i (assuming a^i and a^(i+1) are conjugates)
    std::vector<uint16_t> mp(m + 1, 0);
    mp[0] = 1;

    // Compute minimal polynomial
    int root = i;
    for (int j = 1; j <= m; j++)
    {
      mp[j] = 1;
      for (int l = j - 1; l > 0; l--)
      {
        if (mp[l] != 0)
        {
          mp[l] = mp[l - 1] ^ alphaTo[(indexOf[mp[l]] + root) % n];
        }
        else
        {
          mp[l] = mp[l - 1];
        }
      }
      mp[0] = alphaTo[(indexOf[mp[0]] + root) % n];
      root = (root * 2) % n;
    }

    // Multiply generator polynomial by minimal polynomial
    std::vector<uint16_t> tmp(n - k + 1, 0);
    for (size_t j = 0; j < g.size(); j++)
    {
      for (size_t l = 0; l < mp.size(); l++)
      {
        if (j + l < tmp.size())
        {
          if (g[j] != 0 && mp[l] != 0)
          {
            tmp[j + l] ^= alphaTo[(indexOf[g[j]] + indexOf[mp[l]]) % n];
          }
        }
      }
    }
    g = tmp;
  }
}

std::vector<bool> BCHCode::encode(const std::vector<bool> &input)
{
  // Pad or truncate input to k bits
  std::vector<bool> message(k);
  for (int i = 0; i < k && i < static_cast<int>(input.size()); i++)
  {
    message[i] = input[i];
  }

  // Create codeword vector
  std::vector<bool> codeword(n);

  // Copy message bits to higher-order positions
  for (int i = 0; i < k; i++)
  {
    codeword[i] = message[i];
  }

  // Compute redundant bits using generator polynomial
  std::vector<bool> temp = codeword; // Make a copy to work with

  for (int i = 0; i < k; i++)
  {
    if (temp[i])
    {
      for (int j = 0; j < static_cast<int>(g.size()); j++)
      {
        if (g[j] != 0 && i + j < n)
        {
          // XOR operation (^ doesn't work directly on vector<bool> references)
          temp[i + j] = temp[i + j] != (g[j] == 1);
        }
      }
    }
  }

  // Copy parity bits to codeword
  for (int i = k; i < n; i++)
  {
    codeword[i] = temp[i];
  }

  return codeword;
}

std::vector<uint16_t> BCHCode::computeSyndrome(const std::vector<bool> &received)
{
  // Compute syndromes
  std::vector<uint16_t> syndrome(2 * t, 0);

  for (int i = 0; i < 2 * t; i++)
  {
    uint16_t syn = 0;
    for (int j = 0; j < n; j++)
    {
      if (received[j])
      {
        if (j == 0)
        {
          syn ^= 1;
        }
        else
        {
          int idx = (i + 1) * j % n;
          syn ^= alphaTo[idx];
        }
      }
    }
    syndrome[i] = syn;
  }

  return syndrome;
}

std::vector<int> BCHCode::findErrorLocations(const std::vector<uint16_t> &syndrome)
{
  // Berlekamp-Massey algorithm with improved error correction
  std::vector<uint16_t> elp(t + 2, 0); // Error locator polynomial
  std::vector<uint16_t> d(t + 2, 0);   // Discrepancy
  std::vector<uint16_t> l(t + 2, 0);   // Current error locator length
  std::vector<uint16_t> b(t + 2, 0);   // Correction polynomial
  std::vector<int> errorLocations;

  elp[0] = 1; // Initialize error locator polynomial
  elp[1] = syndrome[0];

  l[0] = 0;
  l[1] = 0;

  if (syndrome[0] != 0)
  {
    l[1] = 1;
  }

  int k = 1;

  // Main algorithm loop
  for (int i = 1; i < t; i++)
  {
    d[i + 1] = syndrome[i];

    for (int j = 1; j <= l[i]; j++)
    {
      if (elp[j] != 0 && syndrome[i - j] != 0)
      {
        d[i + 1] ^= alphaTo[(indexOf[elp[j]] + indexOf[syndrome[i - j]]) % n];
      }
    }

    if (d[i + 1] == 0)
    {
      l[i + 1] = l[i];
      for (int j = 0; j <= l[i]; j++)
      {
        elp[j + i + 1 - k] = elp[j];
      }
    }
    else
    {
      int m;
      for (m = 1; m <= k && d[m] == 0; m++)
        ;

      if (m <= k && l[m] < l[i])
      {
        l[i + 1] = l[i];
        for (int j = 0; j <= l[i]; j++)
        {
          b[j + i + 1 - k] = elp[j] ^ alphaTo[(indexOf[d[i + 1]] + n - indexOf[d[m]] + indexOf[b[j + m - k]]) % n];
        }
      }
      else
      {
        l[i + 1] = i + 1 - k;
        for (int j = 0; j <= l[k]; j++)
        {
          b[j] = elp[j];
        }

        for (int j = 0; j <= l[i]; j++)
        {
          if (elp[j] != 0 && d[i + 1] != 0)
          {
            elp[j + i + 1 - k] = elp[j] ^ alphaTo[(indexOf[d[i + 1]] + n - indexOf[d[k]] + indexOf[b[j]]) % n];
          }
          else
          {
            elp[j + i + 1 - k] = elp[j];
          }
        }
      }

      k = i + 1;
    }
  }

  // Find roots of the error locator polynomial using Chien search
  for (int i = 1; i <= n; i++)
  {
    uint16_t sum = 0;
    for (int j = 0; j <= l[t]; j++)
    {
      if (elp[j] != 0)
      {
        sum ^= alphaTo[(indexOf[elp[j]] + j * i) % n];
      }
    }

    if (sum == 0)
    {
      errorLocations.push_back(n - i); // Root found - this is an error location
    }
  }

  return errorLocations;
}

std::pair<std::vector<bool>, int> BCHCode::decode(const std::vector<bool> &input)
{
  // Ensure received word has correct length
  if (static_cast<int>(input.size()) != n)
  {
    return {{}, 0}; // Error: Invalid input size
  }

  // Make a copy of the received word
  std::vector<bool> received = input;

  // Compute syndromes
  std::vector<uint16_t> syndrome = computeSyndrome(received);

  // Check if all syndromes are zero (no errors)
  bool errorFree = true;
  for (size_t i = 0; i < syndrome.size(); i++)
  {
    if (syndrome[i] != 0)
    {
      errorFree = false;
      break;
    }
  }

  int errorsFixed = 0;

  if (!errorFree)
  {
    try
    {
      // Find error locations
      std::vector<int> errorLocations = findErrorLocations(syndrome);
      errorsFixed = errorLocations.size();

      // Correct errors - simply flip the bits at error locations
      for (int loc : errorLocations)
      {
        if (loc >= 0 && loc < n)
        {
          received[loc] = !received[loc];
        }
      }

      // Verify correction by recomputing syndrome
      std::vector<uint16_t> checkSyndrome = computeSyndrome(received);
      bool allZero = true;
      for (auto &s : checkSyndrome)
      {
        if (s != 0)
        {
          allZero = false;
          break;
        }
      }

      if (!allZero)
      {
        // Correction failed, too many errors
        errorsFixed = 0;
      }
    }
    catch (const std::exception &e)
    {
      // Error correction failed
      errorsFixed = 0;
    }
  }

  // Extract the message bits (first k bits)
  std::vector<bool> decoded(k);
  for (int i = 0; i < k; i++)
  {
    decoded[i] = received[i];
  }

  return {decoded, errorsFixed};
}
