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
    const auto board1 = new Panzer::Board_Bit();
    board1->FenToBoard(STARTFEN);
    auto board2 = new Panzer::Board_Bit(*board1);
    auto move = Panzer::Move(A2, A4, DOUBLE_PAWN_PUSH, ALL_CASTLE_FLAGS);
    board1->MakeMove(move);
    board1->PrintBoard(board1->GetOccupancy());
    board2->PrintBoard(board2->GetOccupancy());
}