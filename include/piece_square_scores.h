/*
	This file includes piece square scores
	currently borrowed from the CPW-Engine on 
	chessprogramming.org
*/
#pragma once

#include <stdint.h>
#include <climits>
#include "bitboard_constants.h"

typedef int16_t piece_score;

struct EvalData {
	piece_score MidGamePieceSquareScore[7][2][64];
	piece_score EndGamePieceSquareScore[7][2][64];
};

const piece_score PAWN_SCORE = 100;
const piece_score KNIGHT_SCORE = 300;
const piece_score BISHOP_SCORE = 340;
const piece_score ROOK_SCORE = 500;
const piece_score QUEEN_SCORE = 900;
const piece_score KING_SCORE = INT16_MAX;

const piece_score CAPTURE_SCORES[7] = {
	0,
	PAWN_SCORE,
	ROOK_SCORE,
	KNIGHT_SCORE,
	BISHOP_SCORE,
	QUEEN_SCORE,
	KING_SCORE
};

const piece_score CAPTURE_SCORES_AGGRESSOR[7] = {
	0,
	7,
	6,
	5,
	4,
	3,
  2
};

const piece_score SORT_PROMO = 500;
const piece_score SORT_CAPTURE = 250;

const piece_score BISHOP_PAIR = 30;
const piece_score KNIGHT_PAIR = 8;
const piece_score ROOK_PAIR = 16;

const piece_score KING_RING = 50;
const piece_score KING_RING_FRONT = 25;

const piece_score PAWN_RANK_SCORES[8] = 
{
	100, // 8
	50,  // 7
	30,  // 6
	22,  // 5
	15,  // 4
	10,  // 3
	3,   // 2
	0,   // 1
};

const piece_score PAWN_STACKED_SCORE[8] = {
	0, // 0 pawns
	0, // 1 pawn
	-20, // 2 pawns
	-50, // 3 pawns
	-200, // 4 pawns
	-200, // 5 pawns likely impossible from here down
	-200, // 6 pawns
	-200, // 7 pawns
};

const piece_score BLOCKED_PAWN = -75;
const piece_score BACKWARD_PAWN = -50;

const piece_score KNIGHT_ADJUSTMENTS[9] = { -20, -16, -12, -8, -4, 0, 4, 8 , 12 };
const piece_score ROOK_ADJUSTMENTS[9] = { 15, 12, 9, 6, 3, 0, -3, -6, -9 };

const piece_score PAWN_SCORE_SQUARE_MID[64] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  -6,  -4,   1,   1,   1,   1,  -4,  -6,
  -6,  -4,   1,   2,   2,   1,  -4,  -6,
  -6,  -4,   2,   8,   8,   2,  -4,  -6,
  -6,  -4,   5,  10,  10,   5,  -4,  -6,
  -4,  -4,   1,   5,   5,   1,  -4,  -4,
  -6,  -4,   1, -24,  -24,  1,  -4,  -6,
  0,   0,   0,   0,   0,   0,   0,   0
};

const piece_score PAWN_SCORE_SQUARE_END[64] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  -6,  -4,   1,   1,   1,   1,  -4,  -6,
  -6,  -4,   1,   2,   2,   1,  -4,  -6,
  -6,  -4,   2,   8,   8,   2,  -4,  -6,
  -6,  -4,   5,  10,  10,   5,  -4,  -6,
  -4,  -4,   1,   5,   5,   1,  -4,  -4,
  -6,  -4,   1, -24,  -24,  1,  -4,  -6,
  0,   0,   0,   0,   0,   0,   0,   0
};

const piece_score KNIGHT_SCORE_SQUARE_MID[64] = {
  -8,  -8,  -8,  -8,  -8,  -8,  -8,  -8,
  -8,   0,   0,   0,   0,   0,   0,  -8,
  -8,   0,   4,   4,   4,   4,   0,  -8,
  -8,   0,   4,   8,   8,   4,   0,  -8,
  -8,   0,   4,   8,   8,   4,   0,  -8,
  -8,   0,   4,   4,   4,   4,   0,  -8,
  -8,   0,   1,   2,   2,   1,   0,  -8,
  -8, -16,  -8,  -8,  -8,  -8, -16,  -8
};

