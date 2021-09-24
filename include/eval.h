#pragma once

#include "bitboard.h"
#include "piece_square_scores.h"
#include "bitboard_constants.h"

namespace Panzer
{
	// adjustments from CPW-Engine
	const piece_score KNIGHT_ADJUSTMENTS[9] = { -20, -16, -12, -8, -4, 0, 4, 8 , 12 };
	const piece_score ROOK_ADJUSTMENTS[9] = { 15, 12, 9, 6, 3, 0, -3, -6, -9 };

	int EvaluateBoard(Board &board);
	void InitEvalData();
}