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

struct EvalScores {
  int mid = 0;
  int end = 0;
};

// NO, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
const piece_score pieceSquareScoreMid[7] = { 0, 124, 781, 825, 1276, 2538, 0 };
const piece_score pieceSquareScoreEnd[7] = { 0, 206, 853, 915, 1380, 2682, 0 };

template <bool isMid>
static constexpr piece_score GetPieceScore(piece p)
{
    if constexpr (isMid) return pieceSquareScoreMid[p];
    return pieceSquareScoreEnd[p];
}

const piece_score non_pawn_bonus_mg[5][8][4] = {
    {{-175,-92,-74,-73},{-77,-41,-27,-15},{-61,-17,6,12},{-35,8,40,49},{-34,13,44,51},{-9,22,58,53},{-67,-27,4,37},{-201,-83,-56,-26}}, // KNIGHT
    {{-53,-5,-8,-23},{-15,8,19,4},{-7,21,-5,17},{-5,11,25,39},{-12,29,22,31},{-16,6,1,11},{-17,-14,5,0},{-48,1,-14,-23}}, // BISHOP
    {{-31,-20,-14,-5},{-21,-13,-8,6},{-25,-11,-1,3},{-13,-5,-4,-6},{-27,-15,-4,3},{-22,-2,6,12},{-2,12,16,18},{-17,-19,-1,9}}, // ROOK
    {{3,-5,-5,4},{-3,5,8,12},{-3,6,13,7},{4,5,9,8},{0,14,12,5},{-4,10,6,8},{-5,6,10,8},{-2,-2,1,-2}}, // QUEEN
    {{271,327,271,198},{278,303,234,179},{195,258,169,120},{164,190,138,98},{154,179,105,70},{123,145,81,31},{88,120,65,33},{59,89,45,-1}} // KING
  }; 

const piece_score non_pawn_bonus_eg[5][8][4] = {
    { {-96,-65,-49,-21},{-67,-54,-18,8},{-40,-27,-8,29},{-35,-2,13,28},{-45,-16,9,39},{-51,-44,-16,17},{-69,-50,-51,12},{-100, -88,-56,-17} }, // KNIGHt
    { {-57,-30,-37,-12},{-37,-13,-17,1},{-16,-1,-2,10},{-20,-6,0,17},{-17,-1,-14,15},{-30,6,4,6},{-31,-20,-1,1},{-46,-42,-37,-24} }, //BISHOP
    {{-9,-13,-10,-9},{-12,-9,-1,-2},{6,-8,-2,-6},{-6,1,-9,7},{-5,8,7,-6},{6,1,-7,10},{4,5,20,-5},{18,0,19,13}}, // ROOK
    {{-69,-57,-47,-26},{-55,-31,-22,-4},{-39,-18,-9,3},{-23,-3,13,24},{-29,-6,9,21},{-38,-18,-12,1},{-50,-27,-24,-8},{-75,-52,-43,-36}}, // QUEEN
    {{1,45,85,76},{53,100,133,135},{88,130,169,175},{103,156,172,172},{96,166,199,199},{92,172,184,191},{47,121,116,131},{11,59,73,78}} // KING
  };

const piece_score pawn_bonus_mg[8][8] =
    {
      {0,0,0,0,0,0,0,0},
      {3,3,10,19,16,19,7,-5},
      {-9,-15,11,15,32,22,5,-22},
      {-4,-23,6,20,40,17,4,-8},
      {13,0,-13,1,11,-2,-13,5},
      {5,-12,-7,22,-8,-5,-15,-8},
      {-7,7,-3,-13,5,-16,10,-8},
      {0,0,0,0,0,0,0,0}
    };

const piece_score pawn_bonus_eg[8][8] = { 
    {0,0,0,0,0,0,0,0},
    {-10,-6,10,0,14,7,-5,-19},
    {-10,-10,-10,4,4,3,-6,-4},
    {6,-2,-8,-4,-13,-12,-10,-9},
    {10,5,4,-5,-5,-5,14,9},
    {28,20,21,28,30,7,6,13},
    {0,-11,12,21,25,19,4,7},
    {0,0,0,0,0,0,0,0} 
  };

const piece_score QUEEN_MOBILITY_BONUS[2][28] {
  {-30,-12,-8,-9,20,23,23,35,38,53,64,65,65,66,67,67,72,72,77,79,93,108,108,108,110,114,114,116},
  { -48,-30,-7,19,40,55,59,75,78,96,96,100,121,127,131,133,136,141,147,150,151,168,168,171,182,182,192,219}
};


const piece_score ROOK_MOBILITY_BONUS[2][15] {
  {-60,-20,2,3,3,11,22,31,40,40,41,48,57,57,62},
  {-78,-17,23,39,70,99,103,121,134,139,158,164,168,169,172},
};

const piece_score BISHOP_MOBILITY_BONUS[2][14] {
  {-48,-20,16,26,38,51,55,63,63,68,81,81,91,98},
  {-59,-23,-3,13,24,42,54,57,65,73,78,86,88,97}
};

