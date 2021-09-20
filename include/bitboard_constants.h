#pragma once
#include <array>
#include <vector>
#include <memory>
#include <string>

typedef uint8_t square;
typedef uint8_t piece;
typedef uint8_t move_flag;
typedef uint8_t castle_flag;

typedef uint64_t hash;
typedef uint64_t bitboard;
typedef uint64_t mask;
typedef int direction;
typedef int color;

typedef std::shared_ptr<std::array<std::vector<bitboard>, 64> > square_bitboards;
typedef std::shared_ptr<std::array<uint64_t, 64> > magics_array;
typedef std::array<bitboard, 64> mask_array;

const square NO_SQUARE = 64;
const piece NO_PIECE = 0;
const move_flag EMPTY_MOVE_FLAGS = 0;
const castle_flag EMPTY_CASTLE_FLAGS = 0;

constexpr auto STARTFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - -";

const mask FIRST_RANK    = 0xFFULL;
const mask SECOND_RANK   = 0xFF00ULL;
const mask THIRD_RANK    = 0xFF0000ULL;
const mask FOURTH_RANK   = 0xFF000000ULL;
const mask FIFTH_RANK    = 0xFF00000000ULL;
const mask SIXTH_RANK    = 0xFF0000000000ULL;
const mask SEVENTH_RANK  = 0xFF000000000000ULL;
const mask EIGHTH_RANK   = 0xFF00000000000000ULL;
const mask A_FILE = 0x0101010101010101ULL;
const mask B_FILE = 0x0202020202020202ULL;
const mask C_FILE = 0x0404040404040404ULL;
const mask D_FILE = 0x0808080808080808ULL;
const mask E_FILE = 0x1010101010101010ULL;
const mask F_FILE = 0x2020202020202020ULL;
const mask G_FILE = 0x4040404040404040ULL;
const mask H_FILE = 0x8080808080808080ULL;
const mask AB_FILE = A_FILE | B_FILE;
const mask GH_FILE = G_FILE | H_FILE;
const std::array<mask, 15> NW_DIAGONALS = 
{
    0x0000000000000001ULL,
    0x0000000000000102ULL,
    0x0000000000010204ULL,
    0x0000000001020408ULL,
    0x0000000102040810ULL,
    0x0000010204081020ULL,
    0x0001020408102040ULL,
    0x0102040810204080ULL,
    0x0204081020408000ULL,
    0x0408102040800000ULL,
    0x0810204080000000ULL,
    0x1020408000000000ULL,
    0x2040800000000000ULL,
    0x4080000000000000ULL,
    0x8000000000000000ULL
};

const std::array<mask, 15> NE_DIAGONALS = 
{
    0x0100000000000000ULL,
    0x0201000000000000ULL,
    0x0402010000000000ULL,
    0x0804020100000000ULL,
    0x1008040201000000ULL,
    0x2010080402010000ULL,
    0x4020100804020100ULL,
    0x8040201008040201ULL,
    0x0080402010080402ULL,
    0x0000804020100804ULL,
    0x0000008040201008ULL,
    0x0000000080402010ULL,
    0x0000000000804020ULL,
    0x0000000000008040ULL,
    0x0000000000000080ULL
};

const bitboard FILLED_BOARD = ~0ULL;

const std::array<bitboard, 8> knight_move_masks = 
{ 
    ~(G_FILE|H_FILE), // A_FILE
    ~(H_FILE), // B_FILE
    FILLED_BOARD, // C_FILE
    FILLED_BOARD, // D_FILE
    FILLED_BOARD, // E_FILE
    FILLED_BOARD, // F_FILE
    ~(A_FILE), // G_FILE
    ~(A_FILE|B_FILE), // H_FILE
};
const bitboard king_move_masks[8] = 
{
    ~H_FILE,
    FILLED_BOARD,
    FILLED_BOARD,
    FILLED_BOARD,
    FILLED_BOARD,
    FILLED_BOARD,
    FILLED_BOARD,
    ~A_FILE,
};

