#include <chrono>
#include <memory>

#include "bitboard.h"
#include "utils/board_utils.h"

long CountMovesRecursive(Panzer::Board_Bit *board, int depth);

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


    std::chrono::time_point<std::chrono::high_resolution_clock> start,end;

    long total_count = 0;

    start = std::chrono::high_resolution_clock::now();
    
    total_count = CountMovesRecursive(board, depth);
    end = std::chrono::high_resolution_clock::now();
    //std::cout << "MOVES\n";
	std::chrono::duration<double> elapsed_seconds = end - start; 
    std::cout << elapsed_seconds.count() << "\n";
    std::cout << "total moves " << total_count << "\n";
    std::cout << "CPS" << total_count / elapsed_seconds.count();
}

long CountMovesRecursive(Panzer::Board_Bit *board, int depth)
{
    auto moves = board->GenerateMoves();

    long currentCount = 0;

    if (depth == 1)
    {
        currentCount = moves->size();
        for (auto it = moves->begin(); it != moves->end(); it++)
        {
            auto move = *it;
            std::cout << squareToString[move->getFrom()] << squareToString[move->getTo()] << std::endl;
        }
        std::cout << "\t" << currentCount << std::endl;
        return currentCount;
    }

    long totalCount = 0;
    for (auto it = moves->begin(); it != moves->end(); it++)
    {
        auto move = *it;
        board->MakeMove(move);

        std::cout << squareToString[move->getFrom()] << squareToString[move->getTo()] << std::endl;
        if (!board->IsChecked(board->GetSideToMove()))
        {
            currentCount = CountMovesRecursive(board, depth - 1);
            std::cout << currentCount << std::endl;
            totalCount += currentCount; 
        }
        else
        {
            std::cout << "checked";
        }

        board->UnmakeMove(move);
    }

    std::cout << "-------" << std::endl;

    return totalCount;
}