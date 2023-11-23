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
  // TODO: no std_lib here
  // each piece on its own value
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

  template <bool onlyCaptures> int GenerateMoves(Move *moves) {
    switch (this->side_to_move) {
    case WHITE:
      return GenerateMovesForColor<WHITE, BLACK, onlyCaptures>(moves);
    case BLACK:
      return GenerateMovesForColor<BLACK, WHITE, onlyCaptures>(moves);
    }
    return 0;
  }

  template <color us, color them, bool onlyCaptures>
  int GenerateMovesForColor(Move *moves) {
    int movecount = 0;
    if constexpr (us == WHITE) {
      movecount = this->MakeWhitePawnMoves<onlyCaptures>(moves, movecount);
    }

    if constexpr (us == BLACK) {
      movecount = this->MakeBlackPawnMoves<onlyCaptures>(moves, movecount);
    }

    movecount = this->MakeRookMoves<us, them, onlyCaptures>(moves, movecount);
    movecount = this->MakeKnightMoves<us, them, onlyCaptures>(moves, movecount);
    movecount = this->MakeBishopMoves<us, them, onlyCaptures>(moves, movecount);
    movecount = this->MakeQueenMoves<us, them, onlyCaptures>(moves, movecount);
    movecount = this->MakeKingMoves<us, them, onlyCaptures>(moves, movecount);
    return movecount;
  }

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

  void FenToBoard(const std::string &fen);
  std::string PrintMoveChain();
  std::string BoardToFen();

  template <color c> bool IsSquareAttacked(square s) {
    bitboard squareBitboard = ONE_BIT << s;
    if constexpr (c == WHITE) {
      bitboard pawnMask = ((squareBitboard & ~A_FILE) << NW) |
                          ((squareBitboard & ~H_FILE) << NE); // shift NW
      bitboard diagonals = GetBishops<BLACK>() | GetQueens<BLACK>() |
                           (GetPawns<BLACK>() & pawnMask);
      bitboard occupancy = this->GetOccupancy();
      bitboard attackedOnDiagonal =
          SliderLookup.GetBishopAttacks(s, occupancy) & diagonals;

      if (attackedOnDiagonal != 0) {
        return true;
      }

      bitboard straights = GetQueens<BLACK>() | GetRooks<BLACK>();
      bitboard attackedOnStraights =
          SliderLookup.GetRookAttacks(s, occupancy) & straights;

      if (attackedOnStraights != 0) {
        return true;
      }

      bitboard knights = GetKnights<BLACK>();

      // mask for file wraps
      auto attackedByKnight = KNIGHT_SPANS[s] & knights;

      if (attackedByKnight != 0) {
        return true;
      }

      bitboard kings = this->GetKings<BLACK>();

      auto attackedByKing = KING_SPANS[s] & kings;

      if (attackedByKing != 0) {
        return true;
      }

      return false;
    }

    if constexpr (c == BLACK) {
      bitboard pawnMask = ((squareBitboard & ~A_FILE) >> SW) |
                          ((squareBitboard & ~H_FILE) >> SE);
      bitboard diagonals = GetBishops<WHITE>() | GetQueens<WHITE>() |
                           (GetPawns<WHITE>() & pawnMask);
      bitboard occupancy = this->GetOccupancy();
      bitboard attackedOnDiagonal =
          SliderLookup.GetBishopAttacks(s, occupancy) & diagonals;

      if (attackedOnDiagonal != 0) {
        return true;
      }

      bitboard straights = GetQueens<WHITE>() | GetRooks<WHITE>();
      bitboard attackedOnStraights =
          SliderLookup.GetRookAttacks(s, occupancy) & straights;

      if (attackedOnStraights != 0) {
        return true;
      }

      bitboard knights = GetKnights<WHITE>();

      // mask for file wraps
      auto attackedByKnight = KNIGHT_SPANS[s] & knights;

      if (attackedByKnight != 0) {
        return true;
      }

      bitboard kings = GetKings<WHITE>();

      auto attackedByKing = KING_SPANS[s] & kings;

      if (attackedByKing != 0) {
        return true;
      }

      return false;
    }

    return false;
  }

  bool IsChecked(color c) {
    if (c == WHITE) {
      return IsChecked<WHITE>();
    }

    return IsChecked<BLACK>();
  }

  template <color us> bool IsChecked() {
    bitboard king = GetKings<us>();
    square kingSquare = Utils::GetLSB(king);
    return this->IsSquareAttacked<us>(kingSquare);
  }

