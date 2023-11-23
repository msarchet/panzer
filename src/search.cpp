#include <board_utils.h>
#include <com.h>
#include <eval.h>
#include <search.h>

#include <algorithm>
#include <chrono>
#include <climits>
#include <unordered_map>

namespace Panzer {
namespace Search {
int64_t nodes = 0;
int64_t qNodes = 0;
int64_t seeNodes = 0;

std::unordered_map<hash, int> repitionHash = std::unordered_map<hash, int>();
std::vector<Move *> moveCollectors;

void AddHashToRepition(hash key) { repitionHash[key] += 1; }

void ClearRepitionHash() {
  repitionHash.clear();
  repitionHash.reserve(10000);
}

bool IsDrawByRepition(hash key) { return repitionHash[key] == 3; }

void Search(Panzer::Board &board, int depth) {
  nodes = 0;
  qNodes = 0;
  seeNodes = 0;
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

  start = std::chrono::high_resolution_clock::now();
  SearchIterate(board, depth);
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  auto time = elapsed_seconds.count();
  auto totalNodes = (nodes + qNodes + seeNodes);
  auto qNodePercentage =
      static_cast<double>(qNodes) / static_cast<double>(totalNodes);
  auto seeNodePercentage =
      static_cast<double>(seeNodes) / static_cast<double>(totalNodes);
  auto nps = totalNodes / time;
  Panzer::Com::SendMessageToUI(
      "info  nps" + std::to_string(nps) + " nodes " +
      std::to_string(totalNodes) + "Root Nodes " + std::to_string(nodes) +
      "Quiesence Nodes " + std::to_string(qNodes) + " " +
      std::to_string(qNodePercentage) + "SEE nodes " +
      std::to_string(seeNodes) + " " + std::to_string(seeNodePercentage) +
      " time " + std::to_string(elapsed_seconds.count()));
}

void SearchIterate(Panzer::Board &board, int depth) {
  // score move for depth 1
  auto alpha = INT16_MIN;
  auto beta = INT16_MAX;

  Panzer::Move bestMove =
      Panzer::Move(NO_SQUARE, NO_SQUARE, 0, 0, NO_PIECE, NO_SQUARE, 0, 0);

  Move *moves = nullptr;

  if (moveCollectors.empty()) {
    moves = new Move[256];
  } else {
    moves = moveCollectors.back();
    moveCollectors.pop_back();
  }

  auto movecount = board.GenerateMoves<false>(moves);

  Utils::SortMoves(moves, movecount);

  int bestMoveIndex = 0;
  int moveIndex = 0;
  // find best move at depth 1 and then use that at the start of each search
  for (auto i = 0; i < movecount; i++) {
    auto move = moves[i];
    board.MakeMove(move);
    repitionHash[board.GetHash()] += 1;
    int legalMoves = 0;
    if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE)) {
      auto score = AlphaBetaMin(board, alpha, beta, 1);
      if (score > alpha) {
        alpha = score;
        bestMove = Panzer::Move(move);
        bestMoveIndex = moveIndex;
      }

      if (Panzer::Com::GetDebug()) {
        auto output = "Top Depth " + board.PrintMoveChain() +
                      std::to_string(alpha) + " " + std::to_string(beta) +
                      ": Move Score: " + std::to_string(score);
        Panzer::Com::OutputDebugFile(output);
      }

      legalMoves++;
    }

    repitionHash[board.GetHash()] -= 1;
    board.UnmakeMove(move);
    moveIndex++;
  }

  Panzer::Com::SendMessageToUI("info depth 1 score " + std::to_string(alpha) +
                               " " + Panzer::Utils::PrintMove(bestMove));

  for (int iterative_depth = 2; iterative_depth <= depth; iterative_depth++) {
    std::swap(moves[0], moves[bestMoveIndex]);

    alpha = INT16_MIN;
    beta = INT16_MAX;

    bestMoveIndex = 0;
    moveIndex = 0;

    for (auto i = 0; i < movecount; i++) {
      auto move = moves[i];
      board.MakeMove(move);
      repitionHash[board.GetHash()] += 1;

      int legalMoves = 0;
      if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE)) {
        auto score = AlphaBetaMin(board, alpha, beta, iterative_depth - 1);

        if (score > alpha) {
          alpha = score;
          bestMove = Panzer::Move(move);
          bestMoveIndex = moveIndex;
        }

        if (Panzer::Com::GetDebug()) {
          auto output =
              "Top Depth Iterative :" + std::to_string(iterative_depth) +
              board.PrintMoveChain() + std::to_string(alpha) + " " +
              std::to_string(beta) + ": Move Score: " + std::to_string(score);
          Panzer::Com::OutputDebugFile(output);
        }

        legalMoves++;
      }

