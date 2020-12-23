// Panzer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <iomanip> // put_time
#include <sstream>

#include "constants.h"
#include "board.h"
#include "name_lookups.h"
typedef Panzer::Board board;

bool isAttacked(const square& square, board* board);
std::shared_ptr<const move> build_move(square from_square, square to_square, piece from, piece captured, char flags, piece to_piece, square ep, board* b);

void make_move(std::shared_ptr<const move>, board* board);
void unmake_move(std::shared_ptr<const move>, board* board);
void generate_moves(std::shared_ptr<std::vector<std::shared_ptr<const move>>> moves, board * board);
void perft(int depth, board* b, std::shared_ptr<std::vector<std::shared_ptr<const move>>> moves);
unsigned long perft_raw(int depth, board* b);

void print_move(move m);
size_t sum_perft_nodes(board* b, int depth);
void hash_start_board(board* b);

std::ofstream outfile;
std::unordered_map<hash, std::shared_ptr<hashed_board>>* hashed_boards = new std::unordered_map<hash, std::shared_ptr<hashed_board>>();
int main(int argc, char *argv[])
{
	// Using time point and system_clock 
    std::chrono::time_point<std::chrono::steady_clock> start, end; 
	int depth = 6;
	if (argc > 1) 
	{
		depth = argv[1][0] - '0';
	}
	auto const now = std::time(NULL);
	std::tm bt{};
	auto start_time = localtime_s(&bt, &now);
	std::stringstream temp;
	temp << std::put_time(&bt, "%F %T");
	std::string filename = "perft-depth" + std::to_string(depth) + ".log";
	board* b = new board;
	auto moves = std::make_shared<std::vector<std::shared_ptr<const move>>>();
	hashed_board* starting_hashboard = new hashed_board();
  
	std::cout << "Running\n";
	auto problem_fen = "8/p7/8/1P6/K1k3p1/6P1/7P/8";
	b->FenToBoard(problem_fen);
	std::string fen = b->BoardToFen();
	b->SetSideToMove(WHITE);
	b->SetEPSquare(NONE);
	b->SetCastleFlags(NO_FLAGS);
	hash_start_board(b);
	hash starting_hash = b->Zorbist();

	outfile.open(filename, std::ios_base::app);

	outfile << "Peft depth " << depth << "\n";
	outfile << "Starting Hash" << starting_hash << "\n";

	//auto b5b6 = build_move(B5, B6, PAWN, NONE, NO_FLAGS, NONE, NONE, b);
	//make_move(b5b6, b);
	//auto a7a6 = build_move(A7, A6, PAWN, NONE, NO_FLAGS, NONE, NONE, b);
	//make_move(a7a6, b);
	//auto b6b7 = build_move(B6, B7, PAWN, NONE, NO_FLAGS, NONE, NONE, b);
	//make_move(b6b7, b);
	//auto c4d5 = build_move(C4, D5, KING, NONE, NO_FLAGS, NONE, NONE, b);
	//make_move(c4d5, b);
	uint32_t legal_count = 0;
	start = std::chrono::high_resolution_clock::now();
	generate_moves(moves, b);
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		auto move = *it;
		make_move(move, b);
		if (!isAttacked(b->KingSquare(b->SideToMove() != WHITE ? WHITE : BLACK), b))
		{
			legal_count++;
			std::cout << Panzer::Utils::square_names[move->from] << Panzer::Utils::square_names[move->to] << ": ";
			outfile << Panzer::Utils::square_names[move->from] << Panzer::Utils::square_names[move->to] << ": ";
			if (move->flags & PROMOTION)
			{
				std::cout << Panzer::Utils::piece_name(move->piece_to);
			}
			auto count = perft_raw(depth - 1, b);
			std::cout << count << "\n";
			outfile << count << "\n";
		}
		unmake_move(move, b);
	}
		
	end = std::chrono::high_resolution_clock::now();
	starting_hashboard->moves = moves;
	outfile << "Ending Hash" << b->Zorbist() << "\n";
	std::chrono::duration<double> elapsed_seconds = end - start; 
	outfile << "Time:" << elapsed_seconds.count();
	outfile << "\n";
	outfile << "Perft" << depth << " " << legal_count;
	outfile << "Moves PS: " << legal_count / elapsed_seconds.count() << "\n";

	outfile << "TOTAL MOVES" << legal_count << "\n";
	std::cout << "Moves PS: " << legal_count / elapsed_seconds.count() << "\n";
	std::cout << "TOAL MOVES" << legal_count << "\n";
	std::cout << "DONE" << "\n";
	std::cout << b->BoardToFen();
	std::cin.get();
}
size_t sum_perft_nodes(board* b, int depth)
{
	size_t sum = 0;
	auto it = hashed_boards->find(b->Zorbist());
	//print_line_with_depth(std::string("found hashboard with hash ") + std::to_string(b->zorbist), 5 - depth);
	if (it == hashed_boards->end())
	{
		outfile.flush();
		assert(false);
	}
	auto hashed(it->second);

	if (depth == 1)
	{
		return hashed->moves->size();
	}

	for (auto mi = hashed->moves->begin(); mi != hashed->moves->end(); mi++)
	{
		auto move = *mi ;
		make_move(move, b);
		sum += sum_perft_nodes(b, depth - 1);
		unmake_move(move, b);
	}

	return sum;
}

