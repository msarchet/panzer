#pragma once
typedef uint64_t square;
typedef uint8_t piece;
typedef bool color;
typedef uint8_t move_flag;
typedef uint8_t castle_flag;
typedef uint64_t hash;
typedef uint64_t bitboard;
typedef uint64_t mask;
const square NO_SQUARE = 0;
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

const color WHITE = 0;
const color BLACK = 1;
const piece PAWN = 2;
const piece ROOK = 4;
const piece KNIGHT = 8;
const piece BISHOP = 16;
const piece QUEEN = 32;
const piece KING = 64;

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


const int NW = 7;
const int N = 8;
const int NE = 9;
const int E = 1;
const int SE = 7;
const int S = 8;
const int SW = 9;
const int W = 1;
const int NN = 16;
const int SS = 16;

const int KING_SPAN_CENTER = 9;

struct KingSpan {
    bitboard span = 0;

    KingSpan() {
        span ^= ONE_BIT << (9 + NW);
        span ^= ONE_BIT << (9 + N);
        span ^= ONE_BIT << (9 + NE);
        span ^= ONE_BIT << (9 + E);
        span ^= ONE_BIT << (9 - SE);
        span ^= ONE_BIT << (9 - S);
        span ^= ONE_BIT << (9 - SW);
        span ^= ONE_BIT << (9 + W);
    }
};

const int KNIGHT_SPAN_CENTER = 18;
struct KnightSpan {
    bitboard span = 0;

    KnightSpan()
    {
        span ^= ONE_BIT << (18 - 2 + N);
        span ^= ONE_BIT << (18 - 2 - S);
        span ^= ONE_BIT << (18 + NN - 1);
        span ^= ONE_BIT << (18 + NN + 1);
        span ^= ONE_BIT << (18 + 2 + N);
        span ^= ONE_BIT << (18 + 2 - S);
        span ^= ONE_BIT << (18 - 1 - SS);
        span ^= ONE_BIT << (18 + 1 - SS);
    }

};

const move_flag NO_FLAGS = 0;
const move_flag CASTLE = 2;
const move_flag EP = 4;
const move_flag PROMOTION = 8;
const move_flag CAPTURE = 16;

