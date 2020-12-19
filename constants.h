#pragma once
#include <set>
#include <map>
#include <array>
#include <unordered_map>
#include <stdlib.h>
#include <random>
#include <cmath>
#include <memory>
#include <unordered_set>

#define IS_SQ(x) (x) & 0x88 ? 0 : 1

constexpr auto STARTFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR W";

typedef unsigned __int8 square;
typedef unsigned __int8 piece;
typedef unsigned __int8 color;
typedef unsigned __int8 move_flags;
typedef unsigned __int64 hash;
typedef unsigned __int8 castle_flags;

const square A1 = 0;
const square B1 = 1;
const square C1 = 2;
const square D1 = 3;
const square E1 = 4;
const square F1 = 5;
const square G1 = 6;
const square H1 = 7;

const square A2 = A1 + 16; // 16
const square B2 = A2 + 1;
const square C2 = B2 + 1;
const square D2 = C2 + 1;
const square E2 = D2 + 1;
const square F2 = E2 + 1;
const square G2 = F2 + 1;
const square H2 = G2 + 1;

const square A3 = A2 + 16;
const square B3 = A3 + 1;
const square C3 = B3 + 1;
const square D3 = C3 + 1;
const square E3 = D3 + 1;
const square F3 = E3 + 1;
const square G3 = F3 + 1;
const square H3 = G3 + 1;

const square A4 = A3 + 16;
const square B4 = A4 + 1;
const square C4 = B4 + 1;
const square D4 = C4 + 1;
const square E4 = D4 + 1;
const square F4 = E4 + 1;
const square G4 = F4 + 1;
const square H4 = G4 + 1;

const square A5 = A4 + 16;
const square B5 = A5 + 1;
const square C5 = B5 + 1;
const square D5 = C5 + 1;
const square E5 = D5 + 1;
const square F5 = E5 + 1;
const square G5 = F5 + 1;
const square H5 = G5 + 1;

const square A6 = A5 + 16;
const square B6 = A6 + 1;
const square C6 = B6 + 1;
const square D6 = C6 + 1;
const square E6 = D6 + 1;
const square F6 = E6 + 1;
const square G6 = F6 + 1;
const square H6 = G6 + 1;

const square A7 = A6 + 16;
const square B7 = A7 + 1;
const square C7 = B7 + 1;
const square D7 = C7 + 1;
const square E7 = D7 + 1;
const square F7 = E7 + 1;
const square G7 = F7 + 1;
const square H7 = G7 + 1;

const square A8 = A7 + 16;
const square B8 = A8 + 1;
const square C8 = B8 + 1;
const square D8 = C8 + 1;
const square E8 = D8 + 1;
const square F8 = E8 + 1;
const square G8 = F8 + 1;
const square H8 = G8 + 1;

const square EMPTY = H8 + 1;

const piece	NONE = 0;
const piece	PAWN = 2;
const piece	ROOK = 4;
const piece	BISHOP = 8;
const piece	KNIGHT = 16;
const piece	QUEEN = 32;
const piece	KING = 64;

const color	WHITE = 0;
const color	BLACK = 1;
const color	NO_COLOR = 2;


const move_flags NO_FLAGS = 0;
const move_flags CASTLE = 2;
const move_flags EP = 4;
const move_flags PROMOTION = 8;
const move_flags CAPTURE = 16;
struct move
{
	char id = 0;
	square from = EMPTY;
	square to = EMPTY;
	piece piece_from = NONE;
	piece piece_to = NONE;
	piece capture = NONE;
	bool castle = false;
	char flags = NO_FLAGS;
	int ply = 0;
	int score = 0;
	square ep = EMPTY;
	square prior_ep = EMPTY;
	move(square from,
		square to,
		piece from_piece,
		piece to_piece,
		piece capture,
		char flags,
		square ep,
		square prior_ep,
		int ply) :
			from(from),
			to(to),
			piece_from(from_piece),
			piece_to(to_piece),
			capture(capture),
			flags(flags),
			ep(ep),
			prior_ep(prior_ep),
			ply(ply)
	{
		castle =flags & CASTLE;
	}
			
};


struct board
{
	std::shared_ptr<std::array<piece, 128>> pieces = std::make_shared<std::array<piece, 128>>();
	std::shared_ptr<std::array<color, 128>> colors = std::make_shared<std::array<color, 128>>();
	std::shared_ptr<std::unordered_set<piece>> white_pieces = std::make_shared<std::unordered_set<piece>>();
	std::shared_ptr<std::unordered_set<piece>> black_pieces = std::make_shared<std::unordered_set<piece>>();
	color side_to_move = WHITE;
	char castle_moves = (1 | 2 | 4 | 8); // 1K 2 Q 4 k 4 q
	square ep_square = EMPTY;
	int ply = 0;
	hash zorbist = 0;
	square black_king = E8;
	square white_king = E1;
};


const castle_flags WHITEK = 1;
const castle_flags WHITEQ = 2;
const castle_flags BLACKK = 4;
const castle_flags BLACKQ = 8;

const int NW = 16 - 1;
const int N = 16;
const int NE = 16 + 1;
const int E = 1;
const int SE = -16 + 1;
const int S = -16;
const int SW = -16 - 1;
const int W = -1;
const int NN = 32;
const int SS = -32;

// left, up, right, down
const int knight_moves[8] 
{

	-2 + N, // left up
	-2 + S, // left down
	NN - 1, // up left
	NN + 1,  // up right
	2 + N, // right up 
	2 + S,  // right down
	-1 + SS,  // down left
	1 +SS // down right
};

const int rays [] =
{
	NW,
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W
};

#pragma region LOOKUPS

const std::string square_names[]
{
	"A1",
	"B1",
	"C1",
	"D1",
	"E1",
	"F1",
	"G1",
	"H1",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A2",
	"B2",
	"C2",
	"D2",
	"E2",
	"F2",
	"G2",
	"H2",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A3",
	"B3",
	"C3",
	"D3",
	"E3",
	"F3",
	"G3",
	"H3",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A4",
	"B4",
	"C4",
	"D4",
	"E4",
	"F4",
	"G4",
	"H4",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A5",
	"B5",
	"C5",
	"D5",
	"E5",
	"F5",
	"G5",
	"H5",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A6",
	"B6",
	"C6",
	"D6",
	"E6",
	"F6",
	"G6",
	"H6",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A7",
	"B7",
	"C7",
	"D7",
	"E7",
	"F7",
	"G7",
	"H7",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A8",
	"B8",
	"C8",
	"D8",
	"E8",
	"F8",
	"G8",
	"H8",
	"EMPTY"
};

const std::string color_names[]
{
	"WHITE",
	"BLACK",
	"NO_COLOR"
};

struct hashed_board
{
	hash hash = 0;
	std::shared_ptr<std::vector<std::shared_ptr<const move>>> moves = std::make_shared<std::vector<std::shared_ptr<const move>>>();
	int move_count = 0;
	int score = 0;
	bool isCheckmate = false;
};

//std::unordered_map<square, std::unordered_map<piece, std::unordered_map<color, hash>>>  zorbist_hash_lookup;
