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
    auto board = new Panzer::Board();
    board->FenToBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q2/1PPBBPpP/1R2K2R b Kkq - 0 2");
    auto move = Panzer::Move(G2, H1, KNIGHT_PROMO_CAPTURE, ALL_CASTLE_FLAGS, ROOK);
    assert(move.isPromo());
    board->MakeMove(move);
    board->PrintBoard();
    Panzer::Utils::PrintBoard(board->GetWhiteKnights());
    board->UnmakeMove(move);
    board->PrintBoard();
    Panzer::Utils::PrintBoard(board->GetWhiteKnights());
}