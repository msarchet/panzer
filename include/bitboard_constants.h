#pragma once

#include <array>
#include <vector>
#include <memory>
#include <string>
#include <iostream>

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
constexpr auto KIWIPETE = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";

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

const bitboard knight_move_masks[8] = 
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

const mask BLACKK_CASTLE_MASK = ((ONE_BIT << F8) | (ONE_BIT << G8));
const mask BLACKQ_CASTLE_MASK = ((ONE_BIT << B8) | (ONE_BIT << C8) | (ONE_BIT << D8));
const mask WHITEK_CASTLE_MASK = ((ONE_BIT << F1) | (ONE_BIT << G1));
const mask WHITEQ_CASTLE_MASK = ((ONE_BIT << B1) | (ONE_BIT << C1) | (ONE_BIT << D1));

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

const bitboard KING_SPANS[64] = {
    (KING_SPAN >> (KING_SPAN_CENTER - A1)) & king_move_masks[0],
    (KING_SPAN >> (KING_SPAN_CENTER - B1)) & king_move_masks[1],
    (KING_SPAN >> (KING_SPAN_CENTER - C1)) & king_move_masks[2],
    (KING_SPAN >> (KING_SPAN_CENTER - D1)) & king_move_masks[3],
    (KING_SPAN >> (KING_SPAN_CENTER - E1)) & king_move_masks[4],
    (KING_SPAN >> (KING_SPAN_CENTER - F1)) & king_move_masks[5],
    (KING_SPAN >> (KING_SPAN_CENTER - G1)) & king_move_masks[6],
    (KING_SPAN >> (KING_SPAN_CENTER - H1)) & king_move_masks[7],
    (KING_SPAN >> (KING_SPAN_CENTER - A2)) & king_move_masks[0],
    (KING_SPAN >> (KING_SPAN_CENTER - B2)) & king_move_masks[1],
    (KING_SPAN >> (KING_SPAN_CENTER - C2)) & king_move_masks[2],
    (KING_SPAN >> (KING_SPAN_CENTER - D2)) & king_move_masks[3],
    (KING_SPAN >> (KING_SPAN_CENTER - E2)) & king_move_masks[4],
    (KING_SPAN >> (KING_SPAN_CENTER - F2)) & king_move_masks[5],
    (KING_SPAN >> (KING_SPAN_CENTER - G2)) & king_move_masks[6],
    (KING_SPAN << (H2 - KING_SPAN_CENTER)) & king_move_masks[7],
    (KING_SPAN << (A3 - KING_SPAN_CENTER)) & king_move_masks[0],
    (KING_SPAN << (B3 - KING_SPAN_CENTER)) & king_move_masks[1],
    (KING_SPAN << (C3 - KING_SPAN_CENTER)) & king_move_masks[2],
    (KING_SPAN << (D3 - KING_SPAN_CENTER)) & king_move_masks[3],
    (KING_SPAN << (E3 - KING_SPAN_CENTER)) & king_move_masks[4],
    (KING_SPAN << (F3 - KING_SPAN_CENTER)) & king_move_masks[5],
    (KING_SPAN << (G3 - KING_SPAN_CENTER)) & king_move_masks[6],
    (KING_SPAN << (H3 - KING_SPAN_CENTER)) & king_move_masks[7],
    (KING_SPAN << (A4 - KING_SPAN_CENTER)) & king_move_masks[0],
    (KING_SPAN << (B4 - KING_SPAN_CENTER)) & king_move_masks[1],
    (KING_SPAN << (C4 - KING_SPAN_CENTER)) & king_move_masks[2],
    (KING_SPAN << (D4 - KING_SPAN_CENTER)) & king_move_masks[3],
    (KING_SPAN << (E4 - KING_SPAN_CENTER)) & king_move_masks[4],
    (KING_SPAN << (F4 - KING_SPAN_CENTER)) & king_move_masks[5],
    (KING_SPAN << (G4 - KING_SPAN_CENTER)) & king_move_masks[6],
    (KING_SPAN << (H4 - KING_SPAN_CENTER)) & king_move_masks[7],
    (KING_SPAN << (A5 - KING_SPAN_CENTER)) & king_move_masks[0],
    (KING_SPAN << (B5 - KING_SPAN_CENTER)) & king_move_masks[1],
    (KING_SPAN << (C5 - KING_SPAN_CENTER)) & king_move_masks[2],
    (KING_SPAN << (D5 - KING_SPAN_CENTER)) & king_move_masks[3],
    (KING_SPAN << (E5 - KING_SPAN_CENTER)) & king_move_masks[4],
    (KING_SPAN << (F5 - KING_SPAN_CENTER)) & king_move_masks[5],
    (KING_SPAN << (G5 - KING_SPAN_CENTER)) & king_move_masks[6],
    (KING_SPAN << (H5 - KING_SPAN_CENTER)) & king_move_masks[7],
    (KING_SPAN << (A6 - KING_SPAN_CENTER)) & king_move_masks[0],
    (KING_SPAN << (B6 - KING_SPAN_CENTER)) & king_move_masks[1],
    (KING_SPAN << (C6 - KING_SPAN_CENTER)) & king_move_masks[2],
    (KING_SPAN << (D6 - KING_SPAN_CENTER)) & king_move_masks[3],
    (KING_SPAN << (E6 - KING_SPAN_CENTER)) & king_move_masks[4],
    (KING_SPAN << (F6 - KING_SPAN_CENTER)) & king_move_masks[5],
    (KING_SPAN << (G6 - KING_SPAN_CENTER)) & king_move_masks[6],
    (KING_SPAN << (H6 - KING_SPAN_CENTER)) & king_move_masks[7],
    (KING_SPAN << (A7 - KING_SPAN_CENTER)) & king_move_masks[0],
    (KING_SPAN << (B7 - KING_SPAN_CENTER)) & king_move_masks[1],
    (KING_SPAN << (C7 - KING_SPAN_CENTER)) & king_move_masks[2],
    (KING_SPAN << (D7 - KING_SPAN_CENTER)) & king_move_masks[3],
    (KING_SPAN << (E7 - KING_SPAN_CENTER)) & king_move_masks[4],
    (KING_SPAN << (F7 - KING_SPAN_CENTER)) & king_move_masks[5],
    (KING_SPAN << (G7 - KING_SPAN_CENTER)) & king_move_masks[6],
    (KING_SPAN << (H7 - KING_SPAN_CENTER)) & king_move_masks[7],
    (KING_SPAN << (A8 - KING_SPAN_CENTER)) & king_move_masks[0],
    (KING_SPAN << (B8 - KING_SPAN_CENTER)) & king_move_masks[1],
    (KING_SPAN << (C8 - KING_SPAN_CENTER)) & king_move_masks[2],
    (KING_SPAN << (D8 - KING_SPAN_CENTER)) & king_move_masks[3],
    (KING_SPAN << (E8 - KING_SPAN_CENTER)) & king_move_masks[4],
    (KING_SPAN << (F8 - KING_SPAN_CENTER)) & king_move_masks[5],
    (KING_SPAN << (G8 - KING_SPAN_CENTER)) & king_move_masks[6],
    (KING_SPAN << (H8 - KING_SPAN_CENTER)) & king_move_masks[7],
};