void hash_start_board(board* b) 
{
}

std::string print_move_chain(std::shared_ptr<std::vector<std::string>> moves)
{
	std::stringstream stream;
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		stream << *it << "->";
	}

	stream << "\n";
	return stream.str();
}

std::shared_ptr<std::vector<std::string>>  move_chain = std::make_shared<std::vector<std::string>>();
unsigned long perft_raw(int depth, board* b)
{
	unsigned long sum = 0;
	auto moves = std::make_shared<std::vector<std::shared_ptr<const move>>>();
	generate_moves(moves, b);
	
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		auto m = *it;
		make_move(m, b);

		//move_chain->push_back(square_names[m->from] + square_names[m->to]);
		if (!isAttacked(b->KingSquare(b->SideToMove() == WHITE ? BLACK : WHITE), b))
		{
			//outfile << print_move_chain(move_chain);
			if (depth != 1)
			{
				unsigned long node = perft_raw(depth - 1, b);
				//outfile << print_move_chain(move_chain);
				//outfile << "DEPTH" << depth <<" SUM " << node;
				//outfile << "\n";

				sum += node;
			}
			else
			{
				sum++;
			}
		}
		else 
		{
			//outfile << "illegal move"<< square_names[m->from] << square_names[m->to] << " --" << depth << "\n";
		}
		unmake_move(m, b);
		//move_chain->erase(move_chain->end() - 1);
	}
	return sum;
}

void perft(int depth, board* b, std::shared_ptr<std::vector<std::shared_ptr<const move>>> moves) 
{

	//auto it = hashed_boards->find(b->zorbist);
	//if (it != hashed_boards->end())
	//{
	//	const hashed_board* board_from_hash = it->second;
	//	// we will hash boards and not hash moves for them because they are 
	//	//	at the last ply
	//	if (board_from_hash->moves->size() != 0 or board_from_hash->isCheckmate)
	//	{
	//		moves = board_from_hash->moves;
	//		return;
	//	}
	//}

	auto raw_moves = std::make_shared<std::vector<std::shared_ptr<const move>>>();
	generate_moves(raw_moves, b);
	
	for (auto it = raw_moves->begin(); it != raw_moves->end(); it++)
	{
		auto raw_move(*it);
		make_move(raw_move, b);
		int king_index = b->KingSquare(b->SideToMove() == WHITE ? BLACK : WHITE);

		if (!isAttacked(king_index, b))
		{
			// insert move into moves
			// if this is a recursive lookup that moves is deep_moves
			moves->push_back(raw_move);

			// if were still going down the tree
			if (depth != 1) 
			{
				auto deep_moves = std::make_shared<std::vector<std::shared_ptr<const move>>>();
				// store the board state from the current move
				// TODO: move to constructor
				perft(depth - 1, b, deep_moves);
				//print_line_with_depth(std::to_string(hashed->hash), 0);
				//print_line_with_depth(board_to_fen(b), 0);
				if (hashed_boards->count(b->Zorbist()) == 0)
				{
					auto hashed = std::make_shared<hashed_board>();
					hashed->hash = b->Zorbist();
					hashed->moves = deep_moves;
					hashed_boards->insert({ hashed->hash, hashed });
				}
				else
				{
					//print_line_with_depth("HASH COLLISION for board (hash on prior line): ", 0);
					//print_line_with_depth(board_to_fen(b), 0);
					//print_line_with_depth("Move Was \n", 0);
					//print_line_with_depth(square_names[raw_move->from] + square_names[raw_move->to] + " " + std::to_string(b->ply) + " ", 0);
				}
			}
		}

		unmake_move(raw_move, b);
		//outfile << "hash after unmake move" << b->zorbist << "\n";
	}
}

