#pragma once
#include "constants.h"

namespace Panzer
{
	class zorbist_lookup
	{
		public:
			hash color_hash = 0;
			zorbist_lookup()
			{
				Fill_Hash_Lookup();
			};

			hash Get_Hash_Value(square s, piece p, color c) { return zorbist_hash_lookup[s][p][c]; }

		private:
			hash zorbist_hash_lookup[128][65][3] = { 0 };

			auto RandomlySeededMersenneTwister () {
				// Magic number 624: The number of unsigned ints the MT uses as state
				std::vector<unsigned int> random_data(624);
				std::random_device source;
				std::generate(begin(random_data), end(random_data), [&](){return source();});
				std::seed_seq seeds(begin(random_data), end(random_data));
				std::mt19937 seededEngine (seeds);
				return seededEngine;
			}

			void Fill_Hash_Lookup()
			{
				auto engine = RandomlySeededMersenneTwister();
				std::uniform_int_distribution<unsigned long long> dist(1, -1LL);

				for (square s = 0; s < 128; s++)
				{
					if (IS_SQ(s)) 
					{
						for (piece p : { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, NONE})
						{
							for (color c : { WHITE, BLACK, NO_COLOR})
							{
								zorbist_hash_lookup[s][p][c] = dist(engine);
							}
						}
					}
				}

				color_hash = dist(engine);
		}
	};
}
 