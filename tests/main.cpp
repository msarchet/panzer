#include <memory>
#include <iostream>
#include <vector>
#include "slider_tests.h"
#include "check_tests.h"
#include "../bitboard.h"
#include "../bitboard_constants.h"

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
    auto board = Panzer::Board_Bit();
    board.FenToBoard("rn1qkbnr/2pppppp/bp6/8/8/4PN2/PPPP1PPP/RNBQK2R w KQkq - 0 4");
    bool isAttacked = board.IsSquareAttacked(F1, WHITE);
    std::cout << (isAttacked ? "true" : "false");
}