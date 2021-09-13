#include <chrono>
#include <memory>

#include "bitboard.h"
#include "utils/board_utils.h"

long CountMovesRecursive(Panzer::Board_Bit *board, int depth);

int main (int argc, char *argv[])
{
	int depth = 4;
	if (argc > 1) 
	{
		depth = argv[1][0] - '0';
	}

    auto board = new Panzer::Board_Bit();
    board->FillSquare(A1, ROOK, WHITE);
    board->FillSquare(B1, KNIGHT, WHITE);
    board->FillSquare(C1, BISHOP, WHITE);
    board->FillSquare(D1, QUEEN, WHITE);
    board->FillSquare(E1, KING, WHITE);
    board->FillSquare(F1, BISHOP, WHITE);
    board->FillSquare(G1, KNIGHT, WHITE);
    board->FillSquare(H1, ROOK, WHITE);
    board->FillSquare(A2, PAWN, WHITE);
    board->FillSquare(B2, PAWN, WHITE);
    board->FillSquare(C2, PAWN, WHITE);
    board->FillSquare(D2, PAWN, WHITE);
    board->FillSquare(E2, PAWN, WHITE);
    board->FillSquare(F2, PAWN, WHITE);
    board->FillSquare(G2, PAWN, WHITE);
    board->FillSquare(H2, PAWN, WHITE);

    board->FillSquare(A7, PAWN, BLACK);
    board->FillSquare(B7, PAWN, BLACK);
    board->FillSquare(C7, PAWN, BLACK);
    board->FillSquare(D7, PAWN, BLACK);
    board->FillSquare(E7, PAWN, BLACK);
    board->FillSquare(F7, PAWN, BLACK);
    board->FillSquare(G7, PAWN, BLACK);
    board->FillSquare(H7, PAWN, BLACK);
    board->FillSquare(A8, ROOK, BLACK);
    board->FillSquare(B8, KNIGHT, BLACK);
    board->FillSquare(C8, BISHOP, BLACK);
    board->FillSquare(D8, QUEEN, BLACK);
    board->FillSquare(E8, KING, BLACK);
    board->FillSquare(F8, BISHOP, BLACK);
    board->FillSquare(G8, KNIGHT, BLACK);
    board->FillSquare(H8, ROOK, BLACK);



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

        board->UnmakeMove(move);
    }

    std::cout << "-------" << std::endl;

    return totalCount;
}