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
  int mask = 1;
  alphaTo[m] = 0;

  for (int i = 0; i < m; i++)
  {
    alphaTo[i] = mask;
    indexOf[alphaTo[i]] = i;

    mask <<= 1;
    if (mask & (1 << m))
    {
      mask ^= p;
    }
  }

  indexOf[0] = -1; // Invalid index for 0

  // Continue generating field elements
  mask = 1;
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
  for (int i = 0; i < k; i++)
  {
    if (codeword[i])
    {
      for (size_t j = 0; j < g.size(); j++)
      {
        // Fix: std::vector<bool> doesn't support ^= operator
        if (g[j] == 1)
        {
          codeword[i + j] = !codeword[i + j]; // XOR operation
        }
      }
    }
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
        syn ^= alphaTo[(i + 1) * j % n];
      }
    }
    syndrome[i] = syn;
  }

  return syndrome;
}

std::vector<int> BCHCode::findErrorLocations(const std::vector<uint16_t> &syndrome)
{
  // Simplified Berlekamp-Massey algorithm
  std::vector<uint16_t> elp(t + 1, 0); // Error locator polynomial
  elp[0] = 1;

  std::vector<uint16_t> d(t + 1, 0);
  std::vector<int> l(t + 1, 0);
  std::vector<int> u_lu(t + 1, 0);
  std::vector<uint16_t> s(2 * t, 0);

  // Copy syndromes
  for (int i = 0; i < 2 * t; i++)
  {
    s[i] = syndrome[i];
  }

  // Berlekamp-Massey algorithm
  d[0] = s[0];

  if (s[0] != 0)
  {
    elp[1] = 1;
    l[0] = 1;
    u_lu[0] = 0;
  }

  for (int i = 1; i < t; i++)
  {
    d[i] = s[i];

    for (int j = 1; j <= l[i - 1]; j++)
    {
      if (elp[j] != 0 && s[i - j] != 0)
      {
        d[i] ^= alphaTo[(indexOf[elp[j]] + indexOf[s[i - j]]) % n];
      }
    }

    if (d[i] == 0)
    {
      // No change in polynomial
      l[i] = l[i - 1];
      u_lu[i] = u_lu[i - 1] + 1;
    }
    else
    {
      // Polynomial update needed
      int j = i - u_lu[i - 1];

      if (l[i - 1] < i - j)
      {
        // Update polynomial
        int L = i - j;
        int dj_idx = indexOf[d[j]];
        int di_idx = indexOf[d[i]];

        for (int m = 0; m <= l[j]; m++)
        {
          if (elp[m] != 0)
          {
            elp[m + i - j] ^= alphaTo[(indexOf[elp[m]] + di_idx - dj_idx + n) % n];
          }
        }

        l[i] = L;
        u_lu[i] = i - L;
      }
      else
      {
        // Compute new polynomial
        l[i] = l[i - 1];
        u_lu[i] = u_lu[i - 1];

        for (int m = 0; m <= l[i - 1]; m++)
        {
          uint16_t tmp = 0;
          if (d[i] != 0 && elp[m] != 0)
          {
            tmp = alphaTo[(indexOf[elp[m]] + indexOf[d[i]] - indexOf[d[0]] + n) % n];
          }
          elp[m] ^= tmp;
        }
      }
    }
  }

  // Find roots of the error locator polynomial
  std::vector<int> errorLocations;
  for (int i = 1; i < n; i++)
  {
    uint16_t sum = 0;
    for (int j = 0; j <= t; j++)
    {
      if (elp[j] != 0)
      {
        sum ^= alphaTo[(indexOf[elp[j]] + j * i) % n];
      }
    }

    if (sum == 0)
    {
      errorLocations.push_back(n - i);
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
    // Find error locations
    std::vector<int> errorLocations = findErrorLocations(syndrome);
    errorsFixed = errorLocations.size();

    // Correct errors
    for (int loc : errorLocations)
    {
      received[loc] = !received[loc];
    }
  }

  // Extract the message bits
  std::vector<bool> decoded(k);
  for (int i = 0; i < k; i++)
  {
    decoded[i] = received[i];
  }

  return {decoded, errorsFixed};
}