void print_move(move m) 
{
#ifdef DEBUG
	std::cout << square_names[m.from] << square_names[m.to];
#endif
}

void make_move(std::shared_ptr<const move> m, board* board) 
{
	board->SetPly(board->Ply() + 1);
	if (board->SideToMove() == WHITE)
	{
		auto whitePieces = board->GetPieces(WHITE);
		whitePieces->erase(m->from);
		whitePieces->insert(m->to);
		if (m->flags & CAPTURE)
		{
			auto blackPieces = board->GetPieces(BLACK);
			if (m->flags & EP)
			{
				blackPieces->erase(board->EPSquare());
			}
			else
			{
				blackPieces->erase(m->to);
			}
		}
		if (m->piece_from == KING) 
		{
			board->SetKingSquare(m->to, WHITE);
		}
	}
	else
	{
		auto blackPieces = board->GetPieces(BLACK);
		blackPieces->erase(m->from);
		blackPieces->insert(m->to);
		if (m->flags & CAPTURE)
		{
			auto whitePieces = board->GetPieces(WHITE);
			if (m->flags & EP)
			{
				whitePieces->erase(board->EPSquare());
			}
			else
			{
				whitePieces->erase(m->to);
			}
		}
		if (m->piece_from == KING) 
		{
			board->SetKingSquare(m->to, BLACK);
		}
	}

	board->ToggleColorHash();
	// remove from piece
	board->ClearSquare(m->from);

	// clear to piece
	if (CAPTURE & m->flags)
	{
		if (m->flags & EP) 
		{
			// remove hash of EP piece
			board->ClearSquare(board->EPSquare());
		}
		else
		{
			board->ClearSquare(m->to);
		}
	}

	// add moved piece
	board->FillSquare(m->to, m->piece_to, board->SideToMove());

	// adjust castling flags
	if (board->CastleFlags()) 
	{
		if (m->from == E1 || m->to == E1) 
		{
			board->ToggleCastleMoves(WHITEK | WHITEQ);
		}
		else if (m->from == A1 || m->to == A1)
		{
			board->ToggleCastleMoves(WHITEQ);
		}
		else if (m->from == H1 || m->to == H1)
		{
			board->ToggleCastleMoves(WHITEK);
		}
		else if (m->from == E8 || m->to== E8) 
		{
			board->ToggleCastleMoves(BLACKK | BLACKQ);
		}
		else if (m->from == A8 || m->to == A8)
		{
			board->ToggleCastleMoves(BLACKQ);
		}
		else if (m->from == H8 || m->to == H8)
		{
			board->ToggleCastleMoves(BLACKK);
		}
	}

	if (m->flags & CASTLE)
	{
		switch (m->to)
		{
			case C1:
				// move rook
				board->ClearSquare(A1);
				board->FillSquare(D1, ROOK, WHITE);
				break;
			case G1:
				// move rook
				board->ClearSquare(H1);
				board->FillSquare(F1, ROOK, WHITE);
				break;
			case C8:
				// move rook
				board->ClearSquare(A8);
				board->FillSquare(D8, ROOK, BLACK);
				break;
			case G8:
				// move rook
				board->ClearSquare(H8);
				board->FillSquare(F8, ROOK, BLACK);
				break;
		}
	}

	board->SetEPSquare(m->ep);
	board->SetSideToMove(board->SideToMove() == BLACK ? WHITE : BLACK);
}

