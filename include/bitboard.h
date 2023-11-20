#pragma once
#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <stdlib.h>
#include <vector>

#include "bitboard_constants.h"
#include "board_utils.h"
#include "sliders.h"
#include "zorbist.h"

namespace Panzer {

class Board {
private:
  std::array<bitboard, 7> Pieces{0ULL};
  std::array<bitboard, 2> Colors{0ULL};
  std::array<piece, 64> pieceLookup{NO_PIECE};
  std::vector<Move> moveChain{};
  color side_to_move = WHITE;
  square ep_square = NO_SQUARE;
  castle_flag castle_flags = (WHITEK | WHITEQ | BLACKK | BLACKQ);
  uint8_t ply = 1;
  uint8_t halfMoveClock = 0;
  uint8_t priorHalfMoveClock = 0;
  hash boardHash = 0;
  zorbist_lookup zorbist{};

public:
  Board() {}

  Board(const Board &board) : Board() {
    Pieces = board.Pieces;
    Colors = board.Colors;
    pieceLookup = board.pieceLookup;
    side_to_move = board.side_to_move;
    ep_square = board.ep_square;
    castle_flags = board.castle_flags;
    ply = board.ply;
    boardHash = board.boardHash;
    halfMoveClock = board.halfMoveClock;
    priorHalfMoveClock = board.priorHalfMoveClock;
  }

  inline void FillSquare(const square s, const piece p, const color c);
  inline void ClearSquare(const square s, const piece p, const color c);
  void PrintBoard();

  int GenerateMoves(Move *moves, bool captures = false);
  int GenerateWhiteMoves(Move *moves, bool captures = false);
  int GenerateBlackMoves(Move *moves, bool captures = false);

  template <color c> constexpr bitboard GetPieces() const {
    if constexpr (c == WHITE) {
      return Colors.at(WHITE);
    }

    return Colors.at(BLACK);
  }

  template <color c> constexpr bitboard GetPawns() const {
    if constexpr (c == WHITE) {
      return Colors.at(WHITE) & Pieces.at(PAWN);
    }

    return Colors.at(BLACK) & Pieces.at(PAWN);
  }

  template <color c> constexpr bitboard GetRooks() const {
    if constexpr (c == WHITE) {
      return Colors.at(WHITE) & Pieces.at(ROOK);
    }

    return Colors.at(BLACK) & Pieces.at(ROOK);
  }

  template <color c> constexpr bitboard GetKnights() const {
    if constexpr (c == WHITE) {
      return Colors.at(WHITE) & Pieces.at(KNIGHT);
    }

    return Colors.at(BLACK) & Pieces.at(KNIGHT);
  }

  template <color c> constexpr bitboard GetBishops() const {
    if constexpr (c == WHITE) {
      return Colors.at(WHITE) & Pieces.at(BISHOP);
    }

    return Colors.at(BLACK) & Pieces.at(BISHOP);
  }

  template <color c> constexpr bitboard GetQueens() const {
    if constexpr (c == WHITE) {
      return Colors.at(WHITE) & Pieces.at(QUEEN);
    }

    return Colors.at(BLACK) & Pieces.at(QUEEN);
  }

  template <color c> constexpr bitboard GetKings() const {
    if constexpr (c == WHITE) {
      return Colors.at(WHITE) & Pieces.at(KING);
    }

    return Colors.at(BLACK) & Pieces.at(KING);
  }

  constexpr bitboard GetOccupancy() const {
    return Colors.at(WHITE) | Colors.at(BLACK);
  }
  inline color GetSideToMove() const { return side_to_move; }

  inline piece GetPieceAtSquare(square s) const { return pieceLookup.at(s); };

  inline square GetEpSquare() { return ep_square; }
  inline int GetPly() { return ply; }
  inline int GetHalfClock() { return halfMoveClock; }
  inline bool isDrawBy50MoveRule() { return halfMoveClock == 100; }
  inline hash GetHash() { return boardHash; }

  void MakeMove(const Move &move);
  void UnmakeMove(const Move &move);
  inline void ToggleBitBoards(square from, square to, piece p, color c);

  bool IsSquareAttacked(square s, color color);
  bool IsChecked(color color);
  void FenToBoard(const std::string &fen);
  std::string PrintMoveChain();
  std::string BoardToFen();

private:
  int MakeWhitePawnMoves(Move *moves, int movecount, bool captures = false);
  int MakeWhiteRooksMoves(Move *moves, int movecount, bool captures = false);
  int MakeWhiteKnightMoves(Move *moves, int movecount, bool captures = false);
  int MakeWhiteBishopMoves(Move *moves, int movecount, bool captures = false);
  int MakeWhiteQueenMoves(Move *moves, int movecount, bool captures = false);
  int MakeWhiteKingMoves(Move *moves, int movecount, bool captures = false);
  int MakeBlackPawnMoves(Move *moves, int movecount, bool captures = false);
  int MakeBlackRooksMoves(Move *moves, int movecount, bool captures = false);
  int MakeBlackKnightMoves(Move *moves, int movecount, bool captures = false);
  int MakeBlackBishopMoves(Move *moves, int movecount, bool captures = false);
  int MakeBlackQueenMoves(Move *moves, int movecount, bool captures = false);
  int MakeBlackKingMoves(Move *moves, int movecount, bool captures = false);

  int MakeRookMoves(Move *moves, int movecount, bitboard rooks,
                    bitboard same_side, bitboard other_side,
                    bool captures = false);
  int MakeKnightMoves(Move *moves, int movecount, bitboard knights,
                      bitboard same_side, bitboard other_side,
                      bool captures = false);
  int MakeBishopMoves(Move *moves, int movecount, bitboard bishops,
                      bitboard same_side, bitboard other_side,
                      bool captures = false);
  int MakeQueenMoves(Move *moves, int movecount, bitboard queens,
                     bitboard same_side, bitboard other_side,
                     bool captures = false);
  int MakeKingMoves(Move *moves, int movecount, bitboard kings,
                    bitboard same_side, bitboard other_side,
                    bool captures = false);
  void PushMove(Move *moves, int movecount, square from, square to,
                move_flag flags, castle_flag castleFlags,
                piece captured = NO_PIECE, square epSquare = NO_SQUARE);
}; // class Board

template <color c> static constexpr bitboard Queens(const Board &board) {
  return board.GetQueens<c>();
}

template <color c> static constexpr bitboard Rooks(const Board &board) {
  return board.GetRooks<c>();
}

template <color c> static constexpr bitboard Knights(const Board &board) {
  return board.GetKnights<c>();
}

template <color c> static constexpr bitboard Bishops(const Board &board) {
  return board.GetBishops<c>();
}

template <color c> static constexpr bitboard Pawns(const Board &board) {
  return board.GetPawns<c>();
}

template <color c> static constexpr bitboard Kings(const Board &board) {
  return board.GetKings<c>();
}

template <color c> static constexpr bitboard Pieces(const Board &board) {
  return board.GetPieces<c>();
}

} // namespace Panzer