const bitboard KNIGHT_SPANS[64] = {
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - A1)) & knight_move_masks[0],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - B1)) & knight_move_masks[1],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - C1)) & knight_move_masks[2],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - D1)) & knight_move_masks[3],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - E1)) & knight_move_masks[4],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - F1)) & knight_move_masks[5],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - G1)) & knight_move_masks[6],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - H1)) & knight_move_masks[7],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - A2)) & knight_move_masks[0],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - B2)) & knight_move_masks[1],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - C2)) & knight_move_masks[2],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - D2)) & knight_move_masks[3],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - E2)) & knight_move_masks[4],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - F2)) & knight_move_masks[5],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - G2)) & knight_move_masks[6],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - H2)) & knight_move_masks[7],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - A3)) & knight_move_masks[0],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - B3)) & knight_move_masks[1],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - C3)) & knight_move_masks[2],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - D3)) & knight_move_masks[3],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - E3)) & knight_move_masks[4],
    (KNIGHT_SPAN >> (KNIGHT_SPAN_CENTER - F3)) & knight_move_masks[5],
    (KNIGHT_SPAN << (G3 - KNIGHT_SPAN_CENTER)) & knight_move_masks[6],
    (KNIGHT_SPAN << (H3 - KNIGHT_SPAN_CENTER)) & knight_move_masks[7],
    (KNIGHT_SPAN << (A4 - KNIGHT_SPAN_CENTER)) & knight_move_masks[0],
    (KNIGHT_SPAN << (B4 - KNIGHT_SPAN_CENTER)) & knight_move_masks[1],
    (KNIGHT_SPAN << (C4 - KNIGHT_SPAN_CENTER)) & knight_move_masks[2],
    (KNIGHT_SPAN << (D4 - KNIGHT_SPAN_CENTER)) & knight_move_masks[3],
    (KNIGHT_SPAN << (E4 - KNIGHT_SPAN_CENTER)) & knight_move_masks[4],
    (KNIGHT_SPAN << (F4 - KNIGHT_SPAN_CENTER)) & knight_move_masks[5],
    (KNIGHT_SPAN << (G4 - KNIGHT_SPAN_CENTER)) & knight_move_masks[6],
    (KNIGHT_SPAN << (H4 - KNIGHT_SPAN_CENTER)) & knight_move_masks[7],
    (KNIGHT_SPAN << (A5 - KNIGHT_SPAN_CENTER)) & knight_move_masks[0],
    (KNIGHT_SPAN << (B5 - KNIGHT_SPAN_CENTER)) & knight_move_masks[1],
    (KNIGHT_SPAN << (C5 - KNIGHT_SPAN_CENTER)) & knight_move_masks[2],
    (KNIGHT_SPAN << (D5 - KNIGHT_SPAN_CENTER)) & knight_move_masks[3],
    (KNIGHT_SPAN << (E5 - KNIGHT_SPAN_CENTER)) & knight_move_masks[4],
    (KNIGHT_SPAN << (F5 - KNIGHT_SPAN_CENTER)) & knight_move_masks[5],
    (KNIGHT_SPAN << (G5 - KNIGHT_SPAN_CENTER)) & knight_move_masks[6],
    (KNIGHT_SPAN << (H5 - KNIGHT_SPAN_CENTER)) & knight_move_masks[7],
    (KNIGHT_SPAN << (A6 - KNIGHT_SPAN_CENTER)) & knight_move_masks[0],
    (KNIGHT_SPAN << (B6 - KNIGHT_SPAN_CENTER)) & knight_move_masks[1],
    (KNIGHT_SPAN << (C6 - KNIGHT_SPAN_CENTER)) & knight_move_masks[2],
    (KNIGHT_SPAN << (D6 - KNIGHT_SPAN_CENTER)) & knight_move_masks[3],
    (KNIGHT_SPAN << (E6 - KNIGHT_SPAN_CENTER)) & knight_move_masks[4],
    (KNIGHT_SPAN << (F6 - KNIGHT_SPAN_CENTER)) & knight_move_masks[5],
    (KNIGHT_SPAN << (G6 - KNIGHT_SPAN_CENTER)) & knight_move_masks[6],
    (KNIGHT_SPAN << (H6 - KNIGHT_SPAN_CENTER)) & knight_move_masks[7],
    (KNIGHT_SPAN << (A7 - KNIGHT_SPAN_CENTER)) & knight_move_masks[0],
    (KNIGHT_SPAN << (B7 - KNIGHT_SPAN_CENTER)) & knight_move_masks[1],
    (KNIGHT_SPAN << (C7 - KNIGHT_SPAN_CENTER)) & knight_move_masks[2],
    (KNIGHT_SPAN << (D7 - KNIGHT_SPAN_CENTER)) & knight_move_masks[3],
    (KNIGHT_SPAN << (E7 - KNIGHT_SPAN_CENTER)) & knight_move_masks[4],
    (KNIGHT_SPAN << (F7 - KNIGHT_SPAN_CENTER)) & knight_move_masks[5],
    (KNIGHT_SPAN << (G7 - KNIGHT_SPAN_CENTER)) & knight_move_masks[6],
    (KNIGHT_SPAN << (H7 - KNIGHT_SPAN_CENTER)) & knight_move_masks[7],
    (KNIGHT_SPAN << (A8 - KNIGHT_SPAN_CENTER)) & knight_move_masks[0],
    (KNIGHT_SPAN << (B8 - KNIGHT_SPAN_CENTER)) & knight_move_masks[1],
    (KNIGHT_SPAN << (C8 - KNIGHT_SPAN_CENTER)) & knight_move_masks[2],
    (KNIGHT_SPAN << (D8 - KNIGHT_SPAN_CENTER)) & knight_move_masks[3],
    (KNIGHT_SPAN << (E8 - KNIGHT_SPAN_CENTER)) & knight_move_masks[4],
    (KNIGHT_SPAN << (F8 - KNIGHT_SPAN_CENTER)) & knight_move_masks[5],
    (KNIGHT_SPAN << (G8 - KNIGHT_SPAN_CENTER)) & knight_move_masks[6],
    (KNIGHT_SPAN << (H8 - KNIGHT_SPAN_CENTER)) & knight_move_masks[7],
};


