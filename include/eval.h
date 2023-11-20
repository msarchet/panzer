#pragma once

#include "bitboard.h"
#include "bitboard_constants.h"
#include "piece_square_scores.h"

namespace Panzer {
// adjustments from CPW-Engine
const unsigned long long TOTAL_PHASE = 256;
const unsigned long long HALF_PHASE = TOTAL_PHASE / 2;
const unsigned long PAWN_PHASE = 2;
const unsigned int QUEEN_PHASE = 44;
const unsigned int ROOK_PHASE = 16;
const unsigned int BISHOP_PHASE = 12;
const unsigned int KNIGHT_PHASE = 6;

int EvaluateBoard(const Board &board);
template <color c> int EvaluateRooks(const Board &board);
template <color c> int EvaluateBishops(const Board &board);
template <color c> int EvaluateKnights(const Board &board);
template <color c> int EvaluateQueens(const Board &board);
template <color c> int EvaluatePawns(const Board &board);
template <color c> int EvaluateKing(const Board &board);
template <color c> int EvaluateControl(const Board &board);
void InitEvalData();
} // namespace Panzer