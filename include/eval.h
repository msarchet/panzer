#pragma once

#include "bitboard.h"
#include "bitboard_constants.h"
#include "piece_square_scores.h"

namespace Panzer {
// adjustments from CPW-Engine
const int TOTAL_PHASE = 256;
const int PAWN_PHASE = 2;
const int QUEEN_PHASE = 44;
const int ROOK_PHASE = 16;
const int BISHOP_PHASE = 12;
const int KNIGHT_PHASE = 6;

int EvaluateBoard(Board &board);
template <color c> int EvaluateRooks(const Board &board);
template <color c> int EvaluateBishops(const Board &board);
template <color c> int EvaluateKnights(const Board &board);
template <color c> int EvaluateQueens(const Board &board);
template <color c> int EvaluatePawns(const Board &board);
template <color c> int EvaluateKing(const Board &board);
template <color c> int EvaluateControl(const Board &board);
void InitEvalData();
} // namespace Panzer