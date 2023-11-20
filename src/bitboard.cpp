#include <bitboard.h>
#include <bitboard_constants.h>
#include <board_utils.h>
#include <com.h>
#include <piece_square_scores.h>
#include <search.h>
#include <sliders.h>
#include <sstream>

namespace Panzer {
std::string Board::PrintMoveChain() {
  std::stringstream chain;
  for (const auto &move : moveChain) {
    chain << Utils::PrintMove(move);
  }

  return chain.str();
}

bool Board::IsSquareAttacked(square s, color color) {
  bitboard squareBitboard = ONE_BIT << s;
  if (color == WHITE) {
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

  if (color == BLACK) {
    bitboard pawnMask =
        ((squareBitboard & ~A_FILE) >> SW) | ((squareBitboard & ~H_FILE) >> SE);
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

bool Board::IsChecked(color color) {
  if (color == WHITE) {
    bitboard king = GetKings<WHITE>();
    square kingSquare = Utils::GetLSB(king);
    return this->IsSquareAttacked(kingSquare, WHITE);
  }

  if (color == BLACK) {
    bitboard king = GetKings<BLACK>();
    square kingSquare = Utils::GetLSB(king);
    return this->IsSquareAttacked(kingSquare, BLACK);
  }
  return false;
}

void Board::ToggleBitBoards(square from, square to, piece p, color c) {
  bitboard fromToBB = (ONE_BIT << from) | (ONE_BIT << to);
  Colors.at(c) ^= fromToBB;
  Pieces.at(p) ^= fromToBB;
  pieceLookup.at(from) = NO_PIECE;
  pieceLookup.at(to) = p;
  boardHash ^= zorbist.Get_Hash_Value(from, p, c);
  boardHash ^= zorbist.Get_Hash_Value(to, p, c);
}

void Board::FillSquare(const square s, const piece p, const color c) {
  bitboard bb = ONE_BIT << s;
  Colors.at(c) ^= bb;
  Pieces.at(p) ^= bb;
  pieceLookup.at(s) = p;
  boardHash ^= zorbist.Get_Hash_Value(s, p, c);
}

void Board::ClearSquare(const square s, const piece p, const color c) {
  bitboard bb = ONE_BIT << s;
  Colors.at(c) ^= bb;
  Pieces.at(p) ^= bb;
  pieceLookup.at(s) = NO_PIECE;
  boardHash ^= zorbist.Get_Hash_Value(s, p, c);
}

void Board::PushMove(Move *moves, int movecount, square from, square to,
                     move_flag flags, castle_flag castleFlags, piece captured,
                     square epSquare) {
  int score = 0;

  if (captured != NO_PIECE) {
    score += SORT_CAPTURE + CAPTURE_SCORES[captured] +
             CAPTURE_SCORES_AGGRESSOR[this->GetPieceAtSquare(from)];
  }

  if (flags & KNIGHT_PROMO) {
    score += SORT_PROMO + CAPTURE_SCORES[KNIGHT];
  } else if (flags & ROOK_PROMO) {
    score += SORT_PROMO + CAPTURE_SCORES[ROOK];
  } else if (flags & BISHOP_PROMO) {
    score += SORT_PROMO + CAPTURE_SCORES[BISHOP];
  } else if (flags & QUEEN_PROMO) {
    score += SORT_PROMO + CAPTURE_SCORES[QUEEN];
  }

  if (flags & CASTLE) {
    score += 500;
  }

  auto move =
      Panzer::Move(from, to, flags, castleFlags, captured,
                   ep_square == NO_SQUARE ? this->ep_square : ep_square, score);
  move.setId(movecount);
  moves[movecount] = move;
}

int Board::GenerateMoves(Move *moves, bool captures) {
  switch (this->side_to_move) {
  case WHITE:
    return GenerateWhiteMoves(moves, captures);
  case BLACK:
    return GenerateBlackMoves(moves, captures);
  }
  return 0;
}

int Board::GenerateWhiteMoves(Move *moves, bool captures) {
  int movecount = 0;
  movecount = this->MakeWhitePawnMoves(moves, movecount, captures);
  movecount = this->MakeWhiteRooksMoves(moves, movecount, captures);
  movecount = this->MakeWhiteKnightMoves(moves, movecount, captures);
  movecount = this->MakeWhiteBishopMoves(moves, movecount, captures);
  movecount = this->MakeWhiteQueenMoves(moves, movecount, captures);
  movecount = this->MakeWhiteKingMoves(moves, movecount, captures);
  return movecount;
}

int Board::GenerateBlackMoves(Move *moves, bool captures) {
  int movecount = 0;
  movecount = this->MakeBlackPawnMoves(moves, movecount, captures);
  movecount = this->MakeBlackRooksMoves(moves, movecount, captures);
  movecount = this->MakeBlackKnightMoves(moves, movecount, captures);
  movecount = this->MakeBlackBishopMoves(moves, movecount, captures);
  movecount = this->MakeBlackQueenMoves(moves, movecount, captures);
  movecount = this->MakeBlackKingMoves(moves, movecount, captures);
  return movecount;
}

void Board::PrintBoard() {
  std::cout << std::endl;
  bitboard white = GetPieces<WHITE>();
  bitboard black = GetPieces<BLACK>();

  std::cout << "-----------------" << std::endl;
  for (int row = 7; row >= 0; --row) {
    std::cout << "|";
    for (int col = 0; col <= 7; ++col) {
      square s = ((row * 8) + col);
      piece p = this->GetPieceAtSquare(s);
      bitboard b = (ONE_BIT << s);

      if (white & b) {
        switch (p) {
        case PAWN:
          std::cout << "P";
          break;
        case ROOK:
          std::cout << "R";
          break;
        case BISHOP:
          std::cout << "B";
          break;
        case KNIGHT:
          std::cout << "N";
          break;
        case QUEEN:
          std::cout << "Q";
          break;
        case KING:
          std::cout << "K";
          break;
        }

      } else if (black & b) {
        switch (p) {
        case PAWN:
          std::cout << "p";
          break;
        case ROOK:
          std::cout << "r";
          break;
        case BISHOP:
          std::cout << "b";
          break;
        case KNIGHT:
          std::cout << "n";
          break;
        case QUEEN:
          std::cout << "q";
          break;
        case KING:
          std::cout << "k";
          break;
        }
      } else {
        std::cout << " ";
      }
      std::cout << "|";
    }

    std::cout << std::endl;
    std::cout << "-----------------";
    std::cout << std::endl;
  }
}

void Board::MakeMove(const Move &move) {
  this->priorHalfMoveClock = halfMoveClock;
  if (this->GetPieceAtSquare(move.getFrom()) == PAWN)
    halfMoveClock = 0;
  // remove the from piece
  if (move.isCapture()) {
    // clear the captured piece
    if (move.isEPCapture()) {
      this->ClearSquare(this->ep_square, PAWN, !this->side_to_move);
    } else {
      this->ClearSquare(move.getTo(), this->GetPieceAtSquare(move.getTo()),
                        !this->side_to_move);
    }

    halfMoveClock = 0;
  }

  // fill the to square
  if (move.isPromo()) {
    // get type from promotion type
    const auto flags = move.getFlags();
    if ((flags & QUEEN_PROMO) != 0) {
      this->FillSquare(move.getTo(), QUEEN, this->side_to_move);
    } else if ((flags & KNIGHT_PROMO) != 0) {
      this->FillSquare(move.getTo(), KNIGHT, this->side_to_move);
    } else if ((flags & ROOK_PROMO) != 0) {
      this->FillSquare(move.getTo(), ROOK, this->side_to_move);
    } else if ((flags & BISHOP_PROMO) != 0) {
      this->FillSquare(move.getTo(), BISHOP, this->side_to_move);
    }
    this->ClearSquare(move.getFrom(), PAWN, this->side_to_move);
  } else {
    this->ToggleBitBoards(move.getFrom(), move.getTo(),
                          this->GetPieceAtSquare(move.getFrom()),
                          this->side_to_move);
  }

  boardHash ^= zorbist.zorbist_castle_hash[this->castle_flags];
  if (this->castle_flags != EMPTY_CASTLE_FLAGS) {
    // toggle off castle flags
    if (move.getFrom() == E1 || move.getTo() == E1) {
      if ((this->castle_flags & (WHITEK | WHITEQ)) != 0) {
        this->castle_flags &= (ALL_CASTLE_FLAGS & ~(WHITEK | WHITEQ));
      }
    } else if (move.getFrom() == E8 || move.getTo() == E8) {
      if ((this->castle_flags & (BLACKK | BLACKQ)) != 0) {
        this->castle_flags &= (ALL_CASTLE_FLAGS & ~(BLACKK | BLACKQ));
      }
    } else if (move.isCastle() && this->side_to_move == WHITE) {
      this->castle_flags &= (ALL_CASTLE_FLAGS & ~(WHITEK | WHITEQ));
    } else if (move.isCastle() && this->side_to_move == BLACK) {
      this->castle_flags &= (ALL_CASTLE_FLAGS & ~(BLACKK | BLACKQ));
    } else if (move.getTo() == A1 || move.getFrom() == A1) {
      this->castle_flags &= (ALL_CASTLE_FLAGS & ~WHITEQ);
    } else if (move.getTo() == H1 || move.getFrom() == H1) {
      this->castle_flags &= (ALL_CASTLE_FLAGS & ~WHITEK);
    } else if (move.getTo() == A8 || move.getFrom() == A8) {
      this->castle_flags &= (ALL_CASTLE_FLAGS & ~BLACKQ);
    } else if (move.getTo() == H8 || move.getFrom() == H8) {
      this->castle_flags &= (ALL_CASTLE_FLAGS & ~BLACKK);
    }
  }

  boardHash ^= zorbist.zorbist_castle_hash[this->castle_flags];

  if (move.isCastle()) {
    switch (move.getTo()) {
    case C1:
      this->ToggleBitBoards(A1, D1, ROOK, WHITE);
      break;
    case G1:
      this->ToggleBitBoards(H1, F1, ROOK, WHITE);
      break;
    case C8:
      this->ToggleBitBoards(A8, D8, ROOK, BLACK);
      break;
    case G8:
      this->ToggleBitBoards(H8, F8, ROOK, BLACK);
      break;
    }
  }

  this->boardHash ^= zorbist.zorbist_ep_hash[this->ep_square];

  if (move.getFlags() == DOUBLE_PAWN_PUSH) {
    this->ep_square = move.getTo();
  } else {
    this->ep_square = NO_SQUARE;
  }

  this->boardHash ^= zorbist.zorbist_ep_hash[this->ep_square];
  this->side_to_move = !this->side_to_move;
  this->boardHash ^= zorbist.color_hash;
  this->ply++;
  moveChain.emplace_back(move);
}

void Board::UnmakeMove(const Move &move) {
  // intentionally ordered backwards from make move for
  // debugging purposes
  moveChain.pop_back();
  this->halfMoveClock = priorHalfMoveClock;
  this->ply--;
  this->side_to_move = !this->side_to_move;
  this->boardHash ^= zorbist.color_hash;

  boardHash ^= zorbist.zorbist_ep_hash[this->ep_square];
  this->ep_square = move.getPriorEPSquare();
  boardHash ^= zorbist.zorbist_ep_hash[this->ep_square];

  if (move.isCastle()) {
    switch (move.getTo()) {
    case C1:
      this->ToggleBitBoards(D1, A1, ROOK, WHITE);
      break;
    case G1:
      this->ToggleBitBoards(F1, H1, ROOK, WHITE);
      break;
    case C8:
      this->ToggleBitBoards(D8, A8, ROOK, BLACK);
      break;
    case G8:
      this->ToggleBitBoards(F8, H8, ROOK, BLACK);
      break;
    }
  }

  boardHash ^= zorbist.zorbist_castle_hash[this->castle_flags];
  this->castle_flags = move.getCastleFlags();
  boardHash ^= zorbist.zorbist_castle_hash[this->castle_flags];

  // clear the to square
  // fill the from square
  if (move.isPromo()) {
    // get type from promotion type
    const auto flags = move.getFlags();
    if (flags & QUEEN_PROMO) {
      this->ClearSquare(move.getTo(), QUEEN, this->side_to_move);
    } else if (flags & KNIGHT_PROMO) {
      this->ClearSquare(move.getTo(), KNIGHT, this->side_to_move);
    } else if (flags & ROOK_PROMO) {
      this->ClearSquare(move.getTo(), ROOK, this->side_to_move);
    } else if (flags & BISHOP_PROMO) {
      this->ClearSquare(move.getTo(), BISHOP, this->side_to_move);
    }
    this->FillSquare(move.getFrom(), PAWN, this->side_to_move);

  } else {
    this->ToggleBitBoards(move.getTo(), move.getFrom(),
                          this->GetPieceAtSquare(move.getTo()),
                          this->side_to_move);
  }

  // put captured piece back
  if (move.isCapture()) {
    if (move.isEPCapture()) {
      this->FillSquare(this->ep_square, PAWN, !this->side_to_move);
    } else {
      this->FillSquare(move.getTo(), move.getCapturedPiece(),
                       !this->side_to_move);
    }
  }
}

int Board::MakeWhitePawnMoves(Move *moves, int movecount, bool captures) {
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
  bitboard left_captures = ((pawns & ~A_FILE) << NW) & black_pieces; // shift NW
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
    PushMove(moves, movecount, from, to, EP_CAPTURE, this->castle_flags, PAWN);
    movecount++;
    ep_captures &= ep_captures - ONE_BIT;
  }

  while (right_captures != 0) {
    int index = Utils::GetLSB(right_captures);
    square to = index;
    square from = to - SW;
    PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
             this->GetPieceAtSquare(to));
    movecount++;
    right_captures &= right_captures - ONE_BIT;
  }

  while (left_captures != 0) {
    int index = Utils::GetLSB(left_captures);
    square to = index;
    square from = to - SE;
    PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
             this->GetPieceAtSquare(to));
    movecount++;
    left_captures &= left_captures - ONE_BIT;
  }

  while (promotion_right_captures != 0) {
    int index = Utils::GetLSB(promotion_right_captures);
    square to = index;
    square from = to - SW;
    PushMove(moves, movecount, from, to, QUEEN_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, ROOK_PROMO_CAPTURE, this->castle_flags,
             this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, BISHOP_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, KNIGHT_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;

    promotion_right_captures &= promotion_right_captures - ONE_BIT;
  }

  while (promotion_left_captures != 0) {
    int index = Utils::GetLSB(promotion_left_captures);
    square to = index;
    square from = to - SE;
    PushMove(moves, movecount, from, to, QUEEN_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, ROOK_PROMO_CAPTURE, this->castle_flags,
             this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, BISHOP_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, KNIGHT_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;

    promotion_left_captures &= promotion_left_captures - ONE_BIT;
  }

  if (captures)
    return movecount;

  while (promotions != 0) {
    int index = Utils::GetLSB(promotions);
    square to = index;
    square from = to - S;
    PushMove(moves, movecount, from, to, QUEEN_PROMO, this->castle_flags);
    movecount++;

    PushMove(moves, movecount, from, to, ROOK_PROMO, this->castle_flags);
    movecount++;

    PushMove(moves, movecount, from, to, KNIGHT_PROMO, this->castle_flags);
    movecount++;

    PushMove(moves, movecount, from, to, BISHOP_PROMO, this->castle_flags);
    movecount++;

    promotions &= promotions - ONE_BIT;
  }

  while (pushes != 0) {
    int index = Utils::GetLSB(pushes);
    square to = index;
    square from = to - S;
    PushMove(moves, movecount, from, to, NO_MOVE_FLAGS, this->castle_flags);
    movecount++;

    pushes &= pushes - ONE_BIT;
  }

  while (double_push != 0) {
    int index = Utils::GetLSB(double_push);
    square to = index;
    square from = to - SS;
    PushMove(moves, movecount, from, to, DOUBLE_PAWN_PUSH, this->castle_flags);
    movecount++;

    double_push &= double_push - ONE_BIT;
  }

  return movecount;
}

int Board::MakeWhiteRooksMoves(Move *moves, int movecount, bool captures) {
  auto rooks = GetRooks<WHITE>();
  if (rooks == 0) {
    return movecount;
  }

  auto white_pieces = GetPieces<WHITE>();
  auto black_pieces = GetPieces<BLACK>();
  movecount = this->MakeRookMoves(moves, movecount, rooks, white_pieces,
                                  black_pieces, captures);
  return movecount;
}

int Board::MakeWhiteKnightMoves(Move *moves, int movecount, bool captures) {
  auto knights = GetKnights<WHITE>();

  if (knights == 0) {
    return movecount;
  }

  auto white_pieces = GetPieces<WHITE>();
  auto black_pieces = GetPieces<BLACK>();
  movecount = this->MakeKnightMoves(moves, movecount, knights, white_pieces,
                                    black_pieces, captures);
  return movecount;
}

int Board::MakeWhiteBishopMoves(Move *moves, int movecount, bool captures) {
  auto bishops = this->GetBishops<WHITE>();
  if (bishops == 0) {
    return movecount;
  }
  auto white_pieces = GetPieces<WHITE>();
  auto black_pieces = GetPieces<BLACK>();
  movecount = this->MakeBishopMoves(moves, movecount, bishops, white_pieces,
                                    black_pieces, captures);
  return movecount;
}

int Board::MakeWhiteQueenMoves(Move *moves, int movecount, bool captures) {
  auto queens = this->GetQueens<WHITE>();
  if (queens == 0) {
    return movecount;
  }

  auto white_pieces = GetPieces<WHITE>();
  auto black_pieces = GetPieces<BLACK>();
  movecount = this->MakeQueenMoves(moves, movecount, queens, white_pieces,
                                   black_pieces, captures);
  return movecount;
}

int Board::MakeWhiteKingMoves(Move *moves, int movecount, bool captures) {
  auto kings = this->GetKings<WHITE>();
  auto white_pieces = GetPieces<WHITE>();
  auto black_pieces = GetPieces<BLACK>();
  if (!captures) {
    // make castle moves
    if ((castle_flags & WHITEK) != 0) {
      bool isOpen = (WHITEK_CASTLE_MASK & this->GetOccupancy()) == 0;
      if (isOpen) {
        bool notChecked =
            !(IsSquareAttacked(F1, WHITE) || IsSquareAttacked(G1, WHITE) ||
              IsSquareAttacked(E1, WHITE));
        if (notChecked) {
          PushMove(moves, movecount, E1, G1, CASTLE, this->castle_flags);
          movecount++;
        }
      }
    }

    if ((castle_flags & WHITEQ) != 0) {
      bool isOpen = (WHITEQ_CASTLE_MASK & this->GetOccupancy()) == 0;
      if (isOpen) {
        bool notChecked =
            !(IsSquareAttacked(C1, WHITE) || IsSquareAttacked(D1, WHITE) ||
              IsSquareAttacked(E1, WHITE));
        if (notChecked) {
          PushMove(moves, movecount, E1, C1, CASTLE, this->castle_flags);
          movecount++;
        }
      }
    }
  }

  movecount = this->MakeKingMoves(moves, movecount, kings, white_pieces,
                                  black_pieces, captures);
  return movecount;
}

int Board::MakeBlackPawnMoves(Move *moves, int movecount, bool captures) {
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
  bitboard left_captures = ((pawns & ~A_FILE) >> SW) & white_pieces; // shift NW

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
    PushMove(moves, movecount, from, to, EP_CAPTURE, this->castle_flags, PAWN);
    movecount++;

    ep_captures &= ep_captures - ONE_BIT;
  }

  while (right_captures != 0) {
    int index = Utils::GetLSB(right_captures);
    square to = index;
    square from = to + NW;
    PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
             this->GetPieceAtSquare(to));
    movecount++;

    right_captures &= right_captures - ONE_BIT;
  }

  while (left_captures != 0) {
    int index = Utils::GetLSB(left_captures);
    square to = index;
    square from = to + NE;
    PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
             this->GetPieceAtSquare(to));
    movecount++;

    left_captures &= left_captures - ONE_BIT;
  }

