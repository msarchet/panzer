// Panzer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <assert.h>
#include <iomanip> // put_time
#include <sstream>

#include "constants.h"
#include "zorbist.h"

void fen_to_board(const std::string& fen, board* board);
std::string board_to_fen(board* b);
bool isAttacked(const square& square, board* board);
const move* build_move(square from_square, square to_square, piece from, piece captured, char flags, piece to_piece, board* b);

void make_move(const move* move, board* board);
void unmake_move(const move* move, board* board);
void generate_moves(std::vector<const move*>* moves, board * board);
void perft(int depth, board* b, std::vector<const move*>* moves);
unsigned long perft_raw(int depth, board* b);

void print_move(move m);
size_t sum_perft_nodes(board* b, int depth);
void print_line_with_depth(std::string line, int depth);
void hash_start_board(board* b);
std::ofstream outfile;
Panzer::zorbist_lookup* zorbist = new Panzer::zorbist_lookup();
std::unordered_map<hash, const hashed_board*>* hashed_boards = new std::unordered_map<hash, const hashed_board*>(1000000);
const std::string piece_name(piece p)
{
	switch (p) 
	{
		case NONE: return "";
		case PAWN: return "P";
		case ROOK: return "R";
		case BISHOP: return "B";
		case KNIGHT: return "N";
		case QUEEN: return "Q ";
		case KING: return "K";
	}
	return "";
};


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
	std::vector<const move*> *moves = new std::vector<const move*>();
	hashed_board* starting_hashboard = new hashed_board();
  
	std::cout << "Running\n";
	fen_to_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", b);
	std::string board_as_fen = board_to_fen(b);
	b->side_to_move = WHITE;
	hash_start_board(b);
	hash starting_hash = b->zorbist;

	outfile.open(filename, std::ios_base::app);

	outfile << "Peft depth " << depth << "\n";
	outfile << "Starting Hash" << starting_hash << "\n";

	start = std::chrono::high_resolution_clock::now();
	perft(depth, b, moves);
	end = std::chrono::high_resolution_clock::now();
	starting_hashboard->moves = moves;
	unsigned long total = perft_raw(depth, b);
	//outfile << "Ending Hash" << b->zorbist << "\n";
	//b->zorbist = 0;
	//hash_start_board(b);
	//outfile << "RE: Hash (reset board hash)" << b->zorbist << "\n";
	//hashed_boards->emplace(starting_hash, starting_hashboard);
	//std::cout << "Summing";
	//outfile << "Summing" << "\n";
	//size_t total = sum_perft_nodes(b, depth);

	std::chrono::duration<double> elapsed_seconds = end - start; 
	outfile << "Time:" << elapsed_seconds.count();
	outfile << "\n";
	outfile << "Perft" << depth << " " << total;
	outfile << "Moves PS: " << total / elapsed_seconds.count() << "\n";

	outfile << "TOTAL MOVES" << total << "\n";
	std::cout << "Moves PS: " << total / elapsed_seconds.count() << "\n";
	std::cout << "TOAL MOVES" << total << "\n";
	std::cout << "DONE";
	std::cin.get();
}
size_t sum_perft_nodes(board* b, int depth)
{
	size_t sum = 0;

	auto it = hashed_boards->find(b->zorbist);
	//print_line_with_depth(std::string("found hashboard with hash ") + std::to_string(b->zorbist), 5 - depth);
	if (it == hashed_boards->end())
	{
		//print_line_with_depth(std::string("Failed to find prior hash"), 0);
		outfile.flush();
		assert(false);
	}
	const hashed_board* hashed = it->second;

	if (depth == 1)
	{
		return hashed->moves->size();
	}

	for (auto mi = hashed->moves->begin(); mi != hashed->moves->end(); mi++)
	{
		const move* move = *mi ;
		hash before = b->zorbist;
		//print_line_with_depth(std::string("hash before make move: ") + std::to_string(b->ply) + std::string(" ") + std::to_string(b->zorbist), 5 - depth);
		make_move(move, b);
		sum += sum_perft_nodes(b, depth - 1);
		unmake_move(move, b);
		hash after = b->zorbist;
		//print_line_with_depth(std::string("hash after unmake move: ") + std::to_string(b->ply) + std::string(" ") + std::to_string(b->zorbist), 5-depth);
		if (after != before) 
		{
			//print_line_with_depth(square_names[move->from] + square_names[move->to] + std::to_string(b->ply) + std::string(" ") + std::to_string(b->zorbist), 6 - depth);
			//print_line_with_depth("The Hash Changed", 0);
			assert(false);
		}
	}

	return sum;
}