const move_flag NO_MOVE_FLAGS =         0b00000000;
const move_flag DOUBLE_PAWN_PUSH =      0b00000001;
const move_flag CASTLE =                0b00000010;
const move_flag CAPTURE =               0b00000100;
const move_flag EP_CAPTURE =            0b00001100;
const move_flag KNIGHT_PROMO =          0b00010000;
const move_flag BISHOP_PROMO =          0b00100000;
const move_flag ROOK_PROMO =            0b01000000;
const move_flag QUEEN_PROMO =           0b10000000;
const move_flag KNIGHT_PROMO_CAPTURE =  0b00010100;
const move_flag BISHOP_PROMO_CAPTURE =  0b00100100;
const move_flag ROOK_PROMO_CAPTURE =    0b01000100;
const move_flag QUEEN_PROMO_CAPTURE =   0b10000100;

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

inline int stringToSquare(std::string square)
{
    int i = 0;
    for (auto value : squareToString)
    {
        if (value == square)
        {
            return i;
        }

        i++;
    }

    return -1;
};

const std::array<std::string, 7> pieceToString= {
    "",
    "Pawn",
    "Rook",
    "Knight",
    "Bishop",
    "King",
    "Queen",
};

namespace Panzer 
{
    struct Move
    {
        square m_from;
        square m_to;
        move_flag m_move_flags;
        piece capturedPiece = NO_PIECE;
        castle_flag priorCastleFlags = EMPTY_CASTLE_FLAGS;
        square priorEP = NO_SQUARE;
        int m_score = 0;