  while (promotion_right_captures != 0) {
    int index = Utils::GetLSB(promotion_right_captures);
    square to = index;
    square from = to + NW;
    PushMove(moves, movecount, from, to, QUEEN_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;
    PushMove(moves, movecount, from, to, ROOK_PROMO_CAPTURE, this->castle_flags,
             this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, BISHOP_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, KNIGHT_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;
    promotion_right_captures &= promotion_right_captures - ONE_BIT;
  }

  while (promotion_left_captures != 0) {
    int index = Utils::GetLSB(promotion_left_captures);
    square to = index;
    square from = to + NE;
    PushMove(moves, movecount, from, to, QUEEN_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, ROOK_PROMO_CAPTURE, this->castle_flags,
             this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, BISHOP_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;

    PushMove(moves, movecount, from, to, KNIGHT_PROMO_CAPTURE,
             this->castle_flags, this->GetPieceAtSquare(to));
    movecount++;

    promotion_left_captures &= promotion_left_captures - ONE_BIT;
  }

  if (captures)
    return movecount;

  while (promotions != 0) {
    int index = Utils::GetLSB(promotions);
    square to = index;
    square from = to + N;
    PushMove(moves, movecount, from, to, QUEEN_PROMO, this->castle_flags);
    movecount++;

    PushMove(moves, movecount, from, to, ROOK_PROMO, this->castle_flags);
    movecount++;

    PushMove(moves, movecount, from, to, KNIGHT_PROMO, this->castle_flags);
    movecount++;

    PushMove(moves, movecount, from, to, BISHOP_PROMO, this->castle_flags);
    movecount++;

    promotions &= promotions - ONE_BIT;
  }

  while (pushes != 0) {
    int index = Utils::GetLSB(pushes);
    square to = index;
    square from = to + N;
    PushMove(moves, movecount, from, to, NO_MOVE_FLAGS, this->castle_flags);
    movecount++;

    pushes &= pushes - ONE_BIT;
  }

  while (double_push != 0) {
    int index = Utils::GetLSB(double_push);
    square to = index;
    square from = to + NN;
    PushMove(moves, movecount, from, to, DOUBLE_PAWN_PUSH, this->castle_flags);
    movecount++;

    double_push &= double_push - ONE_BIT;
  }

  return movecount;
}

int Board::MakeBlackRooksMoves(Move *moves, int movecount, bool captures) {
  auto rooks = this->GetRooks<BLACK>();
  if (rooks == 0ULL) {
    return movecount;
  }
  auto white_pieces = GetPieces<WHITE>();
  auto black_pieces = GetPieces<BLACK>();
  movecount = this->MakeRookMoves(moves, movecount, rooks, black_pieces,
                                  white_pieces, captures);
  return movecount;
}

int Board::MakeBlackKnightMoves(Move *moves, int movecount, bool captures) {
  auto knights = this->GetKnights<BLACK>();
  if (knights == 0ULL) {
    return movecount;
  }
  auto white_pieces = GetPieces<WHITE>();
  auto black_pieces = GetPieces<BLACK>();
  movecount = this->MakeKnightMoves(moves, movecount, knights, black_pieces,
                                    white_pieces, captures);
  return movecount;
}

int Board::MakeBlackBishopMoves(Move *moves, int movecount, bool captures) {
  auto bishops = this->GetBishops<BLACK>();
  if (bishops == 0ULL) {
    return movecount;
  }
  auto white_pieces = GetPieces<WHITE>();
  auto black_pieces = GetPieces<BLACK>();
  movecount = this->MakeBishopMoves(moves, movecount, bishops, black_pieces,
                                    white_pieces, captures);
  return movecount;
}

int Board::MakeBlackQueenMoves(Move *moves, int movecount, bool captures) {
  auto queens = this->GetQueens<BLACK>();
  if (queens == 0ULL) {
    return movecount;
  }
  auto white_pieces = GetPieces<WHITE>();
  auto black_pieces = GetPieces<BLACK>();
  movecount = this->MakeQueenMoves(moves, movecount, queens, black_pieces,
                                   white_pieces, captures);
  return movecount;
}

int Board::MakeBlackKingMoves(Move *moves, int movecount, bool captures) {
  auto kings = this->GetKings<BLACK>();
  auto white_pieces = GetPieces<WHITE>();
  auto black_pieces = GetPieces<BLACK>();
  if (!captures) {
    if ((castle_flags & BLACKK) != 0) {
      bool isOpen = (BLACKK_CASTLE_MASK & this->GetOccupancy()) == 0;
      if (isOpen) {
        bool notChecked =
            !(IsSquareAttacked(F8, BLACK) || IsSquareAttacked(G8, BLACK) ||
              IsSquareAttacked(E8, BLACK));
        if (notChecked) {
          PushMove(moves, movecount, E8, G8, CASTLE, this->castle_flags);
          movecount++;
        }
      }
    }

    if ((castle_flags & BLACKQ) != 0) {
      bool isOpen = (BLACKQ_CASTLE_MASK & this->GetOccupancy()) == 0;
      if (isOpen) {
        bool notChecked =
            !(IsSquareAttacked(C8, BLACK) || IsSquareAttacked(D8, BLACK) ||
              IsSquareAttacked(E8, BLACK));
        if (notChecked) {
          PushMove(moves, movecount, E8, C8, CASTLE, this->castle_flags);
          movecount++;
        }
      }
    }
  }

  movecount = this->MakeKingMoves(moves, movecount, kings, black_pieces,
                                  white_pieces, captures);
  return movecount;
}

int Board::MakeRookMoves(Move *moves, int movecount, bitboard rooks,
                         bitboard same_side, bitboard other_side,
                         bool captures) {
  auto occupancy = same_side | other_side;
  while (rooks != 0) {
    square from = Utils::GetLSB(rooks);
    auto possible = SliderLookup.GetRookAttacks(from, occupancy);
    auto capture_moves = possible & other_side;

    while (capture_moves != 0) {
      square to = Utils::GetLSB(capture_moves);
      PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
               this->GetPieceAtSquare(to));
      movecount++;

      capture_moves &= capture_moves - ONE_BIT;
    }

    if (!captures) {
      auto slides = possible & ~occupancy;
      while (slides != 0) {
        square to = Utils::GetLSB(slides);
        PushMove(moves, movecount, from, to, NO_MOVE_FLAGS, this->castle_flags);
        movecount++;

        slides &= slides - ONE_BIT;
      }
    }

    rooks &= rooks - ONE_BIT;
  }

  return movecount;
}

int Board::MakeKnightMoves(Move *moves, int movecount, bitboard knights,
                           bitboard same_side, bitboard other_side,
                           bool captures) {

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
               this->GetPieceAtSquare(to));
      movecount++;

      capture_moves &= capture_moves - ONE_BIT;
    }

