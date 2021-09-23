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

    if (QUEEN_PROMO & QUEEN_PROMO)
    {
        assert(true);
    }

    if (QUEEN_PROMO & NO_MOVE_FLAGS)
    {
        assert(false);
    }

    auto capture_move = Panzer::Move(A1, A2, CAPTURE, EMPTY_CASTLE_FLAGS, ROOK);
    assert(capture_move.isCapture());
    assert(!capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(!capture_move.isCastle());
    capture_move = Panzer::Move(A1, A2, QUEEN_PROMO_CAPTURE, EMPTY_CASTLE_FLAGS, ROOK);
    assert(capture_move.isCapture());
    assert(capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(!capture_move.isCastle());
    capture_move = Panzer::Move(A1, A2, ROOK_PROMO_CAPTURE, EMPTY_CASTLE_FLAGS, ROOK);
    assert(capture_move.isCapture());
    assert(capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(!capture_move.isCastle());
    capture_move = Panzer::Move(A1, A2, BISHOP_PROMO_CAPTURE, EMPTY_CASTLE_FLAGS, ROOK);
    assert(capture_move.isCapture());
    assert(capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(!capture_move.isCastle());
    capture_move = Panzer::Move(A1, A2, KNIGHT_PROMO_CAPTURE, EMPTY_CASTLE_FLAGS, ROOK);
    assert(capture_move.isCapture());
    assert(capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(!capture_move.isCastle());
    capture_move = Panzer::Move(A1, A2, NO_MOVE_FLAGS, EMPTY_CASTLE_FLAGS, ROOK);
    assert(!capture_move.isCapture());
    assert(!capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(!capture_move.isCastle());
    capture_move = Panzer::Move(A1, A2, EP_CAPTURE, EMPTY_CASTLE_FLAGS, ROOK);
    assert(capture_move.isCapture());
    assert(!capture_move.isPromo());
    assert(capture_move.isEPCapture());
    assert(!capture_move.isCastle());
    capture_move = Panzer::Move(A1, A2, QUEEN_CASTLE, EMPTY_CASTLE_FLAGS, ROOK);
    assert(!capture_move.isCapture());
    assert(!capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(capture_move.isCastle());

    capture_move = Panzer::Move(A1, A2, QUEEN_PROMO, EMPTY_CASTLE_FLAGS, ROOK);
    assert(!capture_move.isCapture());
    assert(capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(!capture_move.isCastle());
    capture_move = Panzer::Move(A1, A2, ROOK_PROMO, EMPTY_CASTLE_FLAGS, ROOK);
    assert(!capture_move.isCapture());
    assert(capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(!capture_move.isCastle());
    capture_move = Panzer::Move(A1, A2, BISHOP_PROMO, EMPTY_CASTLE_FLAGS, ROOK);
    assert(!capture_move.isCapture());
    assert(capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(!capture_move.isCastle());
    capture_move = Panzer::Move(A1, A2, KNIGHT_PROMO, EMPTY_CASTLE_FLAGS, ROOK);
    assert(!capture_move.isCapture());
    assert(capture_move.isPromo());
    assert(!capture_move.isEPCapture());
    assert(!capture_move.isCastle());

    castle_flag flags = EMPTY_CASTLE_FLAGS;
    flags |= BLACKK;
    flags |= BLACKQ;
    flags |= WHITEK;
    flags |= WHITEQ;

    assert(flags == ALL_CASTLE_FLAGS);
    board->FenToBoard(KIWIPETE);
}