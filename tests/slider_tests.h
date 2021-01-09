#pragma once
#include "../sliding-attacks/sliders.h"

namespace Panzer
{
    class SlidingTests
    {
        Sliders *sliders;
        public:
            SlidingTests()
            {
                sliders = new Sliders();
            };

            void Setup();
            bool CanInitialize();
            bool ValidateBishopAttacks();
            bool ValidateRookAttacks();
            bool ValidateQueenAttacks();
        private:
            bitboard GenerateBoard(std::vector<square> squares);
    };

}