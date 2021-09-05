#include "bitboard.h"
#include <chrono>
#include <future>

int main (int argc, char *argv[])
{

    std::chrono::time_point<std::chrono::steady_clock> start,end;
    auto futures = std::make_shared<std::vector<std::shared_future<std::size_t>>>();
    auto iterations = 16;

	auto board = new Panzer::Board_Bit();
	board->FillSquare(A2, PAWN, WHITE);
	board->FillSquare(B2, PAWN, WHITE);
	board->FillSquare(C2, PAWN, WHITE);
	board->FillSquare(D2, PAWN, WHITE);
	board->FillSquare(E2, PAWN, WHITE);
	board->FillSquare(F2, PAWN, WHITE);
	board->FillSquare(G2, PAWN, WHITE);
	board->FillSquare(H2, PAWN, WHITE);
	//std::cout << "White Pieces\n";
	//board->PrintBoard(board->GetWhitePieces());
	//std::cout << "White Pawns\n";
	//board->PrintBoard(board->GetWhitePawns());
	//std::cout << "Occupied\n";
	//board->PrintBoard(board->GetOccupancy());

	board->FillSquare(A7, PAWN, BLACK);
	board->FillSquare(B7, PAWN, BLACK);
	board->FillSquare(C7, PAWN, BLACK);
	board->FillSquare(D7, PAWN, BLACK);
	board->FillSquare(E7, PAWN, BLACK);
	board->FillSquare(F7, PAWN, BLACK);
	board->FillSquare(G7, PAWN, BLACK);
	board->FillSquare(H7, PAWN, BLACK);

	//std::cout << "Black Pieces\n";
	//board->PrintBoard(board->GetBlackPieces());
	//std::cout << "Black Pawns\n";
	//board->PrintBoard(board->GetBlackPawns());
	//std::cout << "Occupied\n";
	//board->PrintBoard(board->GetOccupancy());

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++)
    {

        futures->push_back(std::async(std::launch::async, [&board] { 
            auto moves = board->GenerateWhiteMoves();
			for (auto it = moves->begin(); it != moves->end(); it++)
			{
				auto move = *it;
				board->MakeMove(move);
				board->UnmakeMove(move);
			}

			return moves->size();
		}));
    }

    end = std::chrono::high_resolution_clock::now();
	uint64_t total_moves = 0;
	for (auto it = futures->begin(); it != futures->end(); it++)
	{
		auto fut = *it;
		total_moves += fut.get();
	}
	std::chrono::duration<double> elapsed_seconds = end - start; 
    std::cout << elapsed_seconds.count() << "\n";
	std::cout << total_moves << "\n";
    std::cout << "MPS" << total_moves / elapsed_seconds.count();
}