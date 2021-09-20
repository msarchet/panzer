#include <chrono>
#include <memory>
#include <future>
#include <sstream>
#include <string>
#include <tuple>

#include "bitboard.h"
#include "board_utils.h"

long CountMovesRecursive(Panzer::Board &board, int depth, bool isTopDepth);

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

    const auto board = new Panzer::Board();
    board->FenToBoard(startpos);

    std::chrono::time_point<std::chrono::high_resolution_clock> start,end;

    long total_count = 0;

    start = std::chrono::high_resolution_clock::now();

    total_count = CountMovesRecursive(*board, depth, true);
    end = std::chrono::high_resolution_clock::now();
    //std::cout << "MOVES\n";
	std::chrono::duration<double> elapsed_seconds = end - start; 
    std::cout << elapsed_seconds.count() << "\n";
    std::cout << "total moves " << total_count << "\n";
    std::cout.precision(0);
    std::cout << "CPS" << std::fixed << total_count / elapsed_seconds.count();
}

long CountMovesRecursive(Panzer::Board &board, int depth, bool isTopDepth)
{
    if (depth == 0)
    {
        return 1;
    }

    auto moves = board.GenerateMoves();

    if (isTopDepth)
    {
        long totalCount = 0;
        auto futures = std::make_shared<std::vector<std::shared_future<std::tuple<std::string, long>> > >();
        futures->reserve(moves->size());
        for (auto it = moves->begin(); it != moves->end(); it++)
        {
            auto captured = *it;
            const auto move = Panzer::Move(captured);
            const auto newBoard = new Panzer::Board(board);
            auto furtherDepth = depth - 1;
            futures->push_back(
            std::async(std::launch::async, [move, furtherDepth, newBoard] {
                newBoard->MakeMove(move);
                long legalCount = 0;

                if (!newBoard->IsChecked(newBoard->GetSideToMove() == WHITE ? BLACK : WHITE))
                {
                    legalCount = CountMovesRecursive(*newBoard, furtherDepth, false);
                }
                auto output = std::string(squareToString[move.getFrom()]) + std::string(squareToString[move.getTo()]) + std::string(": ") + std::to_string(legalCount);

                return std::tuple(output, legalCount);
            }));
        }

        for (auto f = futures->begin(); f != futures->end(); f++)
        {
            auto resolved = *f;
            auto result = f->get();
            std::cout << std::get<std::string>(result) << std::endl;
            totalCount += std::get<long>(result);
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