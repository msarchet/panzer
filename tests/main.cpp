#include <memory>
#include <iostream>
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
    auto board = new Panzer::Board_Bit();
    //board->FenToBoard(STARTFEN);
    board->FillSquare(B2, PAWN, WHITE);
    //MoveVector moves = std::make_shared<std::vector<std::shared_ptr<const Panzer::Move>>>();
    //const auto a2a4 = std::make_shared<Panzer::Move>
    //(
    //    B2,
    //    B4,
    //    DOUBLE_PAWN_PUSH,
    //    EMPTY_CASTLE_FLAGS,
    //    PAWN
    //);

    //moves->push_back(a2a4);

    ////board->MakeMove(a2a4);

    //const auto a7a6 = std::make_shared<Panzer::Move>
    //(
    //    A7,
    //    A6,
    //    NO_MOVE_FLAGS,
    //    EMPTY_CASTLE_FLAGS,
    //    PAWN
    //);

    //board->MakeMove(a7a6);
    auto moves = board->GenerateWhiteMoves();
    for (auto it = moves->begin(); it != moves->end(); it++)
    {
        auto move = *it;
        Panzer::Utils::PrintBoard(board->GetWhitePawns());
        std::cout << squareToString[move->getFrom()] << std::endl;
        std::cout << squareToString[move->getTo()] << std::endl;
        board->MakeMove(move);
        board->UnmakeMove(move);
        Panzer::Utils::PrintBoard(board->GetWhitePawns());
        std::cout << "next move" << std::endl;
    }
    std::cout << moves->size() << std::endl; 
    std::cout << "Done!";
}