#pragma once
#include "bitboard_constants.h"
#include "utils/board_utils.h"
#include <random>
#include <cmath>
#include <climits>

namespace Panzer
{
	class zorbist_lookup
	{
		public:
			hash color_hash = 0;
			std::uniform_int_distribution<unsigned long long> dist;
			std::mt19937 engine;

			zorbist_lookup()
			{
				Fill_Hash_Lookup();
				dist = std::uniform_int_distribution<hash>(1ULL, ULLONG_MAX);
				engine = RandomlySeededMersenneTwister();
			};

			hash Get_Hash_Value(square s, piece p, color c) { return zorbist_hash_lookup[s][p][c]; }
			hash GetRandomValue()
			{
				return this->dist(this->engine);
			};

		private:
			hash zorbist_hash_lookup[64][7][3] = { 0 };
			unsigned int seed_data[624] = { 0 };

			std::mt19937 RandomlySeededMersenneTwister () {
				// Magic number 624: The number of unsigned ints the MT uses as state
				auto seed_data = Panzer::Utils::GetSeedData();
				std::seed_seq seeds(seed_data->begin(), seed_data->end());
				std::mt19937 seededEngine (seeds);
				return seededEngine;
			}

			void Fill_Hash_Lookup()
			{

				for (square s = 0; s < 64; s++)
				{
					for (piece p : { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, NO_PIECE})
					{
						for (color c : { WHITE, BLACK, NO_COLOR})
						{
							zorbist_hash_lookup[s][p][c] = dist(engine);
						}
					}
				}

				color_hash = dist(engine);
			}
	};

}
 