const color WHITE = 0;
const color BLACK = 1;
const color NO_COLOR = 2;

const piece PAWN = 1;
const piece ROOK = 2;
const piece KNIGHT = 3;
const piece BISHOP = 4;
const piece QUEEN = 5;
const piece KING = 6;

const bitboard ONE_BIT = 1ULL;

const square A1 = 0;
const square B1 = A1 + 1;
const square C1 = B1 + 1;
const square D1 = C1 + 1;
const square E1 = D1 + 1;
const square F1 = E1 + 1;
const square G1 = F1 + 1;
const square H1 = G1 + 1;

const square A2 = H1 + 1;
const square B2 = A2 + 1;
const square C2 = B2 + 1;
const square D2 = C2 + 1;
const square E2 = D2 + 1;
const square F2 = E2 + 1;
const square G2 = F2 + 1;
const square H2 = G2 + 1;

const square A3 = H2 + 1;
const square B3 = A3 + 1;
const square C3 = B3 + 1;
const square D3 = C3 + 1;
const square E3 = D3 + 1;
const square F3 = E3 + 1;
const square G3 = F3 + 1;
const square H3 = G3 + 1;

const square A4 = H3 + 1;
const square B4 = A4 + 1;
const square C4 = B4 + 1;
const square D4 = C4 + 1;
const square E4 = D4 + 1;
const square F4 = E4 + 1;
const square G4 = F4 + 1;
const square H4 = G4 + 1;

const square A5 = H4 + 1;
const square B5 = A5 + 1;
const square C5 = B5 + 1;
const square D5 = C5 + 1;
const square E5 = D5 + 1;
const square F5 = E5 + 1;
const square G5 = F5 + 1;
const square H5 = G5 + 1;

const square A6 = H5 + 1;
const square B6 = A6 + 1;
const square C6 = B6 + 1;
const square D6 = C6 + 1;
const square E6 = D6 + 1;
const square F6 = E6 + 1;
const square G6 = F6 + 1;
const square H6 = G6 + 1;

const square A7 = H6 + 1;
const square B7 = A7 + 1;
const square C7 = B7 + 1;
const square D7 = C7 + 1;
const square E7 = D7 + 1;
const square F7 = E7 + 1;
const square G7 = F7 + 1;
const square H7 = G7 + 1;

const square A8 = H7 + 1;
const square B8 = A8 + 1;
const square C8 = B8 + 1;
const square D8 = C8 + 1;
const square E8 = D8 + 1;
const square F8 = E8 + 1;
const square G8 = F8 + 1;
const square H8 = G8 + 1;

const castle_flag WHITEK = 1;
const castle_flag WHITEQ = 2;
const castle_flag BLACKK = 4;
const castle_flag BLACKQ = 8;
const castle_flag ALL_CASTLE_FLAGS = WHITEK | WHITEQ | BLACKK | BLACKQ;

const direction NW = 7;
const direction N = 8;
const direction NE = 9;
const direction E = 1;
const direction SE = 7;
const direction S = 8;
const direction SW = 9;
const direction W = 1;
const direction NN = 16;
const direction SS = 16;

const int KING_SPAN_CENTER = 14;

// 0 0 0 0 0 1 1 1
// 0 0 0 0 0 1 0 1
// 0 0 0 0 0 1 1 1
struct KingSpan {
    bitboard span = 0;

    KingSpan() {
        span ^= ONE_BIT << (KING_SPAN_CENTER + NW);
        span ^= ONE_BIT << (KING_SPAN_CENTER + N);
        span ^= ONE_BIT << (KING_SPAN_CENTER + NE);
        span ^= ONE_BIT << (KING_SPAN_CENTER - E);
        span ^= ONE_BIT << (KING_SPAN_CENTER - SE);
        span ^= ONE_BIT << (KING_SPAN_CENTER - S);
        span ^= ONE_BIT << (KING_SPAN_CENTER - SW);
        span ^= ONE_BIT << (KING_SPAN_CENTER + W);
    }

};