private:
  template <bool onlyCaptures>
  int MakeWhitePawnMoves(Move *moves, int movecount) {
    // generate pawn moves
    // first shift pawns up one
    const bitboard pawns = GetPawns<WHITE>();

    if (pawns == 0) {
      return movecount;
    }

    const bitboard black_pieces = GetPieces<BLACK>();
    const bitboard occupied = this->GetOccupancy();
    bitboard pushes = (pawns << N) & ~occupied; // shift north
    // scan the bits and make the move
    // char from = bit_index - 8;
    // char to = bit_index;
    // flag move_type = PAWN_PUSH;
    bitboard double_push = ((pushes & THIRD_RANK) << N) &
                           ~occupied; // moves forward that cant be occupied
    // scan the bits and make the moves
    // char from = bit_index - 16;
    // char to = bit_index;
    // flag move_type = PAWN_DOUBLE_PUSH;
    bitboard left_captures =
        ((pawns & ~A_FILE) << NW) & black_pieces; // shift NW
    bitboard right_captures =
        ((pawns & ~H_FILE) << NE) & black_pieces; // shift NE
    bitboard ep_captures = (pawns & rank_EP_masks[this->ep_square]);

    bitboard promotions = 0ULL;
    bitboard promotion_left_captures = 0ULL;
    bitboard promotion_right_captures = 0ULL;

    if ((pawns & SEVENTH_RANK) != 0) {
      promotions = (pushes & EIGHTH_RANK);
      pushes &= ~promotions;
      promotion_left_captures = (left_captures & EIGHTH_RANK);
      left_captures &= ~promotion_left_captures;
      promotion_right_captures = (right_captures & EIGHTH_RANK);
      right_captures &= ~promotion_right_captures;
    }

    while (ep_captures != 0) {
      square from = Utils::GetLSB(ep_captures);
      square to = this->ep_square + N;
      PushMove(moves, movecount, from, to, EP_CAPTURE, this->castle_flags, PAWN,
               this->ep_square);
      movecount++;
      ep_captures &= ep_captures - ONE_BIT;
    }

    while (right_captures != 0) {
      int index = Utils::GetLSB(right_captures);
      square to = index;
      square from = to - SW;
      PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
               this->GetPieceAtSquare(to), this->ep_square);
      movecount++;
      right_captures &= right_captures - ONE_BIT;
    }

    while (left_captures != 0) {
      int index = Utils::GetLSB(left_captures);
      square to = index;
      square from = to - SE;
      PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
               this->GetPieceAtSquare(to), this->ep_square);
      movecount++;
      left_captures &= left_captures - ONE_BIT;
    }

    while (promotion_right_captures != 0) {
      int index = Utils::GetLSB(promotion_right_captures);
      square to = index;
      square from = to - SW;
      PushMove(moves, movecount, from, to, QUEEN_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, ROOK_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, BISHOP_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, KNIGHT_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      promotion_right_captures &= promotion_right_captures - ONE_BIT;
    }

    while (promotion_left_captures != 0) {
      int index = Utils::GetLSB(promotion_left_captures);
      square to = index;
      square from = to - SE;
      PushMove(moves, movecount, from, to, QUEEN_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, ROOK_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, BISHOP_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, KNIGHT_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      promotion_left_captures &= promotion_left_captures - ONE_BIT;
    }

    if constexpr (onlyCaptures)
      return movecount;

    while (promotions != 0) {
      int index = Utils::GetLSB(promotions);
      square to = index;
      square from = to - S;
      PushMove(moves, movecount, from, to, QUEEN_PROMO, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, ROOK_PROMO, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, KNIGHT_PROMO, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, BISHOP_PROMO, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      promotions &= promotions - ONE_BIT;
    }

    while (pushes != 0) {
      int index = Utils::GetLSB(pushes);
      square to = index;
      square from = to - S;
      PushMove(moves, movecount, from, to, NO_MOVE_FLAGS, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      pushes &= pushes - ONE_BIT;
    }

    while (double_push != 0) {
      int index = Utils::GetLSB(double_push);
      square to = index;
      square from = to - SS;
      PushMove(moves, movecount, from, to, DOUBLE_PAWN_PUSH, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      double_push &= double_push - ONE_BIT;
    }

    return movecount;
  }

  template <color us, color them, bool onlyCaptures>
  int MakeRookMoves(Move *moves, int movecount) {
    auto rooks = GetRooks<us>();
    if (rooks == 0) {
      return movecount;
    }

    auto our_pieces = GetPieces<us>();
    auto their_pieces = GetPieces<them>();
    movecount = this->MakeRookMoves<onlyCaptures>(moves, movecount, rooks,
                                                  our_pieces, their_pieces);
    return movecount;
  }

  template <color us, color them, bool onlyCaptures>
  int MakeKnightMoves(Move *moves, int movecount) {
    auto knights = GetKnights<us>();

    if (knights == 0) {
      return movecount;
    }

    auto ourPieces = GetPieces<us>();
    auto theirPieces = GetPieces<them>();
    movecount = this->MakeKnightMoves<onlyCaptures>(moves, movecount, knights,
                                                    ourPieces, theirPieces);
    return movecount;
  }

  template <color us, color them, bool onlyCaptures>
  int MakeBishopMoves(Move *moves, int movecount) {
    auto bishops = this->GetBishops<us>();
    if (bishops == 0) {
      return movecount;
    }
    auto ourPieces = GetPieces<us>();
    auto theirPieces = GetPieces<them>();
    movecount = this->MakeBishopMoves<onlyCaptures>(moves, movecount, bishops,
                                                    ourPieces, theirPieces);
    return movecount;
  }

  template <color us, color them, bool onlyCaptures>
  int MakeQueenMoves(Move *moves, int movecount) {
    auto queens = this->GetQueens<us>();
    if (queens == 0) {
      return movecount;
    }

    auto ourPieces = GetPieces<us>();
    auto theirPieces = GetPieces<them>();
    movecount = this->MakeQueenMoves<onlyCaptures>(moves, movecount, queens,
                                                   ourPieces, theirPieces);
    return movecount;
  }

  template <color us, color them, bool onlyCaptures>
  int MakeKingMoves(Move *moves, int movecount) {
    auto kings = this->GetKings<us>();
    if (kings == 0) {
      return movecount;
    }

    auto ourPieces = GetPieces<us>();
    auto theirPieces = GetPieces<them>();
    if constexpr (us == WHITE) {
      if constexpr (!onlyCaptures) {
        // make castle moves
        if ((castle_flags & WHITEK) != 0) {
          bool isOpen = (WHITEK_CASTLE_MASK & this->GetOccupancy()) == 0;
          if (isOpen) {
            bool notChecked =
                !(IsSquareAttacked<WHITE>(F1) || IsSquareAttacked<WHITE>(G1) ||
                  IsSquareAttacked<WHITE>(E1));
            if (notChecked) {
              PushMove(moves, movecount, E1, G1, CASTLE, this->castle_flags,
                       NO_PIECE, this->ep_square);
              movecount++;
            }
          }
        }

        if ((castle_flags & WHITEQ) != 0) {
          bool isOpen = (WHITEQ_CASTLE_MASK & this->GetOccupancy()) == 0;
          if (isOpen) {
            bool notChecked =
                !(IsSquareAttacked<WHITE>(C1) || IsSquareAttacked<WHITE>(D1) ||
                  IsSquareAttacked<WHITE>(E1));
            if (notChecked) {
              PushMove(moves, movecount, E1, C1, CASTLE, this->castle_flags,
                       NO_PIECE, this->ep_square);
              movecount++;
            }
          }
        }
      }
    }

    if constexpr (us == BLACK) {
      if constexpr (!onlyCaptures) {
        if ((castle_flags & BLACKK) != 0) {
          bool isOpen = (BLACKK_CASTLE_MASK & this->GetOccupancy()) == 0;
          if (isOpen) {
            bool notChecked =
                !(IsSquareAttacked<BLACK>(F8) || IsSquareAttacked<BLACK>(G8) ||
                  IsSquareAttacked<BLACK>(E8));
            if (notChecked) {
              PushMove(moves, movecount, E8, G8, CASTLE, this->castle_flags,
                       NO_PIECE, this->ep_square);
              movecount++;
            }
          }
        }

        if ((castle_flags & BLACKQ) != 0) {
          bool isOpen = (BLACKQ_CASTLE_MASK & this->GetOccupancy()) == 0;
          if (isOpen) {
            bool notChecked =
                !(IsSquareAttacked<BLACK>(C8) || IsSquareAttacked<BLACK>(D8) ||
                  IsSquareAttacked<BLACK>(E8));
            if (notChecked) {
              PushMove(moves, movecount, E8, C8, CASTLE, this->castle_flags,
                       NO_PIECE, this->ep_square);
              movecount++;
            }
          }
        }
      }
    }
    movecount = this->MakeKingMoves<onlyCaptures>(moves, movecount, kings,
                                                  ourPieces, theirPieces);
    return movecount;
  }

  template <bool onlyCaptures>
  int MakeBlackPawnMoves(Move *moves, int movecount) {
    // generate pawn moves
    // first shift pawns up one
    const bitboard pawns = this->GetPawns<BLACK>();
    if (pawns == 0ULL) {
      return movecount;
    }
    const bitboard white_pieces = GetPieces<WHITE>();
    const bitboard occupied = this->GetOccupancy();
    bitboard pushes = (pawns >> S) & ~occupied; // shift north
    // scan the bits and make the move
    // char from = bit_index - 8;
    // char to = bit_index;
    // flag move_type = PAWN_PUSH;
    bitboard double_push = ((pushes & SIXTH_RANK) >> S) &
                           ~occupied; // moves forward that cant be occupied
    // scan the bits and make the moves
    // char from = bit_index - 16;
    // char to = bit_index;
    // flag move_type = PAWN_DOUBLE_PUSH;
    bitboard left_captures =
        ((pawns & ~A_FILE) >> SW) & white_pieces; // shift NW

    bitboard right_captures =
        ((pawns & ~H_FILE) >> SE) & white_pieces; // shift NE

    bitboard ep_captures = (pawns & rank_EP_masks[this->ep_square]);

    bitboard promotions = 0ULL;
    bitboard promotion_left_captures = 0ULL;
    bitboard promotion_right_captures = 0ULL;

    if ((pawns & SECOND_RANK) != 0) {
      promotions = (pushes & FIRST_RANK);
      pushes &= ~promotions;
      promotion_left_captures = (left_captures & FIRST_RANK);
      left_captures &= ~promotion_left_captures;
      promotion_right_captures = (right_captures & FIRST_RANK);
      right_captures &= ~promotion_right_captures;
    }

    while (ep_captures != 0) {
      square from = Utils::GetLSB(ep_captures);
      square to = this->ep_square - S;
      PushMove(moves, movecount, from, to, EP_CAPTURE, this->castle_flags, PAWN,
               this->ep_square);
      movecount++;

      ep_captures &= ep_captures - ONE_BIT;
    }

    while (right_captures != 0) {
      int index = Utils::GetLSB(right_captures);
      square to = index;
      square from = to + NW;
      PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
               this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      right_captures &= right_captures - ONE_BIT;
    }

    while (left_captures != 0) {
      int index = Utils::GetLSB(left_captures);
      square to = index;
      square from = to + NE;
      PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
               this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      left_captures &= left_captures - ONE_BIT;
    }

    while (promotion_right_captures != 0) {
      int index = Utils::GetLSB(promotion_right_captures);
      square to = index;
      square from = to + NW;
      PushMove(moves, movecount, from, to, QUEEN_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;
      PushMove(moves, movecount, from, to, ROOK_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, BISHOP_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, KNIGHT_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;
      promotion_right_captures &= promotion_right_captures - ONE_BIT;
    }

    while (promotion_left_captures != 0) {
      int index = Utils::GetLSB(promotion_left_captures);
      square to = index;
      square from = to + NE;
      PushMove(moves, movecount, from, to, QUEEN_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, ROOK_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, BISHOP_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, KNIGHT_PROMO_CAPTURE,
               this->castle_flags, this->GetPieceAtSquare(to), this->ep_square);
      movecount++;

      promotion_left_captures &= promotion_left_captures - ONE_BIT;
    }

    if constexpr (onlyCaptures)
      return movecount;

    while (promotions != 0) {
      int index = Utils::GetLSB(promotions);
      square to = index;
      square from = to + N;
      PushMove(moves, movecount, from, to, QUEEN_PROMO, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, ROOK_PROMO, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, KNIGHT_PROMO, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      PushMove(moves, movecount, from, to, BISHOP_PROMO, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      promotions &= promotions - ONE_BIT;
    }

    while (pushes != 0) {
      int index = Utils::GetLSB(pushes);
      square to = index;
      square from = to + N;
      PushMove(moves, movecount, from, to, NO_MOVE_FLAGS, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      pushes &= pushes - ONE_BIT;
    }

    while (double_push != 0) {
      int index = Utils::GetLSB(double_push);
      square to = index;
      square from = to + NN;
      PushMove(moves, movecount, from, to, DOUBLE_PAWN_PUSH, this->castle_flags,
               NO_PIECE, this->ep_square);
      movecount++;

      double_push &= double_push - ONE_BIT;
    }

    return movecount;
  }

  template <bool onlyCaptures>
  int MakeRookMoves(Move *moves, int movecount, bitboard rooks,
                    bitboard same_side, bitboard other_side) {
    auto occupancy = same_side | other_side;
    while (rooks != 0) {
      square from = Utils::GetLSB(rooks);
      auto possible = SliderLookup.GetRookAttacks(from, occupancy);
      auto capture_moves = possible & other_side;

      while (capture_moves != 0) {
        square to = Utils::GetLSB(capture_moves);
        PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
                 this->GetPieceAtSquare(to), this->ep_square);
        movecount++;

        capture_moves &= capture_moves - ONE_BIT;
      }

      if constexpr (!onlyCaptures) {
        auto slides = possible & ~occupancy;
        while (slides != 0) {
          square to = Utils::GetLSB(slides);
          PushMove(moves, movecount, from, to, NO_MOVE_FLAGS,
                   this->castle_flags, NO_PIECE, this->ep_square);
          movecount++;

          slides &= slides - ONE_BIT;
        }
      }

      rooks &= rooks - ONE_BIT;
    }

    return movecount;
  }

  template <bool onlyCaptures>
  int MakeKnightMoves(Move *moves, int movecount, bitboard knights,
                      bitboard same_side, bitboard other_side) {

    while (knights != 0) {
      auto from = Utils::GetLSB(knights);
      auto span = KNIGHT_SPANS[from];

      // mask for file wraps
      auto all_moves = span & ~same_side;
      auto capture_moves = all_moves & other_side;
      auto regular = all_moves & ~capture_moves;

      while (capture_moves != 0) {
        int to = Utils::GetLSB(capture_moves);
        PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
                 this->GetPieceAtSquare(to), this->ep_square);
        movecount++;

        capture_moves &= capture_moves - ONE_BIT;
      }

      if constexpr (!onlyCaptures) {
        while (regular != 0) {
          int to = Utils::GetLSB(regular);
          PushMove(moves, movecount, from, to, NO_MOVE_FLAGS,
                   this->castle_flags, NO_PIECE, this->ep_square);
          movecount++;

          regular &= regular - ONE_BIT;
        }
      }
      knights &= knights - ONE_BIT;
    }

    return movecount;
  }

  template <bool onlyCaptures>
  int MakeBishopMoves(Move *moves, int movecount, bitboard bishops,
                      bitboard same_side, bitboard other_side) {
    auto occupancy = same_side | other_side;
    while (bishops != 0) {
      square from = Utils::GetLSB(bishops);
      auto possible = SliderLookup.GetBishopAttacks(from, occupancy);
      auto capture_moves = possible & other_side;

      while (capture_moves != 0) {
        square to = Utils::GetLSB(capture_moves);
        PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
                 this->GetPieceAtSquare(to), this->ep_square);
        movecount++;

        capture_moves &= capture_moves - ONE_BIT;
      }

      if constexpr (!onlyCaptures) {
        auto slides = possible & ~occupancy;
        while (slides != 0) {
          square to = Utils::GetLSB(slides);
          PushMove(moves, movecount, from, to, NO_MOVE_FLAGS,
                   this->castle_flags, NO_PIECE, this->ep_square);
          movecount++;

          slides &= slides - ONE_BIT;
        }
      }

      bishops &= bishops - ONE_BIT;
    }

    return movecount;
  }

  template <bool onlyCaptures>
  int MakeQueenMoves(Move *moves, int movecount, bitboard queens,
                     bitboard same_side, bitboard other_side) {
    auto occupancy = same_side | other_side;
    while (queens != 0) {
      square from = Utils::GetLSB(queens);
      auto possible =
          SliderLookup.GetQueenAttacks(from, occupancy) & ~same_side;
      auto capture_moves = possible & other_side;

      while (capture_moves != 0) {
        square to = Utils::GetLSB(capture_moves);
        PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
                 this->GetPieceAtSquare(to), this->ep_square);
        movecount++;

        capture_moves &= capture_moves - ONE_BIT;
      }

      if constexpr (!onlyCaptures) {
        auto slides = possible & ~occupancy;
        while (slides != 0) {
          square to = Utils::GetLSB(slides);
          PushMove(moves, movecount, from, to, NO_MOVE_FLAGS,
                   this->castle_flags, NO_PIECE, this->ep_square);
          movecount++;

          slides &= slides - ONE_BIT;
        }
      }

      queens &= queens - ONE_BIT;
    }
    return movecount;
  }

  template <bool onlyCaptures>
  int MakeKingMoves(Move *moves, int movecount, bitboard kings,
                    bitboard same_side, bitboard other_side) {
    while (kings != 0) {
      int from = Utils::GetLSB(kings);
      bitboard king_span = KING_SPANS[from];
      auto all_moves = king_span & ~same_side;
      auto capture_moves = all_moves & other_side;
      auto regular_moves = all_moves & ~capture_moves;

      while (capture_moves != 0) {
        int to = Utils::GetLSB(capture_moves);
        PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
                 this->GetPieceAtSquare(to), this->ep_square);
        movecount++;

        capture_moves &= capture_moves - ONE_BIT;
      }

      if constexpr (!onlyCaptures) {
        while (regular_moves != 0) {
          int to = Utils::GetLSB(regular_moves);
          PushMove(moves, movecount, from, to, NO_MOVE_FLAGS,
                   this->castle_flags, NO_PIECE, this->ep_square);
          movecount++;

          regular_moves &= regular_moves - ONE_BIT;
        }
      }

      kings &= kings - ONE_BIT;
    }

    return movecount;
  }

  void PushMove(Move *moves, int movecount, square from, square to,
                move_flag flags, castle_flag castleFlags, piece captured,
                square epSquare);
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