void unmake_move(std::shared_ptr<const move> m, board* board) 
{
	board->SetPly(board->Ply() - 1);
	board->SetSideToMove(board->SideToMove() == BLACK ? WHITE : BLACK);
	// remove to piece
	board->ClearSquare(m->to);
	board->SetEPSquare(m->prior_ep);
	if (CAPTURE & m->flags) 
	{
		color captured_color = board->SideToMove() == WHITE ? BLACK : WHITE;
		if (EP & m->flags) 
		{
			square ep = board->EPSquare();
			board->FillSquare(ep, m->capture, captured_color);
		}
		else
		{
			board->FillSquare(m->to, m->capture, captured_color);
		}
	}


	// add back from piece
	board->FillSquare(m->from, m->piece_from, board->SideToMove());
	board->ToggleColorHash();

	if (board->SideToMove() == WHITE)
	{
		auto whitePieces = board->GetPieces(WHITE);
		whitePieces->insert(m->from);
		whitePieces->erase(m->to);
		if (m->flags & CAPTURE)
		{
			auto blackPieces = board->GetPieces(BLACK);
			if (m->flags & EP)
			{
				blackPieces->insert(board->EPSquare());
			}
			else
			{
				blackPieces->insert(m->to);
			}
		}
		if (m->piece_from == KING) 
		{
			board->SetKingSquare(m->from, WHITE);
		}
	}
	else
	{
		auto blackPieces = board->GetPieces(BLACK);
		blackPieces->insert(m->from);
		blackPieces->erase(m->to);
		if (m->flags & CAPTURE)
		{
			auto whitePieces = board->GetPieces(WHITE);
			if (m->flags & EP)
			{
				whitePieces->insert(board->EPSquare());
			}
			else
			{
				whitePieces->insert(m->to);
			}
		}
		if (m->piece_from == KING)
		{
			board->SetKingSquare(m->from, BLACK);
		}
	}
	// adjust castling flags
	// do things here if move is castle
	if (m->flags & CASTLE) 
	{
		// check m->to square to revert a castle
		switch (m->to)
		{
			case C1:
				// move rook
				board->ClearSquare(D1);
				board->FillSquare(A1, ROOK, WHITE);
				break;
			case G1:
				// move rook
				board->ClearSquare(F1);
				board->FillSquare(H1, ROOK, WHITE);
				break;
			case C8:
				// move rook
				board->ClearSquare(D8);
				board->FillSquare(A8, ROOK, BLACK);
				break;
			case G8:
				// move rook
				board->ClearSquare(F8);
				board->FillSquare(H8, ROOK, BLACK);
				break;
		}
	}
	board->SetCastleFlags(m->prior_castle);
	}

bool isAttacked(const square& square, board* board) 
{
	color moving_color = board->GetColorAt(square);
	color attack_color = NO_COLOR;

	if (moving_color == WHITE) 
	{
		attack_color = BLACK;
	}
	else if (moving_color == BLACK) 
	{
		attack_color = WHITE;
	}

	for (int ray: knight_moves) 
	{
		const int target = square + ray;
		
		if (IS_SQ(target) and board->GetColorAt(target) == attack_color) 
		{
			if (board->GetPieceAt(target) == KNIGHT) 
			{
				return true;
			}
		}
	}

	for (int ray : rays) 
	{
		int target = square + ray;
		bool in_pawn_range = true;
		while (IS_SQ(target))
		{
			if (board->GetColorAt(target) == moving_color)
			{
				break;
			}
			else if (board->GetColorAt(target) == attack_color)
			{
				piece target_piece = board->GetPieceAt(target);
				if (in_pawn_range && target_piece == PAWN) 
				{	
					if (attack_color == WHITE 
						&& (ray == SW || ray == SE)) 
					{
						return true;
					}

					if (attack_color == BLACK
						&& (ray == NW || ray == NE)) 
					{
						return true;
					}
				}

				if (in_pawn_range && target_piece == KING) 
				{
					return true;
				}

				if (ray == NW || ray == NE || ray == SE || ray == SW)
				{
					if (target_piece & (BISHOP | QUEEN))
					{
						return true;
					}
				}
				else 
				{
					if (target_piece & (QUEEN | ROOK))
					{
						return true;
					}
				}
				break;
			}

			// move along the ray
			target += ray;
			in_pawn_range = false;
		}
	}

	return false;
}


