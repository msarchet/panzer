#include "bitboard.cpp"
#include <chrono>
#include <memory>

int main (int argc, char *argv[])
{
    const auto board = new Panzer::Board_Bit();
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

    auto copyBoard = new Panzer::Board_Bit(*board);
    copyBoard->ClearSquare(A2, PAWN, WHITE);

    std::cout << "First Board\n";
    board->PrintBoard(board->GetOccupancy());
    auto moves = board->GenerateBlackMoves();
    std::cout << moves->size() << "\n";

    std::chrono::time_point<std::chrono::steady_clock> start,end;
    //uint32_t total_count = 0;
    for (auto it = moves->begin(); it != moves->end(); it++)
    {
        std::cout << int((*it)->from) << " " << int((*it)->to) << "\n";
    }

    start = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i < 1e6; i++)
    //{
    //    auto copyBoard = new Panzer::Board_Bit(*board);
    //    //moves = board->GenerateWhiteMoves();
    //    //for (auto it = moves->begin(); it != moves->end(); it++)
    //    //{                
    //    //    board->MakeMove(*it);
    //    //    board->UnmakeMove(*it);
    //    //    total_count++;
    //    //}
    //}

    //const auto a2a4 = std::make_shared<Panzer::Move>(A2, A4, PAWN, NO_PIECE, NO_PIECE, A4, NO_SQUARE, EMPTY_MOVE_FLAGS, EMPTY_MOVE_FLAGS, 1);
    end = std::chrono::high_resolution_clock::now();
    //std::cout << "MOVES\n";
	std::chrono::duration<double> elapsed_seconds = end - start; 
    std::cout << elapsed_seconds.count() << "\n";
    //std::cout << "total moves " << total_count << "\n";
    std::cout << "CPS" << 1e6 / elapsed_seconds.count();
}