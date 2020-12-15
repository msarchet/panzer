// Panzer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "constants.h"
#include <chrono>
#include <fstream>
#include <string>


void fen_to_board(const std::string& fen, board* board);
void make_move(const move& move, board* board);
void unmake_move(const move& move, board* board);
bool isAttacked(const square& square, board* board);
move build_move(square from_square, square to_square, piece from, piece captured, char flags, board* b);
int generate_moves(move * moves, board * board);
void print_move(move m);
int perft(int depth, board* b, move* moves);
int sum_perft_nodes(board* b, int depth);

void hash_start_board(board* b);

std::ofstream outfile;

int main(int argc, char *argv[])
{
	fill_hash_lookup();
	// Using time point and system_clock 
    std::chrono::time_point<std::chrono::steady_clock> start, end; 
  

	std::cout << "Running";
	board* b = new board;
	fen_to_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", b);
	b->side_to_move = WHITE;
	hash_start_board(b);
	int depth = 4;
	std::string filename = "perft-depth" + std::to_string(depth) + ".log";
	outfile.open(filename, std::ios_base::app);

	if (argc > 1) 
	{
		depth = argv[1][0] - '0';
	}

	outfile << "Peft depth " << depth << "\n";
	//outfile << "Starting Hash" << b->zorbist << "\n";

	move m1 = build_move(E2, E4, PAWN, NONE, NO_FLAGS, b);
	move m2 = build_move(D7, D5, PAWN, NONE, NO_FLAGS, b);
	move m3 = build_move(E4, D5, PAWN, PAWN, CAPTURE, b);
	make_move(m1, b);
	//outfile << "make_move 1 " << b->zorbist << "\n";

	make_move(m2, b);
	//outfile << "make_move 2 " << b->zorbist << "\n";
	make_move(m3, b);
	//outfile << "make_move 3 " << b->zorbist << "\n";
	unmake_move(m3, b);
	//outfile << "unmake_move 3 (2) " << b->zorbist << "\n";
	unmake_move(m2, b);
	//outfile << "unmake_move 2 (1)" << b->zorbist << "\n";
	unmake_move(m1, b);
	//outfile << "unmake_move 1 (0) " << b->zorbist << "\n";
	hashed_board starting_hash;
	move moves[256];
	start = std::chrono::high_resolution_clock::now();
	int nodes = perft(depth, b, moves);
	end = std::chrono::high_resolution_clock::now();
	starting_hash.moves = moves;
	starting_hash.move_count = nodes;
	//outfile << "Ending Hash" << b->zorbist << "\n";
	b->zorbist = 0;
	hash_start_board(b);
	//outfile << "RE: Hash" << b->zorbist << "\n";
	hashed_boards[b->zorbist] = &starting_hash;
	//outfile << "Summing" << "\n";
	int total = sum_perft_nodes(b, depth);

	std::chrono::duration<double> elapsed_seconds = end - start; 
	outfile << "Time:" << elapsed_seconds.count();
	outfile << "\n";
	outfile << "Perft" << depth << " " << total;
	outfile << "Moves PS: " << total / elapsed_seconds.count();

	int move_count = 0;
	for (const auto& pair : hashed_boards)
	{
		const hashed_board* board = pair.second;
		move_count += board->move_count;
	}
	std::cin.get();
}
int sum_perft_nodes(board* b, int depth)
{
	int sum = 0;

	auto it = hashed_boards.find(b->zorbist);
	//outfile << "found hashboard with hash" << b->zorbist;
	const hashed_board* hashed = it->second;

	for (int i = 0; i < hashed->move_count; i++) 
	{
		//outfile << "hash before make move: " << b->ply << " " << b->zorbist;
		make_move(hashed->moves[i], b);
		//outfile << square_names[hashed.moves[i].from] << square_names[hashed.moves[i].to] << b->ply << " " << b->zorbist;
		if (depth == 1)
		{
			sum++;
		}
		else
		{
			sum += sum_perft_nodes(b, depth - 1);
		}
		unmake_move(hashed->moves[i], b);
		//outfile << "hash after unmake move: " << b->ply << " " << b->zorbist;
	}

	return sum;
}
void hash_start_board(board* b) 
{
	for (square s = 0; s < 128; s++)
	{
		if (IS_SQ(s))
		{
			if (b->pieces[s] != NONE)
			{
				b->zorbist ^= zorbist_hash_lookup[s][b->pieces[s]][b->colors[s]];
			}
		}
	}
}