      repitionHash[board.GetHash()] -= 1;
      board.UnmakeMove(move);
      moveIndex++;
    }

    Panzer::Com::SendMessageToUI(
        "info depth " + std::to_string(iterative_depth) + " score " +
        std::to_string(alpha) + " " + Panzer::Utils::PrintMove(bestMove));
  }

  if (movecount == 0) {
    // draw
    if (!board.IsChecked(board.GetSideToMove())) {
    }

    Panzer::Com::OutputDebugFile("no moves");
  }

  if (board.isDrawBy50MoveRule() || IsDrawByRepition(board.GetHash())) {
    bestMove = Panzer::Move(NO_SQUARE, A1, EMPTY_MOVE_FLAGS, EMPTY_CASTLE_FLAGS,
                            NO_PIECE, NO_SQUARE, 0, 0);
  }

  Panzer::Com::SendMessageToUI("bestmove " +
                               Panzer::Utils::PrintMove(bestMove));

  auto output = "FINAL ALPHA " + std::to_string(alpha);
  Panzer::Com::OutputDebugFile(output);

  if (moveCollectors.size() > 1000) {
    delete moves;
  } else {
    moveCollectors.push_back(moves);
  }
}

int16_t AlphaBetaMax(Panzer::Board &board, int16_t alpha, int16_t beta,
                     int depth) {
  // if out of time return
  if (IsDrawByRepition(board.GetHash()) || board.isDrawBy50MoveRule()) {
    return 0;
  }

  if (board.IsChecked(board.GetSideToMove()))
    depth++;

  if (depth == 0) {
    nodes++;
    auto eval = Quiesence(board, alpha, beta);
    if (Panzer::Com::GetDebug()) {
      auto output =
          "\t\tEVAL MAX" + std::to_string(eval) + " " + board.PrintMoveChain();
      Panzer::Com::OutputDebugFile(output);
    }
    return eval;
  }

  Move *moves = nullptr;

  if (moveCollectors.empty()) {
    moves = new Move[256];
  } else {
    moves = moveCollectors.back();
    moveCollectors.pop_back();
  }
  auto movecount = board.GenerateMoves<false>(moves);
  Utils::SortMoves(moves, movecount);
  int legalMoves = 0;
  for (auto i = 0; i < movecount; i++) {
    auto move = moves[i];
    color movedSide = board.GetSideToMove();
    board.MakeMove(move);
    hash boardHash = board.GetHash();
    repitionHash[boardHash] += 1;
    if (board.isDrawBy50MoveRule() || IsDrawByRepition(boardHash)) {
      repitionHash[boardHash] -= 1;
      board.UnmakeMove(move);
      if (moveCollectors.size() > 1000) {
        delete moves;
      } else {
        moveCollectors.push_back(moves);
      }
      return 0;
    }

    int16_t score = INT16_MIN;

    if (board.IsChecked(movedSide)) {
      repitionHash[boardHash] -= 1;
      board.UnmakeMove(move);
      continue;
    }

    score = AlphaBetaMin(board, alpha, beta, depth - 1);
    legalMoves++;

    if (score >= beta) {
      if (Panzer::Com::GetDebug()) {
        auto output = "\tMax Early Cut Off " + board.PrintMoveChain() +
                      std::to_string(alpha) + " " + std::to_string(score);
        Panzer::Com::OutputDebugFile(output);
      }
      repitionHash[boardHash] -= 1;
      board.UnmakeMove(move);
      if (moveCollectors.size() > 1000) {
        delete moves;
      } else {
        moveCollectors.push_back(moves);
      }
      return beta;
    }
    if (score > alpha)
      alpha = score;

    if (Panzer::Com::GetDebug()) {
      auto output = "\tMax Score " + board.PrintMoveChain() +
                    std::to_string(alpha) + " " + std::to_string(score);
      Panzer::Com::OutputDebugFile(output);
    }

    repitionHash[boardHash] -= 1;
    board.UnmakeMove(move);
  }
  if (moveCollectors.size() > 1000) {
    delete moves;
  } else {
    moveCollectors.push_back(moves);
  }

  if (legalMoves == 0) {
    if (board.IsChecked(board.GetSideToMove())) {
      if (Panzer::Com::GetDebug()) {
        Panzer::Com::OutputDebugFile("Checkmate at " + board.PrintMoveChain());
      }
      return -9999;
    }

    return 0;
  }

  return alpha;
}