const piece_score KNIGHT_MOBILITY_BONUS[2][9] {
  {-62,-53,-12,-4,3,13,22,28,33},
  {-81,-56,-31,-16,5,11,17,20,25}
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

const square index_black[64] = {
  A8, B8, C8, D8, E8, F8, G8, H8,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A1, B1, C1, D1, E1, F1, G1, H1
};

const square index_white[64] = {
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

const mask PAWN_SUPPORT_MASK[64] = {
  0, 0, 0, 0, 0, 0, 0, 0,         
  0, 0, 0, 0, 0, 0, 0, 0,         
  (ONE_BIT << B2),                
  (ONE_BIT << A2 | ONE_BIT << C2),
  (ONE_BIT << B2 | ONE_BIT << D2),
  (ONE_BIT << C2 | ONE_BIT << E2),
  (ONE_BIT << D2 | ONE_BIT << F2),
  (ONE_BIT << E2 | ONE_BIT << G2),
  (ONE_BIT << F2 | ONE_BIT << H2),
  (ONE_BIT << G2),                
  (ONE_BIT << B3),                
  (ONE_BIT << A3 | ONE_BIT << C3),
  (ONE_BIT << B3 | ONE_BIT << D3),
  (ONE_BIT << C3 | ONE_BIT << E3),
  (ONE_BIT << D3 | ONE_BIT << F3),
  (ONE_BIT << E3 | ONE_BIT << G3),
  (ONE_BIT << F3 | ONE_BIT << H3),
  (ONE_BIT << G3),                
  (ONE_BIT << B4),                
  (ONE_BIT << A4 | ONE_BIT << C4),
  (ONE_BIT << B4 | ONE_BIT << D4),
  (ONE_BIT << C4 | ONE_BIT << E4),
  (ONE_BIT << D4 | ONE_BIT << F4),
  (ONE_BIT << E4 | ONE_BIT << G4),
  (ONE_BIT << F4 | ONE_BIT << H4),
  (ONE_BIT << G4),                
  (ONE_BIT << B5),                
  (ONE_BIT << A5 | ONE_BIT << C5),
  (ONE_BIT << B5 | ONE_BIT << D5),
  (ONE_BIT << C5 | ONE_BIT << E5),
  (ONE_BIT << D5 | ONE_BIT << F5),
  (ONE_BIT << E5 | ONE_BIT << G5),
  (ONE_BIT << F5 | ONE_BIT << H5),
  (ONE_BIT << G5),                
  (ONE_BIT << B6),                
  (ONE_BIT << A6 | ONE_BIT << C6),
  (ONE_BIT << B6 | ONE_BIT << D6),
  (ONE_BIT << C6 | ONE_BIT << E6),
  (ONE_BIT << D6 | ONE_BIT << F6),
  (ONE_BIT << E6 | ONE_BIT << G6),
  (ONE_BIT << F6 | ONE_BIT << H6),
  (ONE_BIT << G6),                
  0, 0, 0 ,0 ,0 ,0 ,0 ,0
};

const mask WHITE_PAWN_ATTACKS[64] {
  0, 0, 0, 0, 0, 0, 0, 0,
  (ONE_BIT << B3),
  (ONE_BIT << A3) | (ONE_BIT << C3),
  (ONE_BIT << B3) | (ONE_BIT << D3),
  (ONE_BIT << C3) | (ONE_BIT << E3),
  (ONE_BIT << D3) | (ONE_BIT << F3),
  (ONE_BIT << E3) | (ONE_BIT << G3),
  (ONE_BIT << F3) | (ONE_BIT << H3),
  (ONE_BIT << G3),
  (ONE_BIT << B4),
  (ONE_BIT << A4) | (ONE_BIT << C4),
  (ONE_BIT << B4) | (ONE_BIT << D4),
  (ONE_BIT << C4) | (ONE_BIT << E4),
  (ONE_BIT << D4) | (ONE_BIT << F4),
  (ONE_BIT << E4) | (ONE_BIT << G4),
  (ONE_BIT << F4) | (ONE_BIT << H4),
  (ONE_BIT << G4),
  (ONE_BIT << B5),
  (ONE_BIT << A5) | (ONE_BIT << C5),
  (ONE_BIT << B5) | (ONE_BIT << D5),
  (ONE_BIT << C5) | (ONE_BIT << E5),
  (ONE_BIT << D5) | (ONE_BIT << F5),
  (ONE_BIT << E5) | (ONE_BIT << G5),
  (ONE_BIT << F5) | (ONE_BIT << H5),
  (ONE_BIT << G5),
  (ONE_BIT << B6),
  (ONE_BIT << A6) | (ONE_BIT << C6),
  (ONE_BIT << B6) | (ONE_BIT << D6),
  (ONE_BIT << C6) | (ONE_BIT << E6),
  (ONE_BIT << D6) | (ONE_BIT << F6),
  (ONE_BIT << E6) | (ONE_BIT << G6),
  (ONE_BIT << F6) | (ONE_BIT << H6),
  (ONE_BIT << G6),
  (ONE_BIT << B7),
  (ONE_BIT << A7) | (ONE_BIT << C7),
  (ONE_BIT << B7) | (ONE_BIT << D7),
  (ONE_BIT << C7) | (ONE_BIT << E7),
  (ONE_BIT << D7) | (ONE_BIT << F7),
  (ONE_BIT << E7) | (ONE_BIT << G7),
  (ONE_BIT << F7) | (ONE_BIT << H7),
  (ONE_BIT << G7),
  (ONE_BIT << A8) | (ONE_BIT << C8),
  (ONE_BIT << B8) | (ONE_BIT << D8),
  (ONE_BIT << C8) | (ONE_BIT << E8),
  (ONE_BIT << D8) | (ONE_BIT << F8),
  (ONE_BIT << E8) | (ONE_BIT << G8),
  (ONE_BIT << F8) | (ONE_BIT << H8),
  (ONE_BIT << G8),
  0, 0, 0, 0, 0, 0, 0, 0
};

const mask BLACK_PAWN_ATTACKS[64] {
  0, 0, 0, 0, 0, 0, 0, 0,
  (ONE_BIT << B6),
  (ONE_BIT << A6) | (ONE_BIT << C6),
  (ONE_BIT << B6) | (ONE_BIT << D6),
  (ONE_BIT << C6) | (ONE_BIT << E6),
  (ONE_BIT << D6) | (ONE_BIT << F6),
  (ONE_BIT << E6) | (ONE_BIT << G6),
  (ONE_BIT << F6) | (ONE_BIT << H6),
  (ONE_BIT << G6),
  (ONE_BIT << B5),
  (ONE_BIT << A5) | (ONE_BIT << C5),
  (ONE_BIT << B5) | (ONE_BIT << D5),
  (ONE_BIT << C5) | (ONE_BIT << E5),
  (ONE_BIT << D5) | (ONE_BIT << F5),
  (ONE_BIT << E5) | (ONE_BIT << G5),
  (ONE_BIT << F5) | (ONE_BIT << H5),
  (ONE_BIT << G5),
  (ONE_BIT << B5),
  (ONE_BIT << A4) | (ONE_BIT << C4),
  (ONE_BIT << B4) | (ONE_BIT << D4),
  (ONE_BIT << C4) | (ONE_BIT << E4),
  (ONE_BIT << D4) | (ONE_BIT << F4),
  (ONE_BIT << E4) | (ONE_BIT << G4),
  (ONE_BIT << F4) | (ONE_BIT << H4),
  (ONE_BIT << G4),
  (ONE_BIT << B4),
  (ONE_BIT << A3) | (ONE_BIT << C3),
  (ONE_BIT << B3) | (ONE_BIT << D3),
  (ONE_BIT << C3) | (ONE_BIT << E3),
  (ONE_BIT << D3) | (ONE_BIT << F3),
  (ONE_BIT << E3) | (ONE_BIT << G3),
  (ONE_BIT << F3) | (ONE_BIT << H3),
  (ONE_BIT << G3),
  (ONE_BIT << B2),
  (ONE_BIT << A2) | (ONE_BIT << C2),
  (ONE_BIT << B2) | (ONE_BIT << D2),
  (ONE_BIT << C2) | (ONE_BIT << E2),
  (ONE_BIT << D2) | (ONE_BIT << F2),
  (ONE_BIT << E2) | (ONE_BIT << G2),
  (ONE_BIT << F2) | (ONE_BIT << H2),
  (ONE_BIT << G2),
  (ONE_BIT << A1) | (ONE_BIT << C1),
  (ONE_BIT << B1) | (ONE_BIT << D1),
  (ONE_BIT << C1) | (ONE_BIT << E1),
  (ONE_BIT << D1) | (ONE_BIT << F1),
  (ONE_BIT << E1) | (ONE_BIT << G1),
  (ONE_BIT << F1) | (ONE_BIT << H1),
  (ONE_BIT << G1),
  0, 0, 0, 0, 0, 0, 0, 0
};

const square SPACE[2][12] {
  { C4, D4, E4, F4, C3, D3, E3, F3, C2, D2, E2, F2  },
  { C5, D5, E5, F5, C6, D6, E6, F6, C7, D7, E7, F7  }
};

const mask* PAWN_ATTACKS[2] = {
  WHITE_PAWN_ATTACKS ,
   BLACK_PAWN_ATTACKS 
};

const mask ADJACENT_FILE_MASKS[8] {
  B_FILE,
  A_FILE | C_FILE,
  B_FILE | D_FILE,
  C_FILE | E_FILE,
  D_FILE | F_FILE,
  E_FILE | G_FILE,
  F_FILE | H_FILE,
  G_FILE
};

const piece_score PAWN_CONNECTED_BONUS[8] = { 0, 7, 8, 12, 29, 48, 86, 0 };
const piece_score PAWN_BLOCKED[2] = { -11, -3 };

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