const piece_score KNIGHT_SCORE_SQUARE_END[64] = {
  -8,  -8,  -8,  -8,  -8,  -8,  -8,  -8,
  -8,   0,   0,   0,   0,   0,   0,  -8,
  -8,   0,   4,   4,   4,   4,   0,  -8,
  -8,   0,   4,   8,   8,   4,   0,  -8,
  -8,   0,   4,   8,   8,   4,   0,  -8,
  -8,   0,   4,   4,   4,   4,   0,  -8,
  -8,   0,   1,   2,   2,   1,   0,  -8,
  -8, -16,  -8,  -8,  -8,  -8, -16,  -8
};

const piece_score BISHOP_SCORE_SQUARE_MID[64] = {
  -4,  -4,  -4,  -4,  -4,  -4,  -4,  -4,
  -4,   0,   0,   0,   0,   0,   0,  -4,
  -4,   0,   2,   4,   4,   2,   0,  -4,
  -4,   0,   4,   6,   6,   4,   0,  -4,
  -4,   0,   4,   6,   6,   4,   0,  -4,
  -4,   1,   2,   4,   4,   2,   1,  -4,
  -4,   2,   1,   1,   1,   1,   2,  -4,
  -4,  -4, -12,  -4,  -4, -12,  -4,  -4
};

const piece_score BISHOP_SCORE_SQUARE_END[64] = {
  -4,  -4,  -4,  -4,  -4,  -4,  -4,  -4,
  -4,   0,   0,   0,   0,   0,   0,  -4,
  -4,   0,   2,   4,   4,   2,   0,  -4,
  -4,   0,   4,   6,   6,   4,   0,  -4,
  -4,   0,   4,   6,   6,   4,   0,  -4,
  -4,   1,   2,   4,   4,   2,   1,  -4,
  -4,   2,   1,   1,   1,   1,   2,  -4,
  -4,  -4, -12,  -4,  -4, -12,  -4,  -4
};

const piece_score ROOK_SCORE_SQUARE_MID[64] = {
  5,   5,   5,   5,   5,   5,   5,   5,
  20,  20,  20,  20,  20,  20,  20,  20,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  0,   0,   0,   2,   2,   0,   0,   0
};

const piece_score ROOK_SCORE_SQUARE_END[64] = {
  5,   5,   5,   5,   5,   5,   5,   5,
  20,  20,  20,  20,  20,  20,  20,  20,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  0,   0,   0,   2,   2,   0,   0,   0
};

const piece_score QUEEN_SCORE_SQUARE_MID[64] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 1, 2, 2, 1, 0, 0,
  0, 0, 2, 3, 3, 2, 0, 0,
  0, 0, 2, 3, 3, 2, 0, 0,
  0, 0, 1, 2, 2, 1, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  -5, -5, -5, -5, -5, -5, -5, -5
};

const piece_score QUEEN_SCORE_SQUARE_END[64] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 1, 2, 2, 1, 0, 0,
  0, 0, 2, 3, 3, 2, 0, 0,
  0, 0, 2, 3, 3, 2, 0, 0,
  0, 0, 1, 2, 2, 1, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  -5, -5, -5, -5, -5, -5, -5, -5
};

const piece_score KING_SCORE_SQUARE_MID[64] = {
  -40, -30, -50, -70, -70, -50, -30, -40,
  -30, -20, -40, -60, -60, -40, -20, -30,
  -20, -10, -30, -50, -50, -30, -10, -20,
  -10,   0, -20, -40, -40, -20,   0, -10,
  0,  10, -10, -30, -30, -10,  10,   0,
  10,  20,   0, -20, -20,   0,  20,  10,
  30,  40,  20,   0,   0,  20,  40,  30,
  40,  100,  30,  10,  10,  30,  100,  40
};

const piece_score KING_SCORE_SQUARE_END[64] = {
  -72, -48, -36, -24, -24, -36, -48, -72,
  -48, -24, -12,   0,   0, -12, -24, -48,
  -36, -12,   0,  12,  12,   0, -12, -36,
  -24,   0,  12,  24,  24,  12,   0, -24,
  -24,   0,  12,  24,  24,  12,   0, -24,
  -36, -12,   0,  12,  12,   0, -12, -36,
  -48, -24, -12,   0,   0, -12, -24, -48,
  -72, -48, -36, -24, -24, -36, -48, -72
};

const square index_white[64] = {
  A8, B8, C8, D8, E8, F8, G8, H8,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A1, B1, C1, D1, E1, F1, G1, H1
};

const square index_black[64] = {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8
};

const mask IN_FRONT_OF_RING_KING = (ONE_BIT << A3) | (ONE_BIT << B3) | (ONE_BIT << C3);
const square RING_CENTER = B1;