int16_t AlphaBetaMin(Panzer::Board &board, int16_t alpha, int16_t beta,
                     int depth) {
  if (IsDrawByRepition(board.GetHash()) || board.isDrawBy50MoveRule()) {
    return 0;
  }

  // if out of time return
  if (board.IsChecked(board.GetSideToMove()))
    depth++;

  if (depth == 0) {
    nodes++;
    auto eval = -1 * Quiesence(board, alpha, beta);
    if (Panzer::Com::GetDebug()) {
      auto output =
          "\t\tEVAL MIN " + std::to_string(eval) + " " + board.PrintMoveChain();
      Panzer::Com::OutputDebugFile(output);
    }
    return eval;
  }

  Move *moves = nullptr;
  if (moveCollectors.empty()) {
    moves = new Move[256];
  } else {
    moves = moveCollectors.back();
    moveCollectors.pop_back();
  }

  auto movecount = board.GenerateMoves<false>(moves);
  Utils::SortMoves(moves, movecount);

  int legalMoves = 0;
  for (auto i = 0; i < movecount; i++) {
    auto move = moves[i];
    color movedSide = board.GetSideToMove();
    board.MakeMove(move);
    hash boardHash = board.GetHash();
    repitionHash[boardHash] += 1;
    if (board.isDrawBy50MoveRule() || IsDrawByRepition(boardHash)) {
      repitionHash[boardHash] -= 1;
      board.UnmakeMove(move);
      return 0;
    }

    int16_t score = INT16_MAX;

    if (board.IsChecked(movedSide)) {
      repitionHash[boardHash] -= 1;
      board.UnmakeMove(move);
      continue;
    }

    score = AlphaBetaMax(board, alpha, beta, depth - 1);
    legalMoves++;
    if (score <= alpha) {
      if (Panzer::Com::GetDebug()) {
        auto output = "\tMin Early Cut Off " + board.PrintMoveChain() +
                      std::to_string(score) + " " + std::to_string(beta);
        Panzer::Com::OutputDebugFile(output);
      }
      repitionHash[boardHash] -= 1;
      board.UnmakeMove(move);
      if (moveCollectors.size() > 1000) {
        delete moves;
      } else {
        moveCollectors.push_back(moves);
      }
      return alpha;
    }
    if (score < beta) {
      beta = score;
    }

    if (Panzer::Com::GetDebug()) {
      auto output = "\tMin Scores " + board.PrintMoveChain() +
                    std::to_string(alpha) + " " + std::to_string(beta);
      Panzer::Com::OutputDebugFile(output);
    }
    repitionHash[boardHash] -= 1;
    board.UnmakeMove(move);
  }
  if (moveCollectors.size() > 1000) {
    delete moves;
  } else {
    moveCollectors.push_back(moves);
  }

  if (legalMoves == 0) {
    if (board.IsChecked(board.GetSideToMove())) {
      if (Panzer::Com::GetDebug()) {
        Panzer::Com::OutputDebugFile("Checkmate at " + board.PrintMoveChain());
      }
      return 9999;
    }

    return 0;
  }

  return beta;
}

int16_t AlphaBetaMinMax(Panzer::Board &board, int16_t alpha, int16_t beta,
                        int depth) {
  // if out of time return
  Move *moves = nullptr;
  if (moveCollectors.empty()) {
    moves = new Move[256];
  } else {
    moves = moveCollectors.back();
    moveCollectors.pop_back();
  }
  auto movecount = board.GenerateMoves<false>(moves);
  Utils::SortMoves(moves, movecount);

  if (depth == 0) {
    return Panzer::EvaluateBoard(board);
  }

  int16_t bestScore = INT16_MIN;
  for (auto i = 0; i < movecount; i++) {
    auto move = moves[i];
    color movedSide = board.GetSideToMove();
    board.MakeMove(move);
    int16_t score = INT16_MIN;

    if (!board.IsChecked(movedSide)) {
      score = -1 * AlphaBetaMinMax(board, -1 * beta, -1 * alpha, depth - 1);
      if (score >= beta) {
        if (moveCollectors.size() > 1000) {
          delete moves;
        } else {
          moveCollectors.push_back(moves);
        }
        return score;
      }

      if (score > bestScore) {
        if (score > alpha) {
          alpha = score;
        }
      }
    }
  }

  if (moveCollectors.size() > 1000) {
    delete moves;
  } else {
    moveCollectors.push_back(moves);
  }

  return bestScore;
}