const int KNIGHT_SPAN_CENTER = 21;
struct KnightSpan {
    bitboard span = 0;

    // 0 0 0 0 0 0 0 0
    // 0 0 0 0 0 0 0 0
    // 0 0 0 0 0 0 0 0
    // 0 0 0 0 1 0 1 0
    // 0 0 0 1 0 0 0 1
    // 0 0 0 0 0 x 0 0
    // 0 0 0 1 0 0 0 1
    // 0 0 0 0 1 0 1 0
    KnightSpan()
    {
        span ^= ONE_BIT << (KNIGHT_SPAN_CENTER - 2 + N);
        span ^= ONE_BIT << (KNIGHT_SPAN_CENTER - 2 - S);
        span ^= ONE_BIT << (KNIGHT_SPAN_CENTER + NN - 1);
        span ^= ONE_BIT << (KNIGHT_SPAN_CENTER + NN + 1);
        span ^= ONE_BIT << (KNIGHT_SPAN_CENTER + 2 + N);
        span ^= ONE_BIT << (KNIGHT_SPAN_CENTER + 2 - S);
        span ^= ONE_BIT << (KNIGHT_SPAN_CENTER - 1 - SS);
        span ^= ONE_BIT << (KNIGHT_SPAN_CENTER + 1 - SS);
    }

};

const KnightSpan ns;
const KingSpan ks;

const bitboard KNIGHT_SPAN = ns.span;
const bitboard KING_SPAN = ks.span;

const move_flag NO_MOVE_FLAGS = 0;
const move_flag DOUBLE_PAWN_PUSH = 1;
const move_flag KING_CASTLE = 2;
const move_flag QUEEN_CASTLE = 3;
const move_flag CAPTURE = 4;
const move_flag EP_CAPTURE = 5;
const move_flag KNIGHT_PROMO = 8;
const move_flag BISHOP_PROMO = 9;
const move_flag ROOK_PROMO = 10;
const move_flag QUEEN_PROMO = 11;
const move_flag KNIGHT_PROMO_CAPTURE = 12;
const move_flag BISHOP_PROMO_CAPTURE = 13;
const move_flag ROOK_PROMO_CAPTURE = 14;
const move_flag QUEEN_PROMO_CAPTURE = 15;

const bitboard rank_EP_masks[65] =
{
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,  0ULL, 0ULL,
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,  0ULL, 0ULL,
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,  0ULL, 0ULL,
    ONE_BIT << B4,
    ONE_BIT << A4 | ONE_BIT << C4,
    ONE_BIT << B4 | ONE_BIT << D4,
    ONE_BIT << C4 | ONE_BIT << E4,
    ONE_BIT << D4 | ONE_BIT << F4,
    ONE_BIT << E4 | ONE_BIT << G4,
    ONE_BIT << F4 | ONE_BIT << H4,
    ONE_BIT << G4,
    ONE_BIT << B5,
    ONE_BIT << A5 | ONE_BIT << C5,
    ONE_BIT << B5 | ONE_BIT << D5,
    ONE_BIT << C5 | ONE_BIT << E5,
    ONE_BIT << D5 | ONE_BIT << F5,
    ONE_BIT << E5 | ONE_BIT << G5,
    ONE_BIT << F5 | ONE_BIT << H5,
    ONE_BIT << G5,
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,  0ULL, 0ULL,
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,  0ULL, 0ULL,
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,  0ULL, 0ULL,
    0ULL,
};

const std::array<int, 64> fenIndexToSquare = {
	A8, B8, C8, D8, E8, F8, G8, H8,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A1, B1, C1, D1, E1, F1, G1, H1,
};

const std::array<std::string, 64> squareToString= {
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

const std::array<std::string, 7> pieceToString= {
    "",
    "Pawn",
    "blkas",
    "adsf",
    "asdf",
    "asdf",
    "asdf",
};


