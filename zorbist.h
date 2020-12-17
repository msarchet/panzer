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

			hash Get_Hash_Value(square s, piece p, color c) 
			{
				return zorbist_hash_lookup->at(s)->at(p)->at(c);
			}

		private:
			std::unordered_map<square, std::unordered_map<piece, std::unordered_map<color, hash>*>*>* zorbist_hash_lookup = new std::unordered_map<square, std::unordered_map<piece, std::unordered_map<color, hash>*>*>();

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
					auto piece_hash = new std::unordered_map<piece, std::unordered_map<color, hash>*>();
					zorbist_hash_lookup->insert({ s, piece_hash});
					if (IS_SQ(s)) 
					{
						for (piece p : { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, NONE})
						{
							auto c_h = new std::unordered_map<color, hash>();
							piece_hash->insert({ p, c_h });
							for (color c : { WHITE, BLACK, NO_COLOR})
							{
								c_h->insert({ c ,dist(engine) });
							}
						}
					}
				}

				color_hash = dist(engine);
		}
	};
}

