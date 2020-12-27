#include "bitboard.cpp"
#include <chrono>
#include <memory>

int main (int argc, char *argv[])
{
    auto board = new Panzer::Board_Bit();
    board->FillSquare(B1, KNIGHT, WHITE);
    board->FillSquare(G1, KNIGHT, WHITE);
    board->FillSquare(A2, PAWN, WHITE);
    board->FillSquare(B2, PAWN, WHITE);
    board->FillSquare(C2, PAWN, WHITE);
    board->FillSquare(D2, PAWN, WHITE);
    board->FillSquare(E2, PAWN, WHITE);
    board->FillSquare(F2, PAWN, WHITE);
    board->FillSquare(G2, PAWN, WHITE);
    board->FillSquare(H2, PAWN, WHITE);
    std::cout << "White Pieces\n";
    board->PrintBoard(board->GetWhitePieces());
    std::cout << "White Pawns\n";
    board->PrintBoard(board->GetWhitePawns());
    std::cout << "Occupied\n";
    board->PrintBoard(board->GetOccupancy());

    board->FillSquare(A7, PAWN, BLACK);
    board->FillSquare(B7, PAWN, BLACK);
    board->FillSquare(C7, PAWN, BLACK);
    board->FillSquare(D7, PAWN, BLACK);
    board->FillSquare(E7, PAWN, BLACK);
    board->FillSquare(F7, PAWN, BLACK);
    board->FillSquare(G7, PAWN, BLACK);
    board->FillSquare(H7, PAWN, BLACK);

    std::cout << "Black Pieces\n";
    board->PrintBoard(board->GetBlackPieces());
    std::cout << "Black Pawns\n";
    board->PrintBoard(board->GetBlackPawns());
    std::cout << "Occupied\n";
    board->PrintBoard(board->GetOccupancy());

    std::chrono::time_point<std::chrono::steady_clock> start,end;
    auto moves = board->GenerateWhiteMoves();
    std::cout << moves->size() << "\n";
    uint32_t total_count = 0;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1e6; i++)
    {
        moves = board->GenerateWhiteMoves();
        for (auto it = moves->begin(); it != moves->end(); it++)
        {                
            board->MakeMove(*it);
            board->UnmakeMove(*it);
            total_count++;
        }
    }

    const auto a2a4 = std::make_shared<Panzer::Move>(A2, A4, PAWN, NO_PIECE, NO_PIECE, A4, NO_SQUARE, EMPTY_MOVE_FLAGS, EMPTY_MOVE_FLAGS, 1);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "MOVES\n";
	std::chrono::duration<double> elapsed_seconds = end - start; 
    std::cout << elapsed_seconds.count() << "\n";
    std::cout << "total moves " << total_count << "\n";
    std::cout << "MPS" <<total_count / elapsed_seconds.count();
}