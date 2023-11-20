#pragma once
#include <iostream>

#include "bitboard_constants.h"

namespace Panzer {

class Utils {
public:
  static void PrintBoard(bitboard board);
  static std::array<unsigned int, 624> GetSeedData();
  static void SortMoves(Panzer::Move *moves, int movecount);
  static bool MoveSorter(const Panzer::Move &move_one,
                         const Panzer::Move &move_two);
  static std::string PrintMove(Panzer::Move move);
  static int GetLSB(bitboard b);
};
} // namespace Panzer