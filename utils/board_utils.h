#pragma once
#include <iostream>

#include "../bitboard_constants.h"

namespace Panzer
{
    class Utils
    {
        public:
            static void PrintBoard(bitboard board);
			static std::shared_ptr<std::array<unsigned int, 624>> GetSeedData();
    };
}