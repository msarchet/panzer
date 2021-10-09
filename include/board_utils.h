#pragma once
#include <iostream>

#include "bitboard_constants.h"

namespace Panzer
{

    class Utils
    {
        public:
            static void PrintBoard(bitboard board);
			static std::shared_ptr<std::array<unsigned int, 624>> GetSeedData();
            static void SortMoves(Move* moves, int movecount);
            static bool MoveSorter(Panzer::Move move_one, Panzer::Move move_two);
            static std::string PrintMove(Panzer::Move move);
#ifdef _MSC_VER
            static inline int GetLSB(bitboard b) { 
                unsigned long index = 0;
                _BitScanForward64(&index, b);
                return index;
            }

            static inline int GetPopcount(bitboard b) { return __popcnt64(b); }
            static inline bitboard SwapBits(bitboard b) { return _byteswap_uint64(b); }
#else
	        static inline int GetLSB(bitboard b) { return __builtin_ctzll(b); }
            static inline int GetPopcount(bitboard b) { return __builtin_popcountll(b); }
            static inline bitboard SwapBits(bitboard b) { return __builtin_bswap64(b); }
#endif

    };
}