    if (!captures) {
      while (regular != 0) {
        int to = Utils::GetLSB(regular);
        PushMove(moves, movecount, from, to, NO_MOVE_FLAGS, this->castle_flags);
        movecount++;

        regular &= regular - ONE_BIT;
      }
    }
    knights &= knights - ONE_BIT;
  }

  return movecount;
}

int Board::MakeBishopMoves(Move *moves, int movecount, bitboard bishops,
                           bitboard same_side, bitboard other_side,
                           bool captures) {
  auto occupancy = same_side | other_side;
  while (bishops != 0) {
    square from = Utils::GetLSB(bishops);
    auto possible = SliderLookup.GetBishopAttacks(from, occupancy);
    auto capture_moves = possible & other_side;

    while (capture_moves != 0) {
      square to = Utils::GetLSB(capture_moves);
      PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
               this->GetPieceAtSquare(to));
      movecount++;

      capture_moves &= capture_moves - ONE_BIT;
    }

    if (!captures) {
      auto slides = possible & ~occupancy;
      while (slides != 0) {
        square to = Utils::GetLSB(slides);
        PushMove(moves, movecount, from, to, NO_MOVE_FLAGS, this->castle_flags);
        movecount++;

        slides &= slides - ONE_BIT;
      }
    }

    bishops &= bishops - ONE_BIT;
  }

  return movecount;
}