int perft(int depth, board* b, move* moves) 
{
	int king_index = b->side_to_move == WHITE ? b->white_king : b->black_king;
	int move_count;

	if (hashed_boards.count(b->zorbist))
	{
		const hashed_board* board_from_hash = hashed_boards[b->zorbist];
		// we will hash boards and not hash moves for them because they are 
		// at the last ply
		if (board_from_hash->move_count != 0 or board_from_hash->isCheckmate)
		{
			moves = board_from_hash->moves;
			return board_from_hash->move_count;
		}
	}

	move* raw_moves = new move[256]{ };
	move_count = generate_moves(raw_moves, b);
	int legal_move_count = 0;
	
	for (int i = 0; i < move_count; i++)
	{
		make_move(raw_moves[i], b);

		if (!isAttacked(square(king_index), b))
		{
			// insert move into moves
			// if this is a recursive lookup that moves is deep_moves
			moves[legal_move_count] = raw_moves[i];
			//outfile << "hash after make move";
			//outfile << square_names[moves[legal_move_count].from] << square_names[moves[legal_move_count].to] << b->ply << " " << b->zorbist;
			legal_move_count++; 

			// if were still going down the tree
			if (depth != 1) 
			{
				move* deep_moves = new move[256]{};
				// store the board state from the current move
				// TODO: move to constructor
				hashed_board* hashed = new hashed_board;
				hashed->hash = b->zorbist;
				hashed->move_count = perft(depth - 1, b, deep_moves);
				hashed->moves = deep_moves;
				// add to boards
				hashed_boards[hashed->hash] = hashed;
			}
		}

		unmake_move(raw_moves[i], b);
		//outfile << "hash after unmake move" << b->zorbist;
	}
	return legal_move_count;
}

void print_move(move m) 
{
	std::cout << square_names[m.from] << square_names[m.to];
}

void make_move(const move& m, board* board) 
{
	board->ply++;
	if (m.capture != NONE) 
	{
		if (board->colors[m.to] == WHITE) 
		{
			board->white_pieces.erase(m.to);
			board->black_pieces.erase(m.from);
			board->black_pieces.insert(m.to);
		}
		else
		{
			board->black_pieces.erase(m.to);
			board->white_pieces.erase(m.from);
			board->white_pieces.insert(m.to);
		}
	}

	board->zorbist ^= color_hash;
	// remove from piece
	board->zorbist ^= zorbist_hash_lookup[m.from][board->pieces[m.from]][board->colors[m.from]];
	board->pieces[m.from] = NONE;
	board->colors[m.from] = NO_COLOR;

	// clear to piece
	if (CAPTURE & m.flags)
	{
		board->zorbist ^= zorbist_hash_lookup[m.to][board->pieces[m.to]][board->colors[m.to]];
	}

	// add moved piece
	board->colors[m.to] = board->side_to_move;
	board->pieces[m.to] = m.piece_to;
	board->zorbist ^= zorbist_hash_lookup[m.to][m.piece_to][board->colors[m.to]];

	// adjust castling flags
	if (board->castle_moves) 
	{
		if (m.from == E1) 
		{
			board->castle_moves ^= (WHITEK | WHITEQ);
		}

		if (m.from == A1)
		{
			board->castle_moves ^= WHITEQ;
		}

		if (m.from == H1)
		{
			board->castle_moves ^= WHITEK;
		}

		if (m.from == E8) 
		{
			board->castle_moves ^= (BLACKK | BLACKQ);
		}

		if (m.from == A8)
		{
			board->castle_moves ^= BLACKQ;
		}

		if (m.from == H8)
		{
			board->castle_moves ^= BLACKK;
		}
	}

	board->ep_square = m.ep;
	board->side_to_move = board->side_to_move == BLACK ? WHITE : BLACK;
}

void unmake_move(const move& m, board* board) 
{
	board->ply--;
	board->side_to_move = board->side_to_move == BLACK ? WHITE : BLACK;
	// remove to piece
	board->zorbist ^= zorbist_hash_lookup[m.to][board->pieces[m.to]][board->colors[m.to]];
	board->pieces[m.to] = m.capture;
	if (CAPTURE & m.flags) 
	{
		board->colors[m.to] = board->side_to_move == WHITE ? BLACK : WHITE;
		// add back captured piece
		board->zorbist ^= zorbist_hash_lookup[m.to][board->pieces[m.to]][board->colors[m.to]];
	}
	else
	{
		board->colors[m.to] = NO_COLOR;
	}

	// add back from piece
	board->pieces[m.from] = m.piece_from;
	board->colors[m.from] = board->side_to_move;
	board->zorbist ^= zorbist_hash_lookup[m.from][board->pieces[m.from]][board->colors[m.from]];
	board->zorbist ^= color_hash;

	if (board->side_to_move == WHITE)
	{
		board->white_pieces.insert(m.from);
		board->white_pieces.erase(m.to);
		if (m.capture)
		{
			board->black_pieces.insert(m.to);
		}
	}
	else
	{
		board->black_pieces.insert(m.from);
		board->black_pieces.erase(m.to);
		if (m.capture)
		{
			board->white_pieces.insert(m.to);
		}
	}
	// adjust castling flags
	// do things here if move is castle
	if (m.castle) 
	{
		// check m.to square to revert a castle
	}

	//update_hash(m, board);
	board->ep_square = m.ep;
}