void print_line_with_depth(std::string line, int depth)
{
#ifdef _DEBUG
	std::string joined = "";
	for (int i = 0; i < depth; i++) {
		joined += "\t";
	}
	outfile << joined + line + "\n";
#endif
}
void hash_start_board(board* b) 
{
	for (square s = 0; s < 128; s++)
	{
		if (IS_SQ(s))
		{
			if (b->pieces->at(s) != NONE)
			{
				b->zorbist ^= zorbist->Get_Hash_Value(s,b->pieces->at(s), b->colors->at(s));
			}
		}
	}
}

unsigned long perft_raw(int depth, board* b)
{
	unsigned long sum = 0;
	std::vector<const move*> *moves = new std::vector<const move*>();
	generate_moves(moves, b);
	
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		const move* m = *it;
		make_move(m, b);
		if (!isAttacked(b->side_to_move == WHITE ? b->white_king : b->black_king, b))
		{
			if (depth != 1)
			{
				sum += perft_raw(depth - 1, b);
			}
			else
			{
				sum++;
			}
		}
		unmake_move(m, b);
		delete m;
	}
	delete moves;
	return sum;
}

void perft(int depth, board* b, std::vector<const move*>* moves) 
{
	int king_index = b->side_to_move == WHITE ? b->white_king : b->black_king;

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

	std::vector<const move*> *raw_moves = new std::vector<const move*>();
	generate_moves(raw_moves, b);
	
	for (auto it = raw_moves->begin(); it != raw_moves->end(); it++)
	{
		const move* raw_move = *it;
		make_move(raw_move, b);

		if (!isAttacked(king_index, b))
		{
			// insert move into moves
			// if this is a recursive lookup that moves is deep_moves
			moves->push_back(raw_move);

			// if were still going down the tree
			if (depth != 1) 
			{
				std::vector<const move*> *deep_moves = new std::vector<const move*>();
				// store the board state from the current move
				// TODO: move to constructor
				perft(depth - 1, b, deep_moves);
				//print_line_with_depth(std::to_string(hashed->hash), 0);
				//print_line_with_depth(board_to_fen(b), 0);
				if (hashed_boards->count(b->zorbist) == 0)
				{
					hashed_board* hashed = new hashed_board();
					hashed->hash = b->zorbist;
					hashed->moves = deep_moves;
					hashed_boards->insert({ hashed->hash, hashed });
				}
				else
				{
					delete deep_moves;
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

	delete raw_moves;
}

void print_move(move m) 
{
#ifdef DEBUG
	std::cout << square_names[m.from] << square_names[m.to];
#endif
}

void make_move(const move* m, board* board) 
{
	board->ply++;
	if (board->side_to_move == WHITE)
	{
		board->white_pieces->erase(m->from);
		board->white_pieces->insert(m->to);
		if (m->capture)
		{
			board->black_pieces->erase(m->to);
		}
	}
	else
	{
		board->black_pieces->erase(m->from);
		board->black_pieces->insert(m->to);
		if (m->capture)
		{
			board->white_pieces->erase(m->to);
		}
	}

	board->zorbist ^= zorbist->color_hash;
	// remove from piece
	board->zorbist ^= zorbist->Get_Hash_Value(m->from, board->pieces->at(m->from), board->colors->at(m->from));
	board->pieces->at(m->from) = NONE;
	board->colors->at(m->from) = NO_COLOR;

	// clear to piece
	if (CAPTURE & m->flags)
	{
		board->zorbist ^= zorbist->Get_Hash_Value((m->to), board->pieces->at(m->to), board->colors->at(m->to));
	}

	// add moved piece
	board->colors->at(m->to) = board->side_to_move;
	board->pieces->at(m->to) = m->piece_to;
	board->zorbist ^= zorbist->Get_Hash_Value(m->to, m->piece_to, board->colors->at(m->to));

	// adjust castling flags
	if (board->castle_moves) 
	{
		if (m->from == E1) 
		{
			board->castle_moves ^= (WHITEK | WHITEQ);
		}

		if (m->from == A1)
		{
			board->castle_moves ^= WHITEQ;
		}

		if (m->from == H1)
		{
			board->castle_moves ^= WHITEK;
		}

		if (m->from == E8) 
		{
			board->castle_moves ^= (BLACKK | BLACKQ);
		}

		if (m->from == A8)
		{
			board->castle_moves ^= BLACKQ;
		}

		if (m->from == H8)
		{
			board->castle_moves ^= BLACKK;
		}
	}

	board->ep_square = m->ep;
	board->side_to_move = board->side_to_move == BLACK ? WHITE : BLACK;
}

void unmake_move(const move* m, board* board) 
{
	board->ply--;
	board->side_to_move = board->side_to_move == BLACK ? WHITE : BLACK;
	// remove to piece
	board->zorbist ^= zorbist->Get_Hash_Value(m->to, board->pieces->at(m->to), board->colors->at(m->to));
	board->pieces->at(m->to) = m->capture;
	if (CAPTURE & m->flags) 
	{
		board->colors->at(m->to) = board->side_to_move == WHITE ? BLACK : WHITE;
		// add back captured piece
		board->zorbist ^= zorbist->Get_Hash_Value(m->to, board->pieces->at(m->to), board->colors->at(m->to));
	}
	else
	{
		board->colors->at(m->to) = NO_COLOR;
	}

	// add back from piece
	board->pieces->at(m->from) = m->piece_from;
	board->colors->at(m->from) = board->side_to_move;
	board->zorbist ^= zorbist->Get_Hash_Value(m->from, board->pieces->at(m->from), board->colors->at(m->from));
	board->zorbist ^= zorbist->color_hash;

	if (board->side_to_move == WHITE)
	{
		board->white_pieces->insert(m->from);
		board->white_pieces->erase(m->to);
		if (m->capture)
		{
			board->black_pieces->insert(m->to);
		}
	}
	else
	{
		board->black_pieces->insert(m->from);
		board->black_pieces->erase(m->to);
		if (m->capture)
		{
			board->white_pieces->insert(m->to);
		}
	}
	// adjust castling flags
	// do things here if move is castle
	if (m->castle) 
	{
		// check m->to square to revert a castle
	}

	//update_hash(m, board);
	board->ep_square = m->ep;
}

bool isAttacked(const square& square, board* board) 
{
	color moving_color = board->colors->at(square);
	color attack_color = moving_color;

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
		
		if (IS_SQ(target) and board->colors->at(target) == attack_color) 
		{
			if (board->pieces->at(target) == KNIGHT) 
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
			if (board->colors->at(target) == moving_color)
			{
				break;
			}
			else if (board->colors->at(target) == attack_color)
			{
				piece target_piece = board->pieces->at(target);
				if (in_pawn_range && target_piece == PAWN) 
				{	
					if (attack_color == WHITE 
						&& (ray == NW || ray == NE)) 
					{
						return true;
					}

					if (attack_color == BLACK
						&& (ray == SW || ray == SE)) 
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
					if (target_piece == (BISHOP | QUEEN))
					{
						return true;
					}
				}
				else 
				{
					if (target_piece == (QUEEN | ROOK))
					{
						return true;
					}
				}
			}

			// move along the ray
			target += ray;
		}
	}

	return false;
}


void generate_moves(std::vector<const move*> *moves, board* board)
{
	char castle_moves = board->castle_moves;
	color same_side = board->side_to_move;
	color other_side = board->side_to_move == WHITE ? BLACK : WHITE;

	if (board->side_to_move == WHITE)
	{
		if (castle_moves & WHITEK)
		{
			if (board->pieces->at(B1) == NONE and board->pieces->at(C1) == NONE and board->pieces->at(D1) == NONE)
			{
				if (!isAttacked(C1, board) and !isAttacked(D1, board))
				{
					moves->push_back(build_move(E1, C1, KING, NONE, CASTLE, NONE, board));
				}
			}
		}
		if (castle_moves & WHITEQ)
		{
			if (board->pieces->at(F1) == NONE and board->pieces->at(G1) == NONE)
			{
				if (!isAttacked(F1, board) and !isAttacked(G1, board))
				{
					moves->push_back(build_move(E1, G1, KING, NONE, CASTLE, NONE, board));
				}
			}
		}

	}
	else if (board->side_to_move == BLACK)
	{
		if (castle_moves & BLACKK)
		{
			if (board->pieces->at(B8) == NONE and board->pieces->at(C8) == NONE and board->pieces->at(D8) == NONE)
			{
				if (!isAttacked(C8, board) and !isAttacked(D8, board))
				{
					moves->push_back(build_move(E8, C8, KING, NONE, CASTLE, NONE, board));
				}
			}
		}
		if (castle_moves & BLACKQ)
		{
			if (board->pieces->at(F8) == NONE and board->pieces->at(G8) == NONE)
			{
				if (!isAttacked(F8, board) and !isAttacked(G8, board))
				{
					moves->push_back(build_move(E8, G8, KING, NONE, CASTLE, NONE, board));
				}
			}
		}
	}

	std::set<int>* same_side_pieces;
	if (board->side_to_move == WHITE)
	{
		same_side_pieces = board->white_pieces;
	}
	else
	{
		same_side_pieces = board->black_pieces;
	}

	for (auto it = same_side_pieces->begin(); it != same_side_pieces->end(); it++)
	{
		const square square_index = *it;
		piece current_piece = board->pieces->at(square_index);
		int current_target = square_index;
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
			if (board->side_to_move == WHITE) 
			{
				if (square_index / 16 == 1) 
				{
					current_target = square_index + NN;
					if (IS_SQ(current_target) and board->pieces->at(current_target) == NONE) 
					{
						moves->push_back(build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, NONE, board));
					}
				}

				current_target = square_index + N;
				if (IS_SQ(current_target) && board->pieces->at(current_target) == NONE) 
				{
					if (current_target / 16 == 7) 
					{
						for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
						{
							const move* m = build_move(square(square_index), square(current_target), PAWN, NONE, PROMOTION, NONE, board);
							moves->push_back(m);
						}
					}
					else 
					{
						moves->push_back(build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, NONE, board));
					}
				}

				int pawn_captures[] = { square_index + NW, square_index + NE };
				for (int capture_index: pawn_captures) 
				{
					if (IS_SQ(capture_index) and board->pieces->at(capture_index) != NONE && board->colors->at(capture_index) != board->side_to_move) 
					{
						if (capture_index / 16 == 7) 
						{
							for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
							{
								const move* m = build_move(square(square_index), square(capture_index), PAWN, board->pieces->at(capture_index), (CAPTURE | PROMOTION), promote, board);
								moves->push_back(m);
							}
						}
						else 
						{
							moves->push_back(build_move(square(square_index), square(capture_index), PAWN, board->pieces->at(capture_index), CAPTURE, NONE, board));
						}
					}
				}
				
			}

			if (board->side_to_move == BLACK) 
			{
				if (square_index / 16 == 6) 
				{
					current_target = square_index + SS;
					if (IS_SQ(current_target) and board->pieces->at(current_target) == NONE) 
					{
						moves->push_back(build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, NONE, board));
					}
				}
				current_target = square_index + S;
				if (IS_SQ(current_target) and board->pieces->at(current_target) == NONE) 
				{
					if (current_target / 16 == 0) 
					{
						for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
						{
							const move* m = build_move(square(square_index), square(current_target), PAWN, NONE, PROMOTION, promote, board);
							moves->push_back(m);
						}
					}
					else 
					{
						moves->push_back(build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, NONE, board));
					}
				}

				int pawn_captures[] = { square_index + SW, square_index + SE };
				for (int capture_index: pawn_captures) 
				{
					if (IS_SQ(capture_index) and board->pieces->at(capture_index) != NONE && board->colors->at(capture_index) != board->side_to_move) 
					{
						if (capture_index/ 16 == 7) 
						{
							for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
							{
								const move* m = build_move(square(square_index), square(capture_index), PAWN, board->pieces->at(capture_index), (CAPTURE | PROMOTION), promote, board);
								moves->push_back(m);
							}
						}
						else 
						{
							moves->push_back(build_move(square(square_index), square(capture_index), PAWN, board->pieces->at(capture_index), CAPTURE, NONE, board));
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
					if (board->colors->at(current_target) == board->side_to_move) 
					{
						break;
					}

					if (board->pieces->at(current_target) == NONE) 
					{
						moves->push_back(build_move(square(square_index), square(current_target), current_piece, NONE, NO_FLAGS, NONE, board));
					}
					else
					{
						moves->push_back(build_move(square(square_index), square(current_target), current_piece, board->pieces->at(current_target), CAPTURE, NONE, board));
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

const move* build_move(square from_square, square to_square, piece from, piece captured, char flags, piece to_piece, board* b) 
{
	move* m = new move();
	m->from = from_square;
	m->to = to_square;
	m->capture = captured;
	m->piece_from = from;
	m->piece_to = to_piece != NONE ? to_piece : from;
	m->flags = flags;
	m->ply = b->ply;
	m->castle = b->castle_moves != 0;
	m->ep = b->ep_square;
	return m;
}

void fen_to_board(const std::string& fen, board* board)
{
	int index = 0;
	bool board_done = false;
	for (int index = 0; index < 128; index++) 
	{
		board->pieces->at(index) = NONE;
		board->colors->at(index) = NO_COLOR;
	}
	for (char const& c : fen) 
	{
		if (board_done)
		{
			break;
		}

		switch (c) {
			case 'r':
				board->pieces->at(index) = ROOK;
				board->colors->at(index) = WHITE;
				index++;
				break;
			case 'n':
				board->pieces->at(index) = KNIGHT;
				board->colors->at(index) = WHITE;
				index++;
				break;
			case 'b':
				board->pieces->at(index) = BISHOP;
				board->colors->at(index) = WHITE;
				index++;
				break;
			case 'q':
				board->pieces->at(index) = QUEEN;
				board->colors->at(index) = WHITE;
				index++;
				break;
			case 'k':
				board->pieces->at(index) = KING;
				board->colors->at(index) = WHITE;
				index++;
				break;
			case 'p':
				board->pieces->at(index) = PAWN;
				board->colors->at(index) = WHITE;
				index++;
				break;
			case 'R':
				board->pieces->at(index) = ROOK;
				board->colors->at(index) = BLACK;
				index++;
				break;
			case 'N':
				board->pieces->at(index) = KNIGHT;
				board->colors->at(index) = BLACK;
				index++;
				break;
			case 'B':
				board->pieces->at(index) = BISHOP;
				board->colors->at(index) = BLACK;
				index++;
				break;
			case 'Q':
				board->pieces->at(index) = QUEEN;
				board->colors->at(index) = BLACK;
				index++;
				break;
			case 'K':
				board->pieces->at(index) = KING;
				board->colors->at(index) = BLACK;
				index++;
				break;
			case 'P':
				board->pieces->at(index) = PAWN;
				board->colors->at(index) = BLACK;
				index++;
				break;
			case '/':
				index += 8;
				break;
			case '1':
				index += 1;
				break;
			case '2':
				index += 2;
				break;
			case '3':
				index += 3;
				break;
			case '4':
				index += 4;
				break;
			case '5':
				index += 5;
				break;
			case '6':
				index += 6;
				break;
			case '7':
				index += 7;
				break;
			case '8':
				index += 8;
				break;
			case ' ':
				board_done = true;
				break;
		}
	}

	for (int index = 0; index < 128; index++) 
	{
		if (board->colors->at(index) == WHITE) 
		{
			if (board->pieces->at(index) == KING) 
			{
				board->white_king = square(index);
			}

			board->white_pieces->insert(index);
		}
		else if (board->colors->at(index) == BLACK) 
		{
			board->black_pieces->insert(index);
			if (board->pieces->at(index) == KING) 
			{
				board->black_king = square(index);
			}
		}
	}
} 

std::string board_to_fen(board* b)
{
	std::string fen = "";
	int empty_squares = 0;
	for (square s = A1; s <= H8; s++) 
	{
		if (IS_SQ(s))
		{
			color c = b->colors->at(s);
			piece p = b->pieces->at(s);
			if (p == NONE)
			{
				empty_squares++;
			}
			else
			{
				if (empty_squares != 0)
				{
					fen += std::to_string(empty_squares);
					empty_squares = 0;
				}
				switch (p) 
				{
					case PAWN:
						fen += c == WHITE ? "p" : "P";
						break;
					case ROOK:
						fen += c == WHITE ? "r" : "R";
						break;
					case KNIGHT:
						fen += c == WHITE ? "n" : "N";
						break;
					case BISHOP:
						fen += c == WHITE ? "b" : "B";
						break;
					case QUEEN:
						fen += c == WHITE ? "q" : "Q";
						break;
					case KING:
						fen += c == WHITE ? "k" : "K";
						break;
				}
			}

			if ((s & 7) == 7)
			{
				if (empty_squares != 0)
				{
					fen += std::to_string(empty_squares);
					empty_squares = 0;
				}
				if (s == H8)
				{
					break;
				}
				fen += "/";
			}
		}
	}

	fen += b->side_to_move == WHITE ? "B" : "W";
	return fen;
}