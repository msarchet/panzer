#include <chrono>
#include <memory>

#include "bitboard.h"
#include "utils/board_utils.h"

long CountMovesRecursive(Panzer::Board_Bit *board, int depth, bool isTopDepth);
int main (int argc, char *argv[])
{
	int depth = 4;
    char* startpos = (char*)STARTFEN;
	if (argc > 1) 
	{
		depth = argv[1][0] - '0';
	}

    if (argc > 2)
    {
        startpos = argv[2];
    }

    std::cout << startpos << std::endl;

    auto board = new Panzer::Board_Bit();
    board->FenToBoard(startpos);

    //auto e2e4 = Panzer::Move(E2, E4, DOUBLE_PAWN_PUSH, ALL_CASTLE_FLAGS);
    //auto d7d6 = Panzer::Move(D7, D6, NO_MOVE_FLAGS, ALL_CASTLE_FLAGS);
    //auto e4e5 = Panzer::Move(E4, E5, NO_MOVE_FLAGS, ALL_CASTLE_FLAGS);
    //auto f7f5= Panzer::Move(F7, F5, DOUBLE_PAWN_PUSH, ALL_CASTLE_FLAGS);
    //auto e5f6 = Panzer::Move(E5, F6, EP_CAPTURE, ALL_CASTLE_FLAGS, PAWN);
    //board->MakeMove(e2e4);
    //board->MakeMove(d7d6);
    //board->MakeMove(e4e5);
    //board->MakeMove(f7f5);
    //board->MakeMove(e5f6);
    //board->UnmakeMove(e5f6);
    //board->UnmakeMove(f7f5);
    //board->UnmakeMove(e4e5);
    //board->UnmakeMove(d7d6);
    //board->UnmakeMove(e2e4);
    Panzer::Utils::PrintBoard(board->GetOccupancy());

    std::chrono::time_point<std::chrono::high_resolution_clock> start,end;

    long total_count = 0;

    start = std::chrono::high_resolution_clock::now();

    total_count = CountMovesRecursive(board, depth, true);
    end = std::chrono::high_resolution_clock::now();
    //std::cout << "MOVES\n";
	std::chrono::duration<double> elapsed_seconds = end - start; 
    std::cout << elapsed_seconds.count() << "\n";
    std::cout << "total moves " << total_count << "\n";
    std::cout << "CPS" << total_count / elapsed_seconds.count();
}

long CountMovesRecursive(Panzer::Board_Bit *board, int depth, bool isTopDepth)
{
    if (depth == 0)
    {
        return 1;
    }

    auto moves = board->GenerateMoves();

    long legalCount = 0;
    long totalCount = 0;

    for (auto it = moves->begin(); it != moves->end(); it++)
    {
        auto move = *it;
        board->MakeMove(move);


        if (!board->IsChecked(board->GetSideToMove() == WHITE ? BLACK : WHITE))
        {
            legalCount = CountMovesRecursive(board, depth - 1, false);
            if (isTopDepth)
            {
                std::cout << squareToString[move.getFrom()] << squareToString[move.getTo()] << ": ";
                std::cout << legalCount <<  std::endl;
            }
            totalCount += legalCount; 
        }


        board->UnmakeMove(move);
    }

    //std::cout << "-------" << std::endl;

    return totalCount;
}