bool isAttacked(const square& square, board* board) 
{
	color moving_color = board->colors[square];
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
		
		if (IS_SQ(target) and board->colors[target] == attack_color) 
		{
			if (board->pieces[target] == KNIGHT) 
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
			if (board->colors[target] == moving_color)
			{
				break;
			}
			else if (board->colors[target] == attack_color)
			{
				piece target_piece = board->pieces[target];
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


int generate_moves(move * moves, board* board)
{
	int move_count = 0;
	char castle_moves = board->castle_moves;
	color same_side = board->side_to_move;
	color other_side = board->side_to_move == WHITE ? BLACK : WHITE;

	if (board->side_to_move == WHITE)
	{
		if (castle_moves & WHITEK)
		{
			if (board->pieces[B1] == NONE and board->pieces[C1] == NONE and board->pieces[D1] == NONE)
			{
				if (!isAttacked(C1, board) and !isAttacked(D1, board))
				{
					moves[move_count] = build_move(E1, C1, KING, NONE, CASTLE, board);
					move_count++;
				}
			}
		}
		if (castle_moves & WHITEQ)
		{
			if (board->pieces[F1] == NONE and board->pieces[G1] == NONE)
			{
				if (!isAttacked(F1, board) and !isAttacked(G1, board))
				{
					moves[move_count] = build_move(E1, G1, KING, NONE, CASTLE, board);
					move_count++;
				}
			}
		}

	}
	else if (board->side_to_move == BLACK)
	{
		if (castle_moves & BLACKK)
		{
			if (board->pieces[B8] == NONE and board->pieces[C8] == NONE and board->pieces[D8] == NONE)
			{
				if (!isAttacked(C8, board) and !isAttacked(D8, board))
				{
					moves[move_count] = build_move(E8, C8, KING, NONE, CASTLE, board);
					move_count++;
				}
			}
		}
		if (castle_moves & BLACKQ)
		{
			if (board->pieces[F8] == NONE and board->pieces[G8] == NONE)
			{
				if (!isAttacked(F8, board) and !isAttacked(G8, board))
				{
					moves[move_count] = build_move(E8, G8, KING, NONE, CASTLE, board);
					move_count++;
				}
			}
		}
	}

	std::set<int> same_side_pieces;
	if (board->side_to_move == WHITE)
	{
		same_side_pieces = board->white_pieces;
	}
	else
	{
		same_side_pieces = board->black_pieces;
	}

	for (square square_index: same_side_pieces)
	{
		piece current_piece = board->pieces[square_index];
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
					if (IS_SQ(current_target) and board->pieces[current_target] == NONE) 
					{
						moves[move_count] = build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, board);
						move_count++;
					}
				}

				current_target = square_index + N;
				if (IS_SQ(current_target) && board->pieces[current_target] == NONE) 
				{
					if (current_target / 16 == 7) 
					{
						for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
						{
							moves[move_count] = build_move(square(square_index), square(current_target), PAWN, NONE, PROMOTION, board);
							moves[move_count].piece_to = promote;
							move_count++;
						}
					}
					else 
					{
						moves[move_count] = build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, board);
						move_count++;
					}
				}

				int pawn_captures[] = { square_index + NW, square_index + NE };
				for (int capture_index: pawn_captures) 
				{
					if (IS_SQ(capture_index) and board->pieces[capture_index] != NONE && board->colors[capture_index] != board->side_to_move) 
					{
						if (capture_index / 16 == 7) 
						{
							for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
							{
								moves[move_count] = build_move(square(square_index), square(capture_index), PAWN, board->pieces[capture_index], (CAPTURE | PROMOTION), board);
								moves[move_count].piece_to = promote;
								move_count++;
							}
						}
						else 
						{
							moves[move_count] = build_move(square(square_index), square(capture_index), PAWN, board->pieces[capture_index], CAPTURE, board);
							move_count++;
						}
					}
				}
				
			}

			if (board->side_to_move == BLACK) 
			{
				if (square_index / 16 == 6) 
				{
					current_target = square_index + SS;
					if (IS_SQ(current_target) and board->pieces[current_target] == NONE) 
					{
						moves[move_count] = build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, board);
						move_count++;
					}
				}
				current_target = square_index + S;
				if (IS_SQ(current_target) and board->pieces[current_target] == NONE) 
				{
					if (current_target / 16 == 0) 
					{
						for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
						{
							moves[move_count] = build_move(square(square_index), square(current_target), PAWN, NONE, PROMOTION, board);
							moves[move_count].piece_to = promote;
							move_count++;
						}
					}
					else 
					{
						moves[move_count] = build_move(square(square_index), square(current_target), PAWN, NONE, NO_FLAGS, board);
						move_count++;
					}
				}

				int pawn_captures[] = { square_index + SW, square_index + SE };
				for (int capture_index: pawn_captures) 
				{
					if (IS_SQ(capture_index) and board->pieces[capture_index] != NONE && board->colors[capture_index] != board->side_to_move) 
					{
						if (capture_index/ 16 == 7) 
						{
							for (piece promote : {QUEEN, ROOK, KNIGHT, BISHOP}) 
							{
								moves[move_count] = build_move(square(square_index), square(capture_index), PAWN, board->pieces[capture_index], (CAPTURE | PROMOTION), board);
								moves[move_count].piece_to = promote;
								move_count++;
							}
						}
						else 
						{
							moves[move_count] = build_move(square(square_index), square(capture_index), PAWN, board->pieces[capture_index], CAPTURE, board);
							move_count++;
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
					if (board->colors[current_target] == board->side_to_move) 
					{
						break;
					}

					if (board->pieces[current_target] == NONE) 
					{
						moves[move_count] = build_move(square(square_index), square(current_target), current_piece, NONE, NO_FLAGS, board);
						move_count++;
					}
					else
					{
						moves[move_count] = build_move(square(square_index), square(current_target), current_piece, board->pieces[current_target], CAPTURE, board);
						move_count++;
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

	return move_count;
}

move build_move(square from_square, square to_square, piece from, piece captured, char flags, board* b) 
{
	move m = move();
	m.from = from_square;
	m.to = to_square;
	m.capture = captured;
	m.piece_from = from;
	m.piece_to = from;
	m.flags = flags;
	m.ply = b->ply;
	m.castle = b->castle_moves != 0;
	m.ep = b->ep_square;
	return m;
}

void fen_to_board(const std::string& fen, board* board)
{
	int index = 0;
	bool board_done = false;
	for (int index = 0; index < 128; index++) 
	{
		board->pieces[index] = NONE;
		board->colors[index] = NO_COLOR;
	}
	for (char const& c : fen) 
	{
		if (board_done)
		{
			break;
		}

		switch (c) {
			case 'r':
				board->pieces[index] = ROOK;
				board->colors[index] = WHITE;
				index++;
				break;
			case 'n':
				board->pieces[index] = KNIGHT;
				board->colors[index] = WHITE;
				index++;
				break;
			case 'b':
				board->pieces[index] = BISHOP;
				board->colors[index] = WHITE;
				index++;
				break;
			case 'q':
				board->pieces[index] = QUEEN;
				board->colors[index] = WHITE;
				index++;
				break;
			case 'k':
				board->pieces[index] = KING;
				board->colors[index] = WHITE;
				index++;
				break;
			case 'p':
				board->pieces[index] = PAWN;
				board->colors[index] = WHITE;
				index++;
				break;
			case 'R':
				board->pieces[index] = ROOK;
				board->colors[index] = BLACK;
				index++;
				break;
			case 'N':
				board->pieces[index] = KNIGHT;
				board->colors[index] = BLACK;
				index++;
				break;
			case 'B':
				board->pieces[index] = BISHOP;
				board->colors[index] = BLACK;
				index++;
				break;
			case 'Q':
				board->pieces[index] = QUEEN;
				board->colors[index] = BLACK;
				index++;
				break;
			case 'K':
				board->pieces[index] = KING;
				board->colors[index] = BLACK;
				index++;
				break;
			case 'P':
				board->pieces[index] = PAWN;
				board->colors[index] = BLACK;
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
		if (board->colors[index] == WHITE) 
		{
			if (board->pieces[index] == KING) 
			{
				board->white_king = square(index);
			}

			board->white_pieces.insert(index);
		}
		else if (board->colors[index] == BLACK) 
		{
			board->black_pieces.insert(index);
			if (board->pieces[index] == KING) 
			{
				board->black_king = square(index);
			}
		}
	}
}
