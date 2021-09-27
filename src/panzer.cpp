#include <chrono>
#include <memory>
#include <future>
#include <sstream>
#include <string>
#include <tuple>

#include "bitboard.h"
#include "board_utils.h"
#include "search.h"
#include "com.h"
#include "eval.h"

uint64_t CountMovesRecursive(Panzer::Board &board, int depth, bool isTopDepth);
void ProcessInputs();
std::string GetNextToken(std::string &line, std::string delimeter);
std::ofstream debug_file;

int main (int argc, char *argv[])
{
    Panzer::InitEvalData();
    Panzer::Com::OpenDebugFile();

    Panzer::Com::SendMessageToUI("Hello welcome to Panzer, created by Michael Sarchet");
    ProcessInputs();
    Panzer::Com::CloseDebugFile();
    return 0;
}

void ProcessInputs()
{
    std::string line;
    std::string token;
    std::string delimeter = " ";
    bool exit = false;

    const auto board = new Panzer::Board();
    board->FenToBoard(STARTFEN);

    while (!exit)
    {
        std::getline(std::cin, line);
        token = GetNextToken(line, delimeter);

        if (token == "moves")
        {
            token = GetNextToken(line, delimeter);

            if (token == "startpos")
            {
                board->FenToBoard(STARTFEN);
                token = GetNextToken(line, delimeter);
            }

            if (token == "kiwi")
            {
                board->FenToBoard(KIWIPETE);
                token = GetNextToken(line, delimeter);
            }

            while (!token.empty())
            {
                if (token.length() < 4)
                {
                    std::cout << token;
                    break;
                }

                auto from = stringToSquare(token.substr(0, 2));
                auto to = stringToSquare(token.substr(2, 2));
                std::string promotion = "";
                if (token.length() == 5)
                {
                    promotion = token.substr(4, 1);
                }
                Panzer::Move moves[256];
                auto movecount = board->GenerateMoves(moves);
                auto foundMove = Panzer::Move(A1, A2, EMPTY_CASTLE_FLAGS, EMPTY_CASTLE_FLAGS);
                auto foundValidMove = false;
                if (movecount == 0) break;
                for (auto i = 0; i < movecount && !foundValidMove; i++)
                {
                    auto possibleMove = moves[i];
                    if (possibleMove.getFrom() == from && possibleMove.getTo() == to)
                    {
                        if (promotion.empty())
                        {
                            foundValidMove = true;
                        }
                        else
                        {
                            if (!possibleMove.isPromo())
                            {
                                continue;
                            }
                            if (promotion == "q" && (possibleMove.getFlags() & QUEEN_PROMO))
                            {
                                foundValidMove = true;
                            }
                            if (promotion == "b" && (possibleMove.getFlags() & BISHOP_PROMO))
                            {
                                foundValidMove = true;
                            }
                            if (promotion == "n" && (possibleMove.getFlags() & KNIGHT_PROMO))
                            {
                                foundValidMove = true;
                            }
                            if (promotion == "r" && (possibleMove.getFlags() & ROOK_PROMO))
                            {
                                foundValidMove = true;
                            }
                        }

                        if (foundValidMove)
                        {
                            foundMove = possibleMove; 
                        }
                    }
                }

                if (foundValidMove)
                {
                    board->MakeMove(foundMove);
                }
                else
                {
                    break;
                }

                token = GetNextToken(line, delimeter);
            }
        }

        if (token == "perft")
        {
            std::cout << "Running perft" << std::endl;
            token = GetNextToken(line, delimeter);
            auto depth = std::stoi(token);
            uint64_t total_count = 0;
            const auto perftBoard = new Panzer::Board(*board);

            std::chrono::time_point<std::chrono::high_resolution_clock> start,end;

            start = std::chrono::high_resolution_clock::now();

            total_count = CountMovesRecursive(*perftBoard, depth, true);

            end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> elapsed_seconds = end - start; 
            std::cout.precision(5);
            std::cout << elapsed_seconds.count() << std::endl;
            std::cout << "total moves " << total_count << std::endl;
            std::cout.precision(0);
            std::cout << "NPS: " << std::fixed << total_count / elapsed_seconds.count() << std::endl;

        }

        if (token == "fen")
        {
            auto fen = line;
            board->FenToBoard(fen);
        }

        if (token == "print")
        {
            board->PrintBoard();
            std::cout << board->BoardToFen();
        }

        if (token == "search")
        {
            auto searchBoard = Panzer::Board(*board);
            int depth = 2;
            token = GetNextToken(line, " ");
            if (!token.empty())
            {
                depth = std::stoi(token);
            }

            Panzer::Search::Search(searchBoard, depth);
        }

        if (token == "quit" || token == "q")
        {
            std::cout << "Goodbye!" << std::endl;
            exit = true;
        }

        if (token == "debug")
        {
            Panzer::Com::SetDebug(true);
        }
    }

    return;
}

std::string GetNextToken(std::string &line, std::string delimeter)
{
    size_t pos;
    std::string token;
    if((pos = line.find(delimeter)) != std::string::npos)
    {
        token = line.substr(0, pos);
        line.erase(0, pos + delimeter.length());
    }
    else if (line.size() != 0)
    {
        token = line;
        line.erase(0, line.size());
    }

    return token;
}

uint64_t CountMovesRecursive(Panzer::Board &board, int depth, bool isTopDepth)
{
    if (depth == 0)
    {
        return 1;
    }

    Panzer::Move moves[256];
    auto movecount = board.GenerateMoves(moves);

    if (movecount == 0) return 0;
    if (isTopDepth)
    {
        uint64_t totalCount = 0;
        auto futures = std::make_shared<std::vector<std::shared_future<std::tuple<std::string, uint64_t> > > >();
        futures->reserve(movecount);
        for (auto i = 0; i < movecount; i++)
        {
            auto captured = moves[i];
            const auto move = Panzer::Move(captured);
            const auto newBoard = new Panzer::Board(board);
            auto furtherDepth = depth - 1;
            futures->push_back(
            std::async(std::launch::async, [move, furtherDepth, newBoard] {
                newBoard->MakeMove(move);

                uint64_t legalCount = 0;
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
            auto count = std::get<uint64_t>(result);
            if (count != 0)
            {
                std::cout << std::get<std::string>(result) << std::endl;
                totalCount += count;
            }
        }

        return totalCount;
    }
    else
    {
        uint64_t legalCount = 0ULL;

        for (auto i = 0; i < movecount; i++)
        {
            auto move = moves[i];
            board.MakeMove(move);

            uint64_t subCount = 0ULL;
            if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
            {
                subCount = CountMovesRecursive(board, depth - 1, false);
                legalCount += subCount;
            }

            board.UnmakeMove(move);

        }

        return legalCount;
    }

    return 0ULL;
}