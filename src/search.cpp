#include "search.h"
#include "board_utils.h"
#include "eval.h"
#include <climits>
namespace Panzer
{
namespace Search
{
	Panzer::Move Search(Panzer::Board &board, int depth)
	{
		Panzer::Move bestMove = Panzer::Move(0, 0, 0, 0);
		auto moves = board.GenerateMoves();
		Utils::SortMoves(moves);
		auto alpha = INT16_MIN;
		auto beta = INT16_MAX;

		for (auto it = moves->begin(); it != moves->end(); it++)
		{
			auto move = *it; 
			board.MakeMove(move);

			int legalMoves = 0;
			if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				auto score = AlphaBetaMax(board, alpha, beta, depth - 1);
				if (score > alpha)
				{
					alpha = score;
					bestMove = Panzer::Move(move);
				}
				legalMoves++;
			}

			board.UnmakeMove(move);
		}

		return bestMove;
	}

	int16_t AlphaBetaMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth)
	{
		if (depth == 0) return -1 * Panzer::EvaluateBoard(board);

		auto moves = board.GenerateMoves();
		Utils::SortMoves(moves);
		int legalMoves = 0;
		for (auto it = moves->begin(); it != moves->end(); it++)
		{
			auto move = *it; 
			board.MakeMove(move);

			int16_t score = INT16_MIN;
			if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				score = AlphaBetaMin(board, alpha, beta, depth - 1);
				legalMoves++;
			}

			board.UnmakeMove(move);

			if (score >= beta) return beta;
			if (score > alpha) alpha = score;

		}

		if (legalMoves == 0)  
		{ 
			if (board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				return 9999; 
			}

			return 9000;
		}
		return alpha;
	}

	int16_t AlphaBetaMin(Panzer::Board &board, int16_t alpha, int16_t beta, int depth)
	{
		if (depth == 0) return Panzer::EvaluateBoard(board);
		auto moves = board.GenerateMoves();
		Utils::SortMoves(moves);

		int legalMoves = 0;
		for (auto it = moves->begin(); it != moves->end(); it++)
		{
			auto move = *it; 
			board.MakeMove(move);
			int16_t score = INT16_MAX;

			if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				score = AlphaBetaMax(board, alpha, beta, depth - 1);
				legalMoves++;
			}

			board.UnmakeMove(move);
			if (score <= alpha) return alpha;
			if (score < beta) beta = score;
		}

		if (legalMoves == 0)  
		{ 
			if (board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				return -9999; 
			}

			return -9000;
		}

		return beta;
	}

	int16_t AlphaBetaMinMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth)
	{
		auto moves = board.GenerateMoves();
		Utils::SortMoves(moves);

		if (depth == 0) {
			return Panzer::EvaluateBoard(board);
		}
		int16_t bestScore = INT16_MIN;
		for (auto it = moves->begin(); it != moves->end(); it++)
		{
			auto move = *it; 
			board.MakeMove(move);
			int16_t score = INT16_MIN;

			if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				score = -1 * AlphaBetaMinMax(board, -1 * beta, -1 * alpha, depth - 1);
				if (score >= beta)
				{
					return score;
				}

				if (score > bestScore)
				{
					bestScore = score;
					if (score > alpha)
					{
						alpha = score;
					}
				}
			}
		}

		return bestScore;
	}
}
}