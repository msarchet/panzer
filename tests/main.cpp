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
#include "com.h"

bool SortMoves(Panzer::Move one, Panzer::Move two);

bool SortMoves(Panzer::Move one, Panzer::Move two)
{
    return one.m_score > two.m_score;
}

int main()
{
    Panzer::InitEvalData();
    Panzer::Com::OpenDebugFile();
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

    std::cout << "Move sort test" << std::endl;
    Panzer::Move moves[256];
    auto move1 = Panzer::Move();
    move1.m_score = 10;
    move1.id = 1;
    moves[0] = move1;
    auto move2 = Panzer::Move();
    move2.m_score = 20;
    move2.id = 2;
    moves[1] = move2;
    std::sort(moves, moves + 10, Panzer::Utils::MoveSorter);
    assert(moves[0].id == 2);
    std::cout << "Move sort test end" << std::endl;

    board = new Panzer::Board();
    board->FenToBoard("rnbqkbnr/pppp1ppp/4p3/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - - 1");
    std::cout << Panzer::EvaluateBoard(*board) << std::endl;

    std::cout << "c6d5" << std::endl;
    board->FenToBoard("3r4/6k1/8/3p1R2/8/3Q2N1/8/5K2 w - - - 1");
    std::cout << Panzer::EvaluateBoard(*board) << std::endl;

    std::cout << "d8d5" << std::endl;
    board->FenToBoard("8/6k1/2p5/3r1R2/8/3Q2N1/8/5K2 w - - - 1");
    std::cout << Panzer::EvaluateBoard(*board) << std::endl;

    int bestMoveIndex = 2;
    int swapTest[4] = { 2, 1, 3, 4 };
    if (bestMoveIndex != 0)
    {
        int temp = swapTest[0];
        swapTest[0] = swapTest[bestMoveIndex];
        for (int i = 1; i <= bestMoveIndex; i++)
        {
            int swap = swapTest[i];
            swapTest[i] = temp;
            temp = swap;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        std::cout << swapTest[i] << std::endl;
    }

}