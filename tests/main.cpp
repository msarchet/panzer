#include <memory>
#include <iostream>
#include <vector>
#include <cassert>
#include <chrono>
#include <algorithm>
#include "slider_tests.h"
#include "check_tests.h"
#include "bitboard.h"
#include "bitboard_constants.h"

#include "eval.h"
#include "search.h"
bool SortMoves(Panzer::Move one, Panzer::Move two);

bool SortMoves(Panzer::Move one, Panzer::Move two)
{
    return one.m_score > two.m_score;
}

int main()
{
    Panzer::InitEvalData();
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
    capture_move = Panzer::Move(A1, A2, CASTLE, EMPTY_CASTLE_FLAGS, ROOK);
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

    board->FenToBoard(STARTFEN);

    std::chrono::time_point<std::chrono::high_resolution_clock> start,end;

    start = std::chrono::high_resolution_clock::now();

    end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start; 
    std::cout.precision(5);
    std::cout << elapsed_seconds.count() << std::endl;

    auto e2e4 = Panzer::Move(E2, E4, DOUBLE_PAWN_PUSH, ALL_CASTLE_FLAGS, NO_PIECE, E4, 0);
    std::cout << Panzer::EvaluateBoard(*board);
    board->MakeMove(e2e4);
    std::cout << Panzer::EvaluateBoard(*board);
}