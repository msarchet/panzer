#include <chrono>
#include <memory>
#include <future>
#include <sstream>
#include <string>
#include "bitboard.h"
#include "utils/board_utils.h"

long CountMovesRecursive(Panzer::Board_Bit &board, int depth, bool isTopDepth);
std::string OutputBoard(bitboard board);
struct MoveCapture
{
    Panzer::Move move;
    long count;
};

std::string OutputBoard(bitboard board)
{
    std::string out = "\n";

    for (int row = 7; row >= 0; --row) 
    {
        for (int col = 0; col <= 7; ++col) 
        {
            if (board & (1ULL << ((row * 8) + col))) 
            {
                out += "1 ";
            } else 
            {
                out += "0 ";
            }
        }

        out += "\n";
    }

    return out;
}

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

    const auto board = new Panzer::Board_Bit();
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

    total_count = CountMovesRecursive(*board, depth, true);
    end = std::chrono::high_resolution_clock::now();
    //std::cout << "MOVES\n";
	std::chrono::duration<double> elapsed_seconds = end - start; 
    std::cout << elapsed_seconds.count() << "\n";
    std::cout << "total moves " << total_count << "\n";
    std::cout << "CPS" << total_count / elapsed_seconds.count();
}

long CountMovesRecursive(Panzer::Board_Bit &board, int depth, bool isTopDepth)
{
    if (depth == 0)
    {
        return 1;
    }

    auto moves = board.GenerateMoves();

    if (isTopDepth)
    {
        long totalCount = 0;
        auto futures = std::make_shared<std::vector<std::shared_future<std::string> > >();
        futures->reserve(moves->size());
        for (auto it = moves->begin(); it != moves->end(); it++)
        {
            auto captured = *it;
            const auto move = Panzer::Move(captured);
            const auto newBoard = new Panzer::Board_Bit(board);
            auto furtherDepth = depth - 1;
            futures->push_back(
            std::async(std::launch::async, [move, furtherDepth, &totalCount, newBoard] {
                newBoard->MakeMove(move);
                long legalCount = 0;

                if (!newBoard->IsChecked(newBoard->GetSideToMove() == WHITE ? BLACK : WHITE))
                {
                    legalCount = CountMovesRecursive(*newBoard, furtherDepth, false);
                }
                auto output = std::string(squareToString[move.getFrom()]) + std::string(squareToString[move.getTo()]) + std::string(": ") + std::to_string(legalCount);
                totalCount += legalCount;
                return output;
            }));
        }

        for (auto f = futures->begin(); f != futures->end(); f++)
        {
            auto resolved = *f;
            std::cout << f->get() << std::endl;
        }

        return totalCount;
    }
    else
    {
        auto legalCount = 0;

        for (auto it = moves->begin(); it != moves->end(); it++)
        {
            auto move = *it;
            board.MakeMove(move);

            if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
            {
                legalCount += CountMovesRecursive(board, depth - 1, false);
            }

            board.UnmakeMove(move);

        }

        return legalCount;
    }

    //std::cout << "-------" << std::endl;

    return 0;
}