#pragma once
#include "bitboard_constants.h"
#include "board_utils.h"
#include <climits>
#include <cmath>
#include <random>

namespace Panzer {
class zorbist_lookup {
public:
  hash color_hash = 0;
  hash zorbist_castle_hash[ALL_CASTLE_FLAGS + 1] = {0};
  hash zorbist_ep_hash[65] = {0};

  std::uniform_int_distribution<hash> dist;
  std::mt19937 engine;

  zorbist_lookup() {
    FillHashLookup();
    dist = std::uniform_int_distribution<hash>(1ULL, ULLONG_MAX);
    engine = RandomlySeededMersenneTwister();
  };

  inline hash Get_Hash_Value(square s, piece p, color c) const {
    return zorbist_hash_lookup[s][p][c];
  }

  hash GetRandomValue() { return this->dist(this->engine); };

private:
  hash zorbist_hash_lookup[64][7][3] = {0};
  unsigned int seed_data[624] = {0};
  void FillHashLookup();
  std::mt19937 RandomlySeededMersenneTwister() {
    // Magic number 624: The number of unsigned ints the MT uses as state
    auto seed_data = Panzer::Utils::GetSeedData();
    std::seed_seq seeds(seed_data.begin(), seed_data.end());
    std::mt19937 seededEngine(seeds);
    return seededEngine;
  }
};

} // namespace Panzer
