#include "sliders.h"
#include <iostream>

namespace Panzer {
Sliders::Sliders() { Initialize(); }
void Sliders::Initialize() {
  if (!mInitialized) {
    std::cout << "Initializing sliders" << std::endl;
    mMagicGenerator.MakeMagic(mRookMagics, mBishopMagics);
    mInitialized = true;
  }
}

const bitboard Sliders::GetRookAttacks(square s, bitboard occupancy) const {
  return mRookMagics.attacks.at(s).at(
      GetAttackIndex(occupancy, mRookMagics.masks.at(s),
                     mRookMagics.magics.at(s), mRookMagics.shifts.at(s)));
}

const bitboard Sliders::GetBishopAttacks(square s, bitboard occupancy) const {
  return mBishopMagics.attacks.at(s).at(
      GetAttackIndex(occupancy, mBishopMagics.masks.at(s),
                     mBishopMagics.magics.at(s), mBishopMagics.shifts.at(s)));
}

const bitboard Sliders::GetQueenAttacks(square s, bitboard occupancy) const {
  return GetRookAttacks(s, occupancy) | GetBishopAttacks(s, occupancy);
}
const int Sliders::GetAttackIndex(bitboard occupancy, mask occupancy_mask,
                                  mask magic, int shift) const {
  return ((occupancy & occupancy_mask) * magic) >> (64 - shift);
}

const Sliders SliderLookup{};
} // namespace Panzer