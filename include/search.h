#pragma once

#include "bitboard_constants.h"
#include "eval.h"
#include "piece_square_scores.h"
#include <set>
#include <climits>

namespace Panzer
{
	namespace Search
	{
		int16_t AlphaBetaMinMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth);
		int16_t AlphaBetaMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth);
		int16_t AlphaBetaMin(Panzer::Board &board, int16_t alpha, int16_t beta, int depth);
		int16_t Quiesence(Panzer::Board &board, int16_t alpha, int16_t beta);
		int16_t SEE(Panzer::Board &board, square to);
		void Search(Panzer::Board &board, int depth);
		void SearchIterate(Panzer::Board &board, int depth);
		bool IsDrawByRepition(hash key);
		void AddHashToRepition(hash key);
		void ClearRepitionHash();
	}

	static const int16_t TT_INVALID = INT16_MIN;
	const int NUM_TT_ENTRIES = 100;
	static const int MIN = 0;
	static const int MAX = 1;

	struct TTEntry
	{
		int16_t score = TT_INVALID;
		int depth;
		int type = 0;
	};


	struct TT
	{
		TTEntry entries[NUM_TT_ENTRIES][64][64] = { TTEntry() };

		void Insert(hash boardHash, square from, square to, int depth, int16_t score)
		{
			int key = static_cast<int>(boardHash % NUM_TT_ENTRIES);
			auto entry = entries[key][from][to];
			if (depth < entry.depth) return;
			entry.depth = depth;
			entry.score = score;
			entries[key][from][to] = entry;
		}

		void Clear()
		{
			entries[NUM_TT_ENTRIES][64][64] = { TTEntry() };
		}

		TTEntry Find(hash boardHash, square from, square to) 
		{
			int key = static_cast<int>(boardHash % NUM_TT_ENTRIES);
			return entries[key][from][to];
		}
	};

	static TT TTTable;
}