const mask SQUARES_IN_FRONT_OF_KING_RING[64] = {
	(IN_FRONT_OF_RING_KING << (RING_CENTER - A1)) & king_move_masks[A1 % 8],
	(IN_FRONT_OF_RING_KING >> (B1 - RING_CENTER)) & king_move_masks[B1 % 8],
	(IN_FRONT_OF_RING_KING >> (C1 - RING_CENTER)) & king_move_masks[C1 % 8],
	(IN_FRONT_OF_RING_KING >> (D1 - RING_CENTER)) & king_move_masks[D1 % 8],
	(IN_FRONT_OF_RING_KING >> (E1 - RING_CENTER)) & king_move_masks[E1 % 8],
	(IN_FRONT_OF_RING_KING >> (F1 - RING_CENTER)) & king_move_masks[F1 % 8],
	(IN_FRONT_OF_RING_KING >> (G1 - RING_CENTER)) & king_move_masks[G1 % 8],
	(IN_FRONT_OF_RING_KING >> (H1 - RING_CENTER)) & king_move_masks[H1 % 8],
	(IN_FRONT_OF_RING_KING >> (A2 - RING_CENTER)) & king_move_masks[A2 % 8],
	(IN_FRONT_OF_RING_KING >> (B2 - RING_CENTER)) & king_move_masks[B2 % 8],
	(IN_FRONT_OF_RING_KING >> (C2 - RING_CENTER)) & king_move_masks[C2 % 8],
	(IN_FRONT_OF_RING_KING >> (D2 - RING_CENTER)) & king_move_masks[D2 % 8],
	(IN_FRONT_OF_RING_KING >> (E2 - RING_CENTER)) & king_move_masks[E2 % 8],
	(IN_FRONT_OF_RING_KING >> (F2 - RING_CENTER)) & king_move_masks[F2 % 8],
	(IN_FRONT_OF_RING_KING >> (G2 - RING_CENTER)) & king_move_masks[G2 % 8],
	(IN_FRONT_OF_RING_KING >> (H2 - RING_CENTER)) & king_move_masks[H2 % 8],
	(IN_FRONT_OF_RING_KING >> (A3 - RING_CENTER)) & king_move_masks[A3 % 8],
	(IN_FRONT_OF_RING_KING >> (B3 - RING_CENTER)) & king_move_masks[B3 % 8],
	(IN_FRONT_OF_RING_KING >> (C3 - RING_CENTER)) & king_move_masks[C3 % 8],
	(IN_FRONT_OF_RING_KING >> (D3 - RING_CENTER)) & king_move_masks[D3 % 8],
	(IN_FRONT_OF_RING_KING >> (E3 - RING_CENTER)) & king_move_masks[E3 % 8],
	(IN_FRONT_OF_RING_KING >> (F3 - RING_CENTER)) & king_move_masks[F3 % 8],
	(IN_FRONT_OF_RING_KING >> (G3 - RING_CENTER)) & king_move_masks[G3 % 8],
	(IN_FRONT_OF_RING_KING >> (H3 - RING_CENTER)) & king_move_masks[H3 % 8],
	(IN_FRONT_OF_RING_KING >> (A4 - RING_CENTER)) & king_move_masks[A4 % 8],
	(IN_FRONT_OF_RING_KING >> (B4 - RING_CENTER)) & king_move_masks[B4 % 8],
	(IN_FRONT_OF_RING_KING >> (C4 - RING_CENTER)) & king_move_masks[C4 % 8],
	(IN_FRONT_OF_RING_KING >> (D4 - RING_CENTER)) & king_move_masks[D4 % 8],
	(IN_FRONT_OF_RING_KING >> (E4 - RING_CENTER)) & king_move_masks[E4 % 8],
	(IN_FRONT_OF_RING_KING >> (F4 - RING_CENTER)) & king_move_masks[F4 % 8],
	(IN_FRONT_OF_RING_KING >> (G4 - RING_CENTER)) & king_move_masks[G4 % 8],
	(IN_FRONT_OF_RING_KING >> (H4 - RING_CENTER)) & king_move_masks[H4 % 8],
	(IN_FRONT_OF_RING_KING >> (A5 - RING_CENTER)) & king_move_masks[A5 % 8],
	(IN_FRONT_OF_RING_KING >> (B5 - RING_CENTER)) & king_move_masks[B5 % 8],
	(IN_FRONT_OF_RING_KING >> (C5 - RING_CENTER)) & king_move_masks[C5 % 8],
	(IN_FRONT_OF_RING_KING >> (D5 - RING_CENTER)) & king_move_masks[D5 % 8],
	(IN_FRONT_OF_RING_KING >> (E5 - RING_CENTER)) & king_move_masks[E5 % 8],
	(IN_FRONT_OF_RING_KING >> (F5 - RING_CENTER)) & king_move_masks[F5 % 8],
	(IN_FRONT_OF_RING_KING >> (G5 - RING_CENTER)) & king_move_masks[G5 % 8],
	(IN_FRONT_OF_RING_KING >> (H5 - RING_CENTER)) & king_move_masks[H5 % 8],
	(IN_FRONT_OF_RING_KING >> (A6 - RING_CENTER)) & king_move_masks[A6 % 8],
	(IN_FRONT_OF_RING_KING >> (B6 - RING_CENTER)) & king_move_masks[B6 % 8],
	(IN_FRONT_OF_RING_KING >> (C6 - RING_CENTER)) & king_move_masks[C6 % 8],
	(IN_FRONT_OF_RING_KING >> (D6 - RING_CENTER)) & king_move_masks[D6 % 8],
	(IN_FRONT_OF_RING_KING >> (E6 - RING_CENTER)) & king_move_masks[E6 % 8],
	(IN_FRONT_OF_RING_KING >> (F6 - RING_CENTER)) & king_move_masks[F6 % 8],
	(IN_FRONT_OF_RING_KING >> (G6 - RING_CENTER)) & king_move_masks[G6 % 8],
	(IN_FRONT_OF_RING_KING >> (H6 - RING_CENTER)) & king_move_masks[H6 % 8],
	(IN_FRONT_OF_RING_KING >> (A7 - RING_CENTER)) & king_move_masks[A7 % 8],
	(IN_FRONT_OF_RING_KING >> (B7 - RING_CENTER)) & king_move_masks[B7 % 8],
	(IN_FRONT_OF_RING_KING >> (C7 - RING_CENTER)) & king_move_masks[C7 % 8],
	(IN_FRONT_OF_RING_KING >> (D7 - RING_CENTER)) & king_move_masks[D7 % 8],
	(IN_FRONT_OF_RING_KING >> (E7 - RING_CENTER)) & king_move_masks[E7 % 8],
	(IN_FRONT_OF_RING_KING >> (F7 - RING_CENTER)) & king_move_masks[F7 % 8],
	(IN_FRONT_OF_RING_KING >> (G7 - RING_CENTER)) & king_move_masks[G7 % 8],
	(IN_FRONT_OF_RING_KING >> (H7 - RING_CENTER)) & king_move_masks[H7 % 8],
	(IN_FRONT_OF_RING_KING >> (A8 - RING_CENTER)) & king_move_masks[A8 % 8],
	(IN_FRONT_OF_RING_KING >> (B8 - RING_CENTER)) & king_move_masks[B8 % 8],
	(IN_FRONT_OF_RING_KING >> (C8 - RING_CENTER)) & king_move_masks[C8 % 8],
	(IN_FRONT_OF_RING_KING >> (D8 - RING_CENTER)) & king_move_masks[D8 % 8],
	(IN_FRONT_OF_RING_KING >> (E8 - RING_CENTER)) & king_move_masks[E8 % 8],
	(IN_FRONT_OF_RING_KING >> (F8 - RING_CENTER)) & king_move_masks[F8 % 8],
	(IN_FRONT_OF_RING_KING >> (G8 - RING_CENTER)) & king_move_masks[G8 % 8],
	(IN_FRONT_OF_RING_KING >> (H8 - RING_CENTER)) & king_move_masks[H8 % 8],
};

const bitboard CENTER_PAWN_MASKS[2] = {
  (ONE_BIT << C3) | (ONE_BIT << D3) | (ONE_BIT << E3) | (ONE_BIT << F3) | (ONE_BIT << C4) | (ONE_BIT << D4) | (ONE_BIT << E4) | (ONE_BIT << F4) | (ONE_BIT << D5) | (ONE_BIT << E5),
  (ONE_BIT << C6) | (ONE_BIT << D6) | (ONE_BIT << E6) | (ONE_BIT << F6) | (ONE_BIT << C5) | (ONE_BIT << D5) | (ONE_BIT << E5) | (ONE_BIT << F5) | (ONE_BIT << D4) | (ONE_BIT << E4),

};