        Move(square from, square to, move_flag flags, castle_flag castleFlags, piece captured = NO_PIECE, square epSquare = NO_SQUARE, int score = 0)
        {
            m_from = from;
            m_to = to;
            m_move_flags = flags;
            priorCastleFlags = castleFlags;
            capturedPiece = captured;
            priorEP = epSquare;
            m_score = score;
        }

        Move(const Move &move)
        {
            m_from = move.getFrom();
            m_to = move.getTo();
            m_move_flags = move.getFlags();
            priorCastleFlags = move.getCastleFlags();
            capturedPiece = move.getCapturedPiece();
            priorEP = move.getPriorEPSquare();
            m_score = move.m_score;
        }

        square getTo() const { return m_to; }
        square getFrom() const { return m_from; }
        move_flag getFlags() const { return m_move_flags; }

        bool isCapture() const { return (getFlags() & CAPTURE) != 0; }
        bool isEPCapture() const { return getFlags() == EP_CAPTURE; }
        bool isCastle() const { return getFlags() == CASTLE; }
        bool isPromo() const { return getFlags() >= KNIGHT_PROMO; }

        castle_flag getCastleFlags() const { return priorCastleFlags; }
        piece getCapturedPiece() const { return capturedPiece; }
        square getPriorEPSquare() const { return priorEP; }
    };
}


typedef std::shared_ptr<std::vector<Panzer::Move> > MoveVector;