int Board::MakeQueenMoves(Move *moves, int movecount, bitboard queens,
                          bitboard same_side, bitboard other_side,
                          bool captures) {
  auto occupancy = same_side | other_side;
  while (queens != 0) {
    square from = Utils::GetLSB(queens);
    auto possible = SliderLookup.GetQueenAttacks(from, occupancy) & ~same_side;
    auto capture_moves = possible & other_side;

    while (capture_moves != 0) {
      square to = Utils::GetLSB(capture_moves);
      PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
               this->GetPieceAtSquare(to));
      movecount++;

      capture_moves &= capture_moves - ONE_BIT;
    }

    if (!captures) {
      auto slides = possible & ~occupancy;
      while (slides != 0) {
        square to = Utils::GetLSB(slides);
        PushMove(moves, movecount, from, to, NO_MOVE_FLAGS, this->castle_flags);
        movecount++;

        slides &= slides - ONE_BIT;
      }
    }

    queens &= queens - ONE_BIT;
  }
  return movecount;
}

int Board::MakeKingMoves(Move *moves, int movecount, bitboard kings,
                         bitboard same_side, bitboard other_side,
                         bool captures) {
  while (kings != 0) {
    int from = Utils::GetLSB(kings);
    bitboard king_span = KING_SPANS[from];
    auto all_moves = king_span & ~same_side;
    auto capture_moves = all_moves & other_side;
    auto regular_moves = all_moves & ~capture_moves;

    while (capture_moves != 0) {
      int to = Utils::GetLSB(capture_moves);
      PushMove(moves, movecount, from, to, CAPTURE, this->castle_flags,
               this->GetPieceAtSquare(to));
      movecount++;

      capture_moves &= capture_moves - ONE_BIT;
    }

    if (!captures) {
      while (regular_moves != 0) {
        int to = Utils::GetLSB(regular_moves);
        PushMove(moves, movecount, from, to, NO_MOVE_FLAGS, this->castle_flags);
        movecount++;

        regular_moves &= regular_moves - ONE_BIT;
      }
    }

    kings &= kings - ONE_BIT;
  }

  return movecount;
}

