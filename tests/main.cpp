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

    Panzer::Move moves[10];
    auto move1 = Panzer::Move();
    move1.m_score = 10;
    move1.id = 1;
    moves[0] = move1;
    auto move2 = Panzer::Move();
    move2.m_score = 20;
    move2.id = 2;
    moves[1] = move2;
    std::cout << (moves + 1)->id << moves[1].id << std::endl;
    int nums[5] = { 1, 2, 3, 5, 4};
    std::sort(moves, moves + 10, Panzer::Utils::MoveSorter);
    for (int i = 0; i < 10; i++)
    {
        std::cout << moves[i].id << std::endl;
    }
    std::cout << move2.id << std::endl;
    assert(moves[0].id == 2);
    assert(flags == ALL_CASTLE_FLAGS);
    board->FenToBoard(KIWIPETE);

    board->FenToBoard(STARTFEN);

    std::chrono::time_point<std::chrono::high_resolution_clock> start,end;

    std::chrono::duration<double> elapsed_seconds = end - start; 
    std::cout.precision(5);
    std::cout << elapsed_seconds.count() << std::endl;

    board = new Panzer::Board();
    board->FillSquare(G1, KING, WHITE);
    board->FillSquare(F1, ROOK, WHITE);
    board->FillSquare(B1, KNIGHT, WHITE);

    std::cout << Panzer::EvaluateBoard(*board) <<std::endl;

    board = new Panzer::Board();
    board->FillSquare(E1, KING, WHITE);
    board->FillSquare(H1, ROOK, WHITE);
    board->FillSquare(C3, KNIGHT, WHITE);

    std::cout << Panzer::EvaluateBoard(*board) <<std::endl;

    board = new Panzer::Board();
    std::cout << "hash test" << std::endl;
    board->FenToBoard(STARTFEN);
    auto beforeHash = board->GetHash();
    std::cout << beforeHash << std::endl;
    auto e2e4 = Panzer::Move(A2, A4, DOUBLE_PAWN_PUSH, ALL_CASTLE_FLAGS);
    board->MakeMove(e2e4);
    std::cout << board->GetHash() << std::endl;
    board->UnmakeMove(e2e4);
    assert(beforeHash == board->GetHash());

    auto entry = Panzer::TTTable.Find(beforeHash, A1, A2);
    assert(entry.score == Panzer::TT_INVALID);

    Panzer::TTTable.Insert(beforeHash, A1, A2, 2, 25);
    entry = Panzer::TTTable.Find(beforeHash, A1, A2);
    assert(entry.score == 25);
    assert(entry.depth == 2);

    Panzer::TTTable.Insert(beforeHash, A1, A2, 1, 40);
    entry = Panzer::TTTable.Find(beforeHash, A1, A2);
    assert(entry.score == 25);
    assert(entry.depth == 2);
}