#include <memory>
#include <iostream>
#include <vector>
#include <cassert>
#include "slider_tests.h"
#include "check_tests.h"
#include "bitboard.h"
#include "bitboard_constants.h"

int main()
{
    auto slider_tests = new Panzer::SlidingTests();
    std::cout << "Running Tests" << std::endl;
    std::cout << "DERP";
    slider_tests->Setup();
    slider_tests->ValidateBishopAttacks();
    slider_tests->ValidateRookAttacks();
    auto check_tests = new Panzer::CheckTests();
    check_tests->ValidateCheckLogic();
    std::array<bitboard, 64> normalMethod = {0ULL};

    for (square s = A1; s <= H8; s++)
    {
       bitboard knight_span = KNIGHT_SPAN;
       square from = s;
       if (from >= KNIGHT_SPAN_CENTER)
       {
           knight_span = knight_span << (from - KNIGHT_SPAN_CENTER);
       }
       else
       {
           knight_span = knight_span >> (KNIGHT_SPAN_CENTER - from);
       }

       knight_span &= knight_move_masks[from % 8];
       normalMethod[s] = knight_span;
    } 

    for (square s = A1; s <= H8; s++)
    {
       if (KNIGHT_SPANS[s] != normalMethod[s])
       {
           std::cout << "unmatched bitboards" << std::endl;
           std::cout << "normal method" << std::endl;
           Panzer::Utils::PrintBoard(normalMethod[s]);
           std::cout << "new" << std::endl;
           Panzer::Utils::PrintBoard(KNIGHT_SPANS[s]);
       }
    }
}