std::string Board::BoardToFen() {
  std::string fen = "";
  int empty_squares = 0;
  for (int index = 0; index < 64; index++) {
    square s = fenIndexToSquare[index];
    bitboard targetSquare = ONE_BIT << s;
    piece piece = this->GetPieceAtSquare(s);
    // if there is no piece at this index then increase empty square count
    if (piece == NO_PIECE) {
      empty_squares++;
    } else {
      color c = (this->GetPieces<WHITE>() & targetSquare) != 0 ? WHITE : BLACK;
      if (empty_squares != 0) {
        fen += std::to_string(empty_squares);
        empty_squares = 0;
      }
      switch (piece) {
      case PAWN:
        fen += c == BLACK ? "p" : "P";
        break;
      case ROOK:
        fen += c == BLACK ? "r" : "R";
        break;
      case KNIGHT:
        fen += c == BLACK ? "n" : "N";
        break;
      case BISHOP:
        fen += c == BLACK ? "b" : "B";
        break;
      case QUEEN:
        fen += c == BLACK ? "q" : "Q";
        break;
      case KING:
        fen += c == BLACK ? "k" : "K";
        break;
      }
    }

    if ((s & 7) == 7) {
      if (empty_squares != 0) {
        fen += std::to_string(empty_squares);
        empty_squares = 0;
      }
      if (s != H1) {
        fen += "/";
      }
    }
  }

  fen += " ";
  fen += this->side_to_move == WHITE ? "w" : "b";

  fen += " ";
  if (this->castle_flags == EMPTY_CASTLE_FLAGS) {
    fen += "-";
  } else {
    if ((this->castle_flags & WHITEK) != 0) {
      fen += "K";
    }

    if ((this->castle_flags & WHITEQ) != 0) {
      fen += "Q";
    }

    if ((this->castle_flags & BLACKK) != 0) {
      fen += "k";
    }

    if ((this->castle_flags & BLACKQ) != 0) {
      fen += "q";
    }
  }

  fen += " ";

  if (this->ep_square != NO_SQUARE) {
    fen += squareToString[this->ep_square];
  } else {
    fen += "-";
  }

  // TODO: implement move clocks

  fen += " ";

  // half move clock
  if (halfMoveClock != 0) {
    fen += std::to_string(halfMoveClock);
  } else {
    fen += "-";
  }

  fen += " ";

  fen += std::to_string(this->ply / 2 - this->side_to_move);
  return fen;
}

