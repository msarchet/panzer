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
	template <color c> void EvaluatePawns(const Board &board, EvalScores &scores);
	template <color c> void MinorBehindPawns(const Board& board, EvalScores &scores);
	template <color c> void EvaluateOutposts(const Board &board, EvalScores &scores);

	template <color c> void EvaluateControl(const Board &board, EvalScores &scores);

	template <color us, color them> void BishopXRayPawns(const Board &board, EvalScores &scores);
	template <color us, color them> void RookScores(const Board &board, EvalScores &scores);
	template <color us, color them> void WeakQueen(const Board &board, EvalScores &scores);
	template <color c> void Mobility(const Board &board, EvalScores &scores);
	template <color c> void Attacks(const Board& board, int* attacks);
	template <color c> int SpaceArea(const Board& board, int* AttackLookup);
	template <color c> void Space(const Board& board, EvalScores &scores, int spaceArea);
	template <color c> void WeakAndHanging(int *attacksUs, int *attacksThem, int *weakLookup);
	void InitEvalData();
}