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
		int16_t AlphaBeta(Panzer::Board &board, int16_t alpha, int16_t beta, int depth);
		int16_t Quiesence(Panzer::Board &board, int16_t alpha, int16_t beta);
		int16_t SEE(Panzer::Board &board, square to);
		void Search(Panzer::Board &board, int depth);
		void SearchIterate(Panzer::Board &board, int depth);
		bool IsDrawByRepition(hash key);
		void AddHashToRepition(hash key);
		void ClearRepitionHash();
	}

	static const int16_t TT_INVALID = INT16_MIN;
	const int NUM_TT_ENTRIES = 10000;
	static const unsigned char MIN = 0;
	static const unsigned char MAX = 1;
	static const unsigned char EXACT = 2;

	struct TTEntry
	{
		int16_t score = TT_INVALID;
		unsigned char depth = 0;
		unsigned char type = 0;
	};


	struct TT
	{
		TTEntry entries[NUM_TT_ENTRIES] = { TTEntry() };

		void Insert(hash boardHash, unsigned char depth, int16_t score, unsigned char type)
		{
			int key = static_cast<int16_t>(boardHash % NUM_TT_ENTRIES);
			auto entry = entries[key];
			if (depth < entry.depth) return;
			entry.depth = depth;
			entry.score = score;
			entry.type = type;
			entries[key] = entry;
		}

		void Clear()
		{
			std::fill_n(entries, NUM_TT_ENTRIES, TTEntry());
		}

		TTEntry Find(hash boardHash) 
		{
			int key = static_cast<int16_t>(boardHash % NUM_TT_ENTRIES);
			return entries[key];
		}
	};

	static TT TTTable;
}