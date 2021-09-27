#pragma once

#include "bitboard_constants.h"
#include "eval.h"
#include "piece_square_scores.h"

namespace Panzer
{
	namespace Search
	{
		int16_t AlphaBetaMinMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth);
		int16_t AlphaBetaMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth);
		int16_t AlphaBetaMin(Panzer::Board &board, int16_t alpha, int16_t beta, int depth);
		int16_t Quiesence(Panzer::Board &board, int16_t alpha, int16_t beta);
		void Search(Panzer::Board &board, int depth);
		void SearchIterate(Panzer::Board &board, int depth);
	}
}