int16_t Quiesence(Panzer::Board &board, int16_t alpha, int16_t beta) {
  // if out of time return alpha
  // return alpha
  if (board.isDrawBy50MoveRule()) {
    return 0;
  }

  auto stand_pat = Panzer::EvaluateBoard(board);

  if (stand_pat >= beta) {
    return beta;
  }

  if (alpha < stand_pat) {
    alpha = stand_pat;
  }

  Move *moves = nullptr;
  if (moveCollectors.empty()) {
    moves = new Move[256]();
  } else {
    moves = moveCollectors.back();
    moveCollectors.pop_back();
  }

  auto movecount = board.GenerateMoves<true>(moves);
  Utils::SortMoves(moves, movecount);

  int legalMoves = 0;
  for (auto i = 0; i < movecount; i++) {
    auto move = moves[i];
    color movedSide = board.GetSideToMove();
    board.MakeMove(move);

    if (board.IsChecked(movedSide)) {
      board.UnmakeMove(move);
      continue;
    }

    legalMoves++;
    qNodes++;

    // if this capture
    if (SEE(board, move.getTo()) + stand_pat > alpha) {
      auto score = -1 * Quiesence(board, -1 * beta, -1 * alpha);
      board.UnmakeMove(move);

      if (score >= beta) {
        if (moveCollectors.size() > 1000) {
          delete moves;
        } else {
          moveCollectors.push_back(moves);
        }
        return beta;
      }

      if (score > alpha) {
        alpha = score;
      }
    } else {
      board.UnmakeMove(move);
    }
  }

  if (moveCollectors.size() > 1000) {
    delete moves;
  } else {
    moveCollectors.push_back(moves);
  }

  if (movecount != 0 && legalMoves == 0) {
    if (board.IsChecked(board.GetSideToMove())) {
      return -9999;
    }
  }

  return alpha;
}

int16_t SEE(Panzer::Board &board, square to) {
  int16_t value = 0;
  Move *seeMoves = nullptr;
  Move *seeFiltered = nullptr;
  if (moveCollectors.empty()) {
    seeMoves = new Move[256]();
  } else {
    seeMoves = moveCollectors.back();
    moveCollectors.pop_back();
  }

  if (moveCollectors.empty()) {
    seeFiltered = new Move[256]();
  } else {
    seeFiltered = moveCollectors.back();
    moveCollectors.pop_back();
  }

  auto movecount = board.GenerateMoves<true>(seeMoves);
  auto filteredCount = 0;
  for (int i = 0; i < movecount; i++) {
    auto move = seeMoves[i];
    if (move.getTo() == to) {
      seeFiltered[filteredCount] = move;
      filteredCount++;
    }
  }

  std::stable_sort(seeFiltered, seeFiltered + filteredCount,
                   [&board](const Panzer::Move &one, const Panzer::Move &two) {
                     return one.getScore() > two.getScore() &&
                            board.GetPieceAtSquare(one.getFrom()) <
                                board.GetPieceAtSquare(two.getFrom());
                   });

  for (int i = 0; i < filteredCount; i++) {
    auto move = seeFiltered[i];
    color movedSide = board.GetSideToMove();
    board.MakeMove(move);

    if (!board.IsChecked(movedSide)) {
      value =
          std::max(0, CAPTURE_SCORES[move.getCapturedPiece()] - SEE(board, to));
    }

    board.UnmakeMove(move);
    seeNodes++;
  }

  if (moveCollectors.size() > 1000) {
    delete seeMoves;
    delete seeFiltered;
  } else {
    moveCollectors.push_back(seeMoves);
    moveCollectors.push_back(seeFiltered);
  }
  return value;
}
} // namespace Search
} // namespace Panzer