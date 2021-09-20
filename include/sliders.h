#pragma once
#include "bitboard_constants.h"
#include <fstream>
#include <vector>

namespace Panzer
{
    class Sliders
    {
        magics_array rook_magics;
        std::shared_ptr<std::array<std::shared_ptr<std::vector<bitboard> >, 64> > rook_attacks;
        mask_array rook_masks;
        int rook_shifts[64];

        magics_array bishop_magics;
        std::shared_ptr<std::array<std::shared_ptr<std::vector<bitboard> >, 64> > bishop_attacks;
        mask_array bishop_masks;
        int bishop_shifts[64];

        public:
            void Initialize();
            bitboard GetRookAttacks(square s, bitboard occupancy);
            bitboard GetBishopAttacks(square s, bitboard occupancy);
            bitboard GetQueenAttacks(square s, bitboard occupancy);
        private:
            int GetAttackIndex(bitboard occupancy, mask occupancy_mask, mask magic, int shift) 
            {
                return ((occupancy & occupancy_mask) * magic) >> (64 - shift);
            }
    };
}