#include <bitboard.h>
#include <board_utils.h>
#include <eval.h>
#include <piece_square_scores.h>
#include <sliders.h>

#include <unordered_map>

namespace Panzer {

EvalData data;
std::unordered_map<bitboard, int> pawnHash;

void InitEvalData() {

  for (int s = 0; s < 64; s++) {
    data.EndGamePieceSquareScore[PAWN][WHITE][index_white[s]] =
        PAWN_SCORE_SQUARE_END[s];
    data.EndGamePieceSquareScore[PAWN][BLACK][index_black[s]] =
        PAWN_SCORE_SQUARE_END[s];

    data.MidGamePieceSquareScore[PAWN][WHITE][index_white[s]] =
        PAWN_SCORE_SQUARE_MID[s];
    data.MidGamePieceSquareScore[PAWN][BLACK][index_black[s]] =
        PAWN_SCORE_SQUARE_MID[s];

    data.EndGamePieceSquareScore[ROOK][WHITE][index_white[s]] =
        ROOK_SCORE_SQUARE_END[s];
    data.EndGamePieceSquareScore[ROOK][BLACK][index_black[s]] =
        ROOK_SCORE_SQUARE_END[s];

    data.MidGamePieceSquareScore[ROOK][WHITE][index_white[s]] =
        ROOK_SCORE_SQUARE_MID[s];
    data.MidGamePieceSquareScore[ROOK][BLACK][index_black[s]] =
        ROOK_SCORE_SQUARE_MID[s];

    data.EndGamePieceSquareScore[BISHOP][WHITE][index_white[s]] =
        BISHOP_SCORE_SQUARE_END[s];
    data.EndGamePieceSquareScore[BISHOP][BLACK][index_black[s]] =
        BISHOP_SCORE_SQUARE_END[s];

    data.MidGamePieceSquareScore[BISHOP][WHITE][index_white[s]] =
        BISHOP_SCORE_SQUARE_MID[s];
    data.MidGamePieceSquareScore[BISHOP][BLACK][index_black[s]] =
        BISHOP_SCORE_SQUARE_MID[s];

    data.EndGamePieceSquareScore[KNIGHT][WHITE][index_white[s]] =
        KNIGHT_SCORE_SQUARE_END[s];
    data.EndGamePieceSquareScore[KNIGHT][BLACK][index_black[s]] =
        KNIGHT_SCORE_SQUARE_END[s];

    data.MidGamePieceSquareScore[KNIGHT][WHITE][index_white[s]] =
        KNIGHT_SCORE_SQUARE_MID[s];
    data.MidGamePieceSquareScore[KNIGHT][BLACK][index_black[s]] =
        KNIGHT_SCORE_SQUARE_MID[s];

    data.EndGamePieceSquareScore[QUEEN][WHITE][index_white[s]] =
        QUEEN_SCORE_SQUARE_END[s];
    data.EndGamePieceSquareScore[QUEEN][BLACK][index_black[s]] =
        QUEEN_SCORE_SQUARE_END[s];

    data.MidGamePieceSquareScore[QUEEN][WHITE][index_white[s]] =
        QUEEN_SCORE_SQUARE_MID[s];
    data.MidGamePieceSquareScore[QUEEN][BLACK][index_black[s]] =
        QUEEN_SCORE_SQUARE_MID[s];

    data.EndGamePieceSquareScore[KING][WHITE][index_white[s]] =
        KING_SCORE_SQUARE_END[s];
    data.EndGamePieceSquareScore[KING][BLACK][index_black[s]] =
        KING_SCORE_SQUARE_END[s];

    data.MidGamePieceSquareScore[KING][WHITE][index_white[s]] =
        KING_SCORE_SQUARE_MID[s];
    data.MidGamePieceSquareScore[KING][BLACK][index_black[s]] =
        KING_SCORE_SQUARE_MID[s];
  }
};

int EvaluateBoard(const Board &board) {
  // score board for piece material only right now
  auto whitePawnCount = __builtin_popcountll(board.GetPawns<WHITE>());
  auto whiteRookCount = __builtin_popcountll(board.GetRooks<WHITE>());
  auto whiteBishopCount = __builtin_popcountll(board.GetBishops<WHITE>());
  auto whiteKnightCount = __builtin_popcountll(board.GetKnights<WHITE>());
  auto whiteQueenCount = __builtin_popcountll(board.GetQueens<WHITE>());

  auto blackPawnCount = __builtin_popcountll(board.GetPawns<BLACK>());
  auto blackRookCount = __builtin_popcountll(board.GetRooks<BLACK>());
  auto blackBishopCount = __builtin_popcountll(board.GetBishops<BLACK>());
  auto blackKnightCount = __builtin_popcountll(board.GetKnights<BLACK>());
  auto blackQueenCount = __builtin_popcountll(board.GetQueens<BLACK>());

  auto whitePieces = board.GetPieces<WHITE>();
  // auto blackPieces = board.GetBlackPieces();
  auto phase = PAWN_PHASE * (whitePawnCount + blackPawnCount) +
               QUEEN_PHASE * (whiteQueenCount + blackQueenCount) +
               ROOK_PHASE * (whiteRookCount + blackRookCount) +
               BISHOP_PHASE * (whiteBishopCount + blackBishopCount) +
               KNIGHT_PHASE * (whiteKnightCount + blackKnightCount);

  phase = ((TOTAL_PHASE - phase) * TOTAL_PHASE + (HALF_PHASE)) / TOTAL_PHASE;

  auto whiteScore = whitePawnCount * PAWN_SCORE + whiteRookCount * ROOK_SCORE +
                    whiteBishopCount * BISHOP_SCORE +
                    whiteKnightCount * KNIGHT_SCORE +
                    whiteQueenCount * QUEEN_SCORE;

  whiteScore += whiteRookCount > 1 ? ROOK_PAIR : 0;
  whiteScore += whiteBishopCount > 1 ? BISHOP_PAIR : 0;
  whiteScore += whiteKnightCount > 1 ? KNIGHT_PAIR : 0;

  auto blackScore = blackPawnCount * PAWN_SCORE + blackRookCount * ROOK_SCORE +
                    blackBishopCount * BISHOP_SCORE +
                    blackKnightCount * KNIGHT_SCORE +
                    blackQueenCount * QUEEN_SCORE;

  blackScore += blackRookCount > 1 ? ROOK_PAIR : 0;
  blackScore += blackBishopCount > 1 ? BISHOP_PAIR : 0;
  blackScore += blackKnightCount > 1 ? KNIGHT_PAIR : 0;

  auto whiteScoreMid = 0;
  auto whiteScoreEnd = 0;
  auto blackScoreMid = 0;
  auto blackScoreEnd = 0;

  for (square s = A1; s < H8; s++) {
    const auto piece = board.GetPieceAtSquare(s);
    if (piece != NO_PIECE) {
      const auto color = ((ONE_BIT << s) & whitePieces) ? WHITE : BLACK;
      if (color == WHITE) {
        whiteScoreMid += data.MidGamePieceSquareScore[piece][WHITE][s];
        whiteScoreEnd += data.EndGamePieceSquareScore[piece][WHITE][s];
      } else {
        blackScoreMid += data.MidGamePieceSquareScore[piece][BLACK][s];
        blackScoreEnd += data.EndGamePieceSquareScore[piece][BLACK][s];
      }
    }
  }

  auto phaseDelta = TOTAL_PHASE - phase;
  auto whitePhaseScore =
      (whiteScoreMid * phaseDelta + (phase * whiteScoreEnd)) / TOTAL_PHASE;
  auto blackPhaseScore =
      (blackScoreMid * phaseDelta + (phase * blackScoreEnd)) / TOTAL_PHASE;

  whiteScore += whitePhaseScore;

  blackScore += blackPhaseScore;

  auto whiteScoreRooks = EvaluateRooks<WHITE>(board);
  auto whiteScoreKnights = EvaluateKnights<WHITE>(board);
  auto whiteScoreBishops = EvaluateBishops<WHITE>(board);
  auto whiteScoreQueens = EvaluateQueens<WHITE>(board);
  auto whiteScorePawns = EvaluatePawns<WHITE>(board);
  auto whiteScoreKing = EvaluateKing<WHITE>(board);
  auto whiteScoreControl = EvaluateControl<WHITE>(board);

  whiteScore += whiteScoreRooks + whiteScoreKnights + whiteScoreBishops +
                whiteScoreQueens + whiteScorePawns + whiteScoreKing +
                whiteScoreControl;
  whiteScore += KNIGHT_ADJUSTMENTS[whitePawnCount];
  whiteScore += ROOK_ADJUSTMENTS[whitePawnCount];

  blackScore += EvaluateRooks<BLACK>(board);
  blackScore += EvaluateKnights<BLACK>(board);
  blackScore += EvaluateBishops<BLACK>(board);
  blackScore += EvaluateQueens<BLACK>(board);
  blackScore += EvaluatePawns<BLACK>(board);
  blackScore += EvaluateKing<BLACK>(board);
  blackScore += EvaluateControl<BLACK>(board);

  blackScore += KNIGHT_ADJUSTMENTS[blackPawnCount];
  blackScore += ROOK_ADJUSTMENTS[blackPawnCount];

  auto score = whiteScore - blackScore;
  if (score == 0) {
    score = 1;
  }
  // std::cout << "done eval" << std::endl;
  //  equivalent to WHITE ? score : -1 * score
  return board.GetSideToMove() == WHITE ? score : -1 * score;
};
template <color c> int EvaluateRooks(const Board &board) {
  int score = 0;
  bitboard rooks = Rooks<c>(board);
  const bitboard pawns = Pawns<c>(board);
  // bitboard occupancy = c == WHITE ? board.GetWhitePieces() :
  // board.GetBlackPieces();
  //  are the rooks paired
  const bitboard rooks_saved = rooks;
  while (rooks != 0) {
    const square s = Utils::GetLSB(rooks);
    // how many squares do the rooks have to moved
    const auto allowed = SliderLookup.GetRookAttacks(s, board.GetOccupancy());

    score += __builtin_popcount(allowed);

    if ((allowed & pawns) == 0) {
      score += 5;
    }

    const mask rank = SQUARE_TO_RANK[s];
    const mask file = SQUARE_TO_FILE[s];

    // are the rooks paired
    if ((rooks_saved & (rank | file)) != 0 && (pawns & (rank | file)) == 0)
      score += 10;

    rooks &= rooks - ONE_BIT;
  }

  return score;
};

template <color c> int EvaluateBishops(const Board &board) {
  int score = 0;
  bitboard bishops = Bishops<c>(board);
  const bitboard pawns = Pawns<c>(board);
  // bitboard occupancy = c == WHITE ? board.GetWhitePieces() :
  // board.GetBlackPieces();
  //  are the rooks paired
  // bitboard bishops_saved = bishops;

  const bitboard occupancy = board.GetPieces<c>();
  while (bishops != 0) {
    // how many square do the bishops have to move
    const square s = Utils::GetLSB(bishops);
    // are the bishops blocked by lots of pawns on the same colors
    const auto allowed = SliderLookup.GetBishopAttacks(s, occupancy);
    score += __builtin_popcount(allowed) * 2;

    const mask diagonals = NW_DIAGONALS[s] | NE_DIAGONALS[s];
    const auto pawnCount = __builtin_popcount(diagonals & pawns);

    if (pawnCount < 3) {
      score += 5;
    }
    bishops &= bishops - ONE_BIT;
  }

  return score;
};

template <color c> int EvaluateQueens(const Board &board) {
  int score = 0;
  bitboard queens = Queens<c>(board);
  // how many squares do the queens have to move
  bitboard occupancy = Pieces<c>(board);
  bitboard pawns = Pawns<c>(board);
  // bitboard queens_saved = queens;

  while (queens != 0) {
    // how many square do the queens have to move
    square s = Utils::GetLSB(queens);
    // are the queens blocked by lots of pawns on the same colors
    if (c == WHITE) {
      if ((s & (FIRST_RANK | SECOND_RANK)) != 0) {
        if (board.GetPieceAtSquare(B1) == KNIGHT)
          score -= 2;
        if (board.GetPieceAtSquare(C1) == BISHOP)
          score -= 2;
        if (board.GetPieceAtSquare(F1) == BISHOP)
          score -= 2;
        if (board.GetPieceAtSquare(G1) == KNIGHT)
          score -= 2;
      }

    } else {
      if (!(s & (SEVENTH_RANK | SEVENTH_RANK))) {
        if (board.GetPieceAtSquare(B8) == KNIGHT)
          score -= 2;
        if (board.GetPieceAtSquare(C8) == BISHOP)
          score -= 2;
        if (board.GetPieceAtSquare(F8) == BISHOP)
          score -= 2;
        if (board.GetPieceAtSquare(G8) == KNIGHT)
          score -= 2;
      }
    }

    auto allowed = SliderLookup.GetQueenAttacks(s, board.GetOccupancy());
    if ((allowed & pawns) == 0) {
      score += 1;
    }

    mask diagonals = (NW_DIAGONALS[s] | NE_DIAGONALS[s] | SQUARE_TO_FILE[s] |
                      SQUARE_TO_RANK[s]);
    auto mob = __builtin_popcount(allowed & ~occupancy);
    score += mob - 14;
    auto pawnCount = __builtin_popcount(diagonals & pawns);

    if (pawnCount < 3) {
      score += 1;
    }
    queens &= queens - ONE_BIT;
  }

  return score;
};

template <color c> int EvaluatePawns(const Board &board) {
  int score = 0;
  bitboard pawns = Pawns<c>(board);

  if (pawnHash.contains(pawns)) {
    return pawnHash[pawns];
  }

  bitboard saved_pawns = pawns;
  while (pawns != 0) {
    square s = Utils::GetLSB(pawns);

    // how many pawns are on a single file
    auto pawns_in_file = SQUARE_TO_FILE[s] & pawns;
    auto pawncount = __builtin_popcount(pawns_in_file);
    score += PAWN_STACKED_SCORE[pawncount];
    pawns &= ~pawns_in_file;
  }

  // second loop to check for pawns guarding pieces
  pawnHash[pawns] = score;
  return score;
};

template <color c> int EvaluateKnights(const Board &board) { return 0; }

template <color c> int EvaluateKing(const Board &board) { return 0; }

int centerSquare[4] = {E4, D4, E5, D5};

template <color c> int EvaluateControl(const Board &board) {
  int score = 0;
  // does this color control the center of the board at all?
  bitboard occupancy = board.GetOccupancy();

  // any pawns?
  bitboard pawns = Pawns<c>(board);
  bitboard rooks = Rooks<c>(board);
  bitboard queens = Queens<c>(board);
  bitboard bishops = Bishops<c>(board);
  bitboard knights = Knights<c>(board);

  if (pawns & CENTER_PAWN_MASKS[c]) {
    score += 10;
  }
  for (auto square : centerSquare) {
    // any rooks or queens attacking center
    auto lineAttacks = SliderLookup.GetRookAttacks(square, occupancy);
    if (lineAttacks & (rooks)) {
      score += 2;
    }
    // any bishops or queens attacking center
    auto diagonalAttacks = SliderLookup.GetBishopAttacks(square, occupancy);
    if (diagonalAttacks & (bishops)) {
      score += 2;
    }

    if ((lineAttacks | diagonalAttacks) & queens) {
      score += 2;
    }

    // any knights attacking center
    if (KNIGHT_SPANS[square] & knights) {
      score += 10;
    }
  }

  return score;
}
}; // namespace Panzer
