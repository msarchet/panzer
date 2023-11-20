#pragma once
#include "bitboard_constants.h"
#include <fstream>
#include <random>
#include <vector>

namespace Panzer {
struct Magics {
  magics_array magics;
  std::array<std::vector<bitboard>, 64> attacks;
  mask_array masks;
  std::array<int, 64> shifts;
};

class Sliders {
public:
  Sliders();
  const bitboard GetRookAttacks(const square s, const bitboard occupancy) const;
  const bitboard GetBishopAttacks(const square s,
                                  const bitboard occupancy) const;
  const bitboard GetQueenAttacks(const square s,
                                 const bitboard occupancy) const;

private:
  class MagicGenerator {
  public:
    void MakeMagic(Magics &rookMagic, Magics &bishopMagic);

  private:
    enum Directions { N, NE, E, SE, S, SW, W, NW };
    class Direction {
      Directions d;

    public:
      Direction(Directions dir) : d(dir) {}

      Directions GetDirection() { return d; }
      square GetNextSquare(square start);
      uint8_t DistanceToEdge(square s);
      std::vector<bitboard> GenerateOccupancies(square s);
      bitboard GenerateAttacks(bitboard occupancy, square s);
      bool IsBoardEdge(Directions d, square s);

    private:
      uint8_t Row(square s);
      uint8_t Column(square s);
      uint8_t Inverse(uint8_t value);
    };

    std::vector<bitboard> CombineBitboards(const std::vector<bitboard> &left,
                                           const std::vector<bitboard> &right);

    bitboard CreateMask(const std::vector<Direction> &directions, square s);

    void MakeGenerator();
    uint64_t random64();
    std::uniform_int_distribution<uint64_t> dist;
    std::mt19937 engine;
    void GenerateMagic(const std::vector<Direction> &directions,
                       Magics &magics);
  };

  const int GetAttackIndex(bitboard occupancy, mask occupancy_mask, mask magic,
                           int shift) const;

  void Initialize();
  MagicGenerator mMagicGenerator;
  Magics mRookMagics;
  Magics mBishopMagics;
  bool mInitialized = false;
};
extern const Sliders SliderLookup;
} // namespace Panzer