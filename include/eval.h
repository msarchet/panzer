#pragma once

#include "bitboard.h"
#include "piece_square_scores.h"
#include "bitboard_constants.h"

namespace Panzer
{
	// adjustments from CPW-Engine
	const int TOTAL_PHASE = 256;
	const int PAWN_PHASE = 2;
	const int QUEEN_PHASE = 44;
	const int ROOK_PHASE = 16;
	const int BISHOP_PHASE = 12;
	const int KNIGHT_PHASE = 6;

	int EvaluateBoard(Board &board);
	int EvaluateRooks(Board &board, color c);
	int EvaluateBishops(Board &board, color c);
	int EvaluateKnights(Board &boardd, color c);
	int EvaluateQueens(Board &board, color c);
	int EvaluatePawns(Board &board, color c);
	int EvaluateKing(Board &board, color c);
	int EvaluateControl(Board &board, color c);
	void InitEvalData();
}