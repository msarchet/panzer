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
	        static inline int GetLSB(bitboard b) { return __builtin_ctzll(b); }
            static inline int GetPopcount(bitboard b) { return __builtin_popcountll(b); }
    };
}