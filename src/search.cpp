#include "search.h"
#include "board_utils.h"
#include "eval.h"
#include "com.h"

#include <climits>
#include <algorithm>
#include <chrono>

namespace Panzer
{
namespace Search
{
	int64_t nodes = 0;
	Panzer::Move Search(Panzer::Board &board, int depth)
	{
		nodes = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> start,end;

		start = std::chrono::high_resolution_clock::now();

		Panzer::Move bestMove = Panzer::Move(0, 0, 0, 0);
		bestMove.id = -1;	

		auto moves = board.GenerateMoves();
		Utils::SortMoves(moves);
		auto alpha = INT16_MIN;
		auto beta = INT16_MAX;

		int bestMoveIndex = 0;
		int moveIndex = 0;
		// find best move at depth 1 and then use that at the start of each search
		for (auto it = moves->begin(); it != moves->end(); it++)
		{
			auto move = *it; 
			board.MakeMove(move);

			int legalMoves = 0;
			if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				auto score = AlphaBetaMin(board, alpha, beta, 1);
				if (score > alpha)
				{
					alpha = score;
					bestMove = Panzer::Move(move);
					bestMoveIndex = moveIndex;
				}

				auto output = "Top Depth " + board.PrintMoveChain() + std::to_string(alpha) + " " + std::to_string(beta) + ": Move Score: " + std::to_string(score);
				Panzer::Com::OutputDebugFile(output);

				legalMoves++;
			}

			board.UnmakeMove(move);
			moveIndex++;
		}

		for (int iterative_depth = 2; iterative_depth <= depth; iterative_depth++)
		{
			std::iter_swap(moves->begin(), moves->begin() + bestMoveIndex);
			bestMoveIndex = 0;
			moveIndex = 0;

			for (auto it = moves->begin(); it != moves->end(); it++)
			{
				auto move = *it; 
				board.MakeMove(move);

				int legalMoves = 0;
				if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
				{
					auto score = AlphaBetaMin(board, alpha, beta, iterative_depth - 1);
					if (score > alpha)
					{
						alpha = score;
						bestMove = Panzer::Move(move);
						bestMoveIndex = moveIndex;
					}

					auto output = "Top Depth Iterative :" + std::to_string(iterative_depth) + board.PrintMoveChain() + std::to_string(alpha) + " " + std::to_string(beta) + ": Move Score: " + std::to_string(score);
					Panzer::Com::OutputDebugFile(output);

					legalMoves++;
				}

				board.UnmakeMove(move);
				moveIndex++;
			}

		}
		end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start; 
		auto time = elapsed_seconds.count();
		auto nps = nodes / time;
		Panzer::Com::SendMessageToUI("NPS: " + std::to_string(nps) + " Nodes: " + std::to_string(nodes) + " In: " + std::to_string(elapsed_seconds.count()));
		auto output = "FINAL ALPHA " + std::to_string(alpha);
		Panzer::Com::OutputDebugFile(output);
		return bestMove;
	}

	int16_t AlphaBetaMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth)
	{
		if (board.IsChecked(board.GetSideToMove())) depth++;
		if (depth == 0)  { 
			nodes++;
			auto eval = Panzer::EvaluateBoard(board); 
			auto output = "\t\tEVAL MAX" + std::to_string(eval) + " " + board.PrintMoveChain();
			Panzer::Com::OutputDebugFile(output);
			return eval;
		}

		auto moves = board.GenerateMoves();
		Utils::SortMoves(moves);
		int legalMoves = 0;
		for (auto it = moves->begin(); it != moves->end(); it++)
		{
			auto move = *it; 
			board.MakeMove(move);

			int16_t score = INT16_MIN;

			if (board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				board.UnmakeMove(move);
				continue;
			}

			score = AlphaBetaMin(board, alpha, beta, depth - 1);
			legalMoves++;

			if (score >= beta) { 
				auto output =  "\tMax Early Cut Off " + board.PrintMoveChain() + std::to_string(alpha) + " " + std::to_string(score);
				Panzer::Com::OutputDebugFile(output);
				board.UnmakeMove(move);
				return beta;
			}
			if (score > alpha) alpha = score;

			auto output = "\tMax Score "+ board.PrintMoveChain() + std::to_string(alpha) + " " +  std::to_string(score);
			Panzer::Com::OutputDebugFile(output);

			board.UnmakeMove(move);
		}

		if (legalMoves == 0)  
		{ 
			if (board.IsChecked(board.GetSideToMove()))
			{
				Panzer::Com::OutputDebugFile("Checkmate at " + board.PrintMoveChain());
				return -9999; 
			}

			return -9000;
		}
		return alpha;
	}

	int16_t AlphaBetaMin(Panzer::Board &board, int16_t alpha, int16_t beta, int depth)
	{
		if (board.IsChecked(board.GetSideToMove())) depth++;

		if (depth == 0)  { 
			nodes++;
			auto eval =  -1 * Panzer::EvaluateBoard(board); 
			auto output = "\t\tEVAL MIN " + std::to_string(eval) + " "+ board.PrintMoveChain();
			Panzer::Com::OutputDebugFile(output);
			return eval;
		}

		auto moves = board.GenerateMoves();
		Utils::SortMoves(moves);

		int legalMoves = 0;
		for (auto it = moves->begin(); it != moves->end(); it++)
		{
			auto move = *it; 
			board.MakeMove(move);
			int16_t score = INT16_MAX;

			if (board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				board.UnmakeMove(move);
				continue;
			}

			score = AlphaBetaMax(board, alpha, beta, depth - 1);
			legalMoves++;
			if (score <= alpha)  { 
				auto output = "\tMin Early Cut Off "+ board.PrintMoveChain() + std::to_string(score) + " " + std::to_string(beta);
				Panzer::Com::OutputDebugFile(output);
				board.UnmakeMove(move);
				return alpha; 
			}
			if (score < beta)
			{
 				beta = score;
			}


			auto output = "\tMin Scores " + board.PrintMoveChain() + std::to_string(alpha) + " " + std::to_string(beta);
			Panzer::Com::OutputDebugFile(output);
			board.UnmakeMove(move);
		}

		if (legalMoves == 0)  
		{ 
			if (board.IsChecked(board.GetSideToMove()))
			{
				Panzer::Com::OutputDebugFile("Checkmate at " + board.PrintMoveChain());
				return 9999; 
			}

			return 9000;
		}

		return beta;
	}

	int16_t AlphaBetaMinMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth)
	{
		auto moves = board.GenerateMoves();
		Utils::SortMoves(moves);

		if (depth == 0) 
		{
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