void generate_moves(std::shared_ptr<std::vector<std::shared_ptr<const move>>> moves, board* board)
{
	char castle_moves = board->CastleFlags();
	color same_side = board->SideToMove();
	color other_side = board->SideToMove()== WHITE ? BLACK : WHITE;

	if (same_side == WHITE)
	{
		if (castle_moves & WHITEK)
		{
			if (board->GetPieceAt(B1) == NONE and board->GetPieceAt(C1) == NONE and board->GetPieceAt(D1) == NONE)
			{
				if (!isAttacked(C1, board) and !isAttacked(D1, board))
				{
					moves->push_back(build_move(E1, C1, KING, NONE, CASTLE, NONE, NONE, board));
				}
			}
		}
		if (castle_moves & WHITEQ)
		{
			if (board->GetPieceAt(F1) == NONE and board->GetPieceAt(G1) == NONE)
			{
				if (!isAttacked(F1, board) and !isAttacked(G1, board))
				{
					moves->push_back(build_move(E1, G1, KING, NONE, CASTLE, NONE, NONE, board));
				}
			}
		}

	}
	else if (same_side == BLACK)
	{
		if (castle_moves & BLACKK)
		{
			if (board->GetPieceAt(B8) == NONE and board->GetPieceAt(C8) == NONE and board->GetPieceAt(D8) == NONE)
			{
				if (!isAttacked(C8, board) and !isAttacked(D8, board))
				{
					moves->push_back(build_move(E8, C8, KING, NONE, CASTLE, NONE, NONE, board));
				}
			}
		}
		if (castle_moves & BLACKQ)
		{
			if (board->GetPieceAt(F8) == NONE and board->GetPieceAt(G8) == NONE)
			{
				if (!isAttacked(F8, board) and !isAttacked(G8, board))
				{
					moves->push_back(build_move(E8, G8, KING, NONE, CASTLE, NONE, NONE, board));
				}
			}
		}
	}

	std::shared_ptr<std::unordered_set<piece>> same_side_pieces = board->GetPieces(same_side);

	for (auto it = same_side_pieces->begin(); it != same_side_pieces->end(); it++)
	{
		const square square_index = *it;
		piece current_piece = board->GetPieceAt(square_index);
		square current_target = square_index;
		bool slides = (current_piece & (PAWN | KNIGHT | KING)) != current_piece;

		int move_rays[8] = {0,0,0,0,0,0,0};
		int ray_count = 8;
		if (current_piece & BISHOP) 
		{
			move_rays[0] = NW;
			move_rays[1] = NE;
			move_rays[2] = SW;
			move_rays[3] = SE;
			ray_count = 4;
		}

		if (current_piece & (KING | QUEEN))
		{
			for (int i = 0; i < 8; i++) 
			{
				move_rays[i] = rays[i];
			}
		}

		if (current_piece & ROOK) 
		{
			move_rays[0] = N;
			move_rays[1] = S;
			move_rays[2] = W;
			move_rays[3] = E;
			ray_count = 4;
		}

		if (current_piece & KNIGHT) 
		{
			for (int i = 0; i < 8; i++) 
			{
				move_rays[i] = knight_moves[i];
			}

		}
		if (current_piece & PAWN) 
		{
			if (same_side == WHITE) 
			{
				if (square_index / 16 == 1) 
				{
					current_target = square_index + NN;
					if (IS_SQ(current_target) and board->GetPieceAt(current_target) == NONE && board->GetPieceAt(current_target + S) == NONE) 
					{
						moves->push_back(build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, NONE, current_target, board));
					}
				}

				if (square_index / 16 == 4)
				{
					if ((square_index - 1 == board->EPSquare() || square_index + 1 == board->EPSquare()) and board->GetColorAt(board->EPSquare()) != same_side)
					{
						moves->push_back(build_move(square_index, board->EPSquare() + N, PAWN, PAWN, EP | CAPTURE, NONE, NONE, board));
					}
				}

				current_target = square_index + N;
				if (IS_SQ(current_target) && board->GetPieceAt(current_target) == NONE) 
				{
					if (current_target / 16 == 7) 
					{
						for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
						{
							auto m = build_move(square(square_index), square(current_target), PAWN, NONE, PROMOTION, promote, NONE, board);
							moves->push_back(m);
						}
					}
					else
					{
						moves->push_back(build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, NONE, NONE, board));
					}
				}

				int pawn_captures[] = { square_index + NW, square_index + NE };
				for (int capture_index: pawn_captures) 
				{
					if (IS_SQ(capture_index) and board->GetPieceAt(capture_index) != NONE && board->GetColorAt(capture_index) != same_side) 
					{
						if (capture_index / 16 == 7) 
						{
							for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
							{
								auto m = build_move(square(square_index), square(capture_index), PAWN, board->GetPieceAt(capture_index), (CAPTURE | PROMOTION), promote, NONE, board);
								moves->push_back(m);
							}
						}
						else
						{
							moves->push_back(build_move(square(square_index), square(capture_index), PAWN, board->GetPieceAt(capture_index), CAPTURE, NONE, NONE, board));
						}
					}
				}
				
			}

			if (same_side == BLACK) 
			{
				if (square_index / 16 == 6) 
				{
					current_target = square_index + SS;
					if (IS_SQ(current_target) and board->GetPieceAt(current_target) == NONE && board->GetPieceAt(current_target + N) == NONE) 
					{
						moves->push_back(build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, NONE, current_target, board));
					}
				}

				if (square_index / 16 == 3)
				{
					if ((square_index - 1 == board->EPSquare() || square_index + 1 == board->EPSquare()) and board->GetColorAt(board->EPSquare()) != same_side)
					{
						moves->push_back(build_move(square_index, board->EPSquare() + S, PAWN, PAWN, EP | CAPTURE, NONE, NONE, board));
					}
				}

				current_target = square_index + S;
				if (IS_SQ(current_target) and board->GetPieceAt(current_target) == NONE) 
				{
					if (current_target / 16 == 0) 
					{
						for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
						{
							auto m = build_move(square(square_index), square(current_target), PAWN, NONE, PROMOTION, promote, NONE, board);
							moves->push_back(m);
						}
					}
					else
					{
						moves->push_back(build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, NONE,NONE, board));
					}
				}

				int pawn_captures[] = { square_index + SW, square_index + SE };
				for (int capture_index: pawn_captures) 
				{
					if (IS_SQ(capture_index) and board->GetPieceAt(capture_index) != NONE && board->GetColorAt(capture_index) != same_side) 
					{
						if (capture_index/ 16 == 7) 
						{
							for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
							{
								auto m = build_move(square(square_index), square(capture_index), PAWN, board->GetPieceAt(capture_index), (CAPTURE | PROMOTION), promote, NONE, board);
								moves->push_back(m);
							}
						}
						else
						{
							moves->push_back(build_move(square(square_index), square(capture_index), PAWN, board->GetPieceAt(capture_index), CAPTURE, NONE, NONE, board));
						}
					}
				}
			}
		}
		else 
		{
			for (int ray_index = 0; ray_index < ray_count; ray_index++) 
			{
				int dir = move_rays[ray_index];
				current_target = square_index + dir;
				while (IS_SQ(current_target)) 
				{
					if (board->GetColorAt(current_target) == same_side) 
					{
						break;
					}

					if (board->GetPieceAt(current_target) == NONE) 
					{
						moves->push_back(build_move(square(square_index), square(current_target), current_piece, NONE, NO_FLAGS, NONE, NONE, board));
					}
					else
					{
						moves->push_back(build_move(square(square_index), square(current_target), current_piece, board->GetPieceAt(current_target), CAPTURE, NONE, NONE, board));
						break;
					}

					if (!slides)
					{
						break;
					}

					current_target += dir;
				}
			}

		}
	}
}

std::shared_ptr<const move> build_move(square from_square,
	square to_square,
	piece from,
	piece captured,
	char flags,
	piece to_piece,
	square ep,
	board* b) 
{
	std::shared_ptr<const move> m = std::make_shared<move>(
		from_square,
		to_square,
		from,
		to_piece != NONE ? to_piece : from,
		captured,
		flags,
		ep,
		b->EPSquare(),
		b->CastleFlags(),
		b->Ply());
	return m;
}