void Board::FenToBoard(const std::string &fen) {
  bool board_done = false;
  bool sideToMoveDone = false;
  bool castleFlagsDone = false;
  bool epSquareDone = false;
  bool halfClockDone = false;
  bool fullMoveNumberDone = false;
  this->boardHash = 0;
  std::vector<char> buffer = {};
  pieceLookup.fill(NO_PIECE);
  Pieces.fill(0ULL);
  Colors.fill(0ULL);
  castle_flags = EMPTY_CASTLE_FLAGS;
  ep_square = NO_SQUARE;
  moveChain.clear();

  int index = 0;
  for (size_t cIndex = 0; cIndex < fen.length(); cIndex++) {
    auto c = fen[cIndex];
    auto s = fenIndexToSquare[index];
    if (board_done) {
      if (!sideToMoveDone) {
        switch (c) {
        case 'b':
          this->side_to_move = BLACK;
          break;
        case 'w':
          this->side_to_move = WHITE;
          break;
        case ' ':
          sideToMoveDone = true;
          continue;
          break;
        }
      }

      // first space is side
      // second spacce is castle flags
      // third space is ep
      // last two digits are moves and plys
      if (sideToMoveDone && !castleFlagsDone) {
        switch (c) {
        case 'k':
          this->castle_flags |= BLACKK;
          break;
        case 'q':
          this->castle_flags |= BLACKQ;
          break;
        case 'K':
          this->castle_flags |= WHITEK;
          break;
        case 'Q':
          this->castle_flags |= WHITEQ;
          break;
        case '-':
          break;
        case ' ':
          castleFlagsDone = true;
          continue;
          break;
        }
      }

      if (castleFlagsDone && !epSquareDone) {
        if (c == '-') {
          epSquareDone = true;
          cIndex++;
          continue;
        }

        if (c == ' ') {
          std::string square = "";
          for (auto b : buffer) {
            square.push_back(b);
          }
          buffer.clear();

          if (!square.empty()) {
            this->ep_square = stringToSquare(square);
          }

          epSquareDone = true;
          continue;
        }

        buffer.push_back(c);
      }

      if (epSquareDone && !halfClockDone) {
        // set half clock from numbers
        if (c == '-') {
          buffer.clear();
          halfClockDone = true;
          cIndex++;
          continue;
        }

        if (c == ' ' && !halfClockDone) {
          std::string halfClock = "";

          for (auto b : buffer) {
            halfClock.push_back(b);
          }

          buffer.clear();
          // set half clock
          this->halfMoveClock = std::stoi(halfClock);
          halfClockDone = true;
          continue;
        }

        buffer.push_back(c);
      }

      if (halfClockDone && !fullMoveNumberDone) {
        // set turn clock from numbers
        if (c == '-') {
          fullMoveNumberDone = true;
          continue;
        }

        buffer.push_back(c);

        if (cIndex == fen.length() - 1) {
          std::string full = "";
          for (auto b : buffer) {
            full.push_back(b);
          }

          int moves = std::stoi(full);
          this->ply = moves * 2 + (int)this->side_to_move;
        }
      }
    } else {
      switch (c) {
      case 'r':
        this->FillSquare(s, ROOK, BLACK);
        index++;
        break;
      case 'n':
        this->FillSquare(s, KNIGHT, BLACK);
        index++;
        break;
      case 'b':
        this->FillSquare(s, BISHOP, BLACK);
        index++;
        break;
      case 'q':
        this->FillSquare(s, QUEEN, BLACK);
        index++;
        break;
      case 'k':
        this->FillSquare(s, KING, BLACK);
        index++;
        break;
      case 'p':
        this->FillSquare(s, PAWN, BLACK);
        index++;
        break;
      case 'R':
        this->FillSquare(s, ROOK, WHITE);
        index++;
        break;
      case 'N':
        this->FillSquare(s, KNIGHT, WHITE);
        index++;
        break;
      case 'B':
        this->FillSquare(s, BISHOP, WHITE);
        index++;
        break;
      case 'Q':
        this->FillSquare(s, QUEEN, WHITE);
        index++;
        break;
      case 'K':
        this->FillSquare(s, KING, WHITE);
        index++;
        break;
      case 'P':
        this->FillSquare(s, PAWN, WHITE);
        index++;
        break;
      case '/':
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
  }

  if (!board_done) {
    Panzer::Com::SendMessageToUI("info Failed to parse fen");
  }

  // hash the board
  if (this->side_to_move == BLACK) {
    this->boardHash ^= zorbist.color_hash;
  }

  this->boardHash ^= zorbist.zorbist_ep_hash[this->ep_square];
  this->boardHash ^= zorbist.zorbist_castle_hash[this->castle_flags];
  Panzer::Search::AddHashToRepition(this->boardHash);
}
} // namespace Panzer
