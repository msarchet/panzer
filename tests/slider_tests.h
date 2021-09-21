#pragma once
#include "sliders.h"

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
            bool ValidateIsChecked();
        private:
            bitboard GenerateBoard(std::vector<square> squares);
    };

}