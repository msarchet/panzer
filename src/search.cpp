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
	int64_t qNodes = 0;
	Panzer::Move Search(Panzer::Board &board, int depth)
	{
		nodes = 0;
		qNodes = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> start,end;

		start = std::chrono::high_resolution_clock::now();

		Panzer::Move bestMove = Panzer::Move(0, 0, 0, 0);
		bestMove.id = -1;	

		Move moves[256];
		auto movecount = board.GenerateMoves(moves);
		if (movecount == 0) return bestMove;
		Utils::SortMoves(moves, movecount);
		auto alpha = INT16_MIN;
		auto beta = INT16_MAX;

		int bestMoveIndex = 0;
		int moveIndex = 0;
		// find best move at depth 1 and then use that at the start of each search
		for (auto i = 0; i < movecount; i++)
		{
			auto move = moves[i]; 
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

				if (Panzer::Com::GetDebug())
				{
					auto output = "Top Depth " + board.PrintMoveChain() + std::to_string(alpha) + " " + std::to_string(beta) + ": Move Score: " + std::to_string(score);
					Panzer::Com::OutputDebugFile(output);
				}

				legalMoves++;
			}

			board.UnmakeMove(move);
			moveIndex++;
		}

		for (int iterative_depth = 2; iterative_depth <= depth; iterative_depth++)
		{
			auto temp = moves[0];
			auto best = moves[bestMoveIndex];
			moves[0] = best;
			moves[bestMoveIndex] = temp;

			//std::iter_swap(moves->begin(), moves->begin() + bestMoveIndex);
			bestMoveIndex = 0;
			moveIndex = 0;

			for (auto i = 0; i < movecount; i++)
			{
				auto move = moves[i]; 
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

					if (Panzer::Com::GetDebug())
					{
						auto output = "Top Depth Iterative :" + std::to_string(iterative_depth) + board.PrintMoveChain() + std::to_string(alpha) + " " + std::to_string(beta) + ": Move Score: " + std::to_string(score);
						Panzer::Com::OutputDebugFile(output);
					}

					legalMoves++;
				}

				board.UnmakeMove(move);
				moveIndex++;
			}

		}
		end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start; 
		auto time = elapsed_seconds.count();
		auto totalNodes = (nodes + qNodes);
		auto qNodePercentage = static_cast<double>(qNodes) / static_cast<double>(totalNodes);
		auto nps = totalNodes / time;
		Panzer::Com::SendMessageToUI("NPS: " + std::to_string(nps) + " Nodes: " + std::to_string(totalNodes) + "Root Nodes: " + std::to_string(nodes) + "Quiesence Nodes: " + std::to_string(qNodes) + " " + std::to_string(qNodePercentage)  + " In: " + std::to_string(elapsed_seconds.count()));
		auto output = "FINAL ALPHA " + std::to_string(alpha);
		Panzer::Com::OutputDebugFile(output);
		return bestMove;
	}

	int16_t AlphaBetaMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth)
	{
		if (board.IsChecked(board.GetSideToMove())) depth++;
		if (depth == 0)  { 
			nodes++;
			auto eval = Quiesence(board, alpha, beta); 
			if (Panzer::Com::GetDebug())
			{
				auto output = "\t\tEVAL MAX" + std::to_string(eval) + " " + board.PrintMoveChain();
				Panzer::Com::OutputDebugFile(output);
			}
			return eval;
		}

		Move moves[256];
		auto movecount = board.GenerateMoves(moves);
		Utils::SortMoves(moves, movecount);
		int legalMoves = 0;
		for (auto i = 0; i < movecount; i++)
		{
			auto move = moves[i]; 
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
				if (Panzer::Com::GetDebug())
				{
					auto output =  "\tMax Early Cut Off " + board.PrintMoveChain() + std::to_string(alpha) + " " + std::to_string(score);
					Panzer::Com::OutputDebugFile(output);
				}
				board.UnmakeMove(move);
				return beta;
			}
			if (score > alpha) alpha = score;

			if (Panzer::Com::GetDebug())
			{
				auto output = "\tMax Score "+ board.PrintMoveChain() + std::to_string(alpha) + " " +  std::to_string(score);
				Panzer::Com::OutputDebugFile(output);
			}

			board.UnmakeMove(move);
		}

		if (legalMoves == 0)  
		{ 
			if (board.IsChecked(board.GetSideToMove()))
			{
				if (Panzer::Com::GetDebug())
				{
					Panzer::Com::OutputDebugFile("Checkmate at " + board.PrintMoveChain());
				}
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
			auto eval =  -1 * Quiesence(board, alpha, beta); 
			if (Panzer::Com::GetDebug())
			{
				auto output = "\t\tEVAL MIN " + std::to_string(eval) + " "+ board.PrintMoveChain();
				Panzer::Com::OutputDebugFile(output);
			}
			return eval;
		}
		Move moves[256];
		auto movecount = board.GenerateMoves(moves);
		Utils::SortMoves(moves, movecount);

		int legalMoves = 0;
		for (auto i = 0; i < movecount; i++)
		{
			auto move = moves[i]; 
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
				if (Panzer::Com::GetDebug())
				{
					auto output = "\tMin Early Cut Off " + board.PrintMoveChain() + std::to_string(score) + " " + std::to_string(beta);
					Panzer::Com::OutputDebugFile(output);
				}
				board.UnmakeMove(move);
				return alpha; 
			}
			if (score < beta)
			{
 				beta = score;
			}

			if (Panzer::Com::GetDebug())
			{
				auto output = "\tMin Scores " + board.PrintMoveChain() + std::to_string(alpha) + " " + std::to_string(beta);
				Panzer::Com::OutputDebugFile(output);
			}
			board.UnmakeMove(move);
		}

		if (legalMoves == 0)  
		{ 
			if (board.IsChecked(board.GetSideToMove()))
			{
				if (Panzer::Com::GetDebug())
				{
					Panzer::Com::OutputDebugFile("Checkmate at " + board.PrintMoveChain());
				}
				return 9999; 
			}

			return 9000;
		}

		return beta;
	}

	int16_t AlphaBetaMinMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth)
	{
		Move moves[256];
		auto movecount = board.GenerateMoves(moves);
		Utils::SortMoves(moves, movecount);

		if (depth == 0) 
		{
			return Panzer::EvaluateBoard(board);
		}

		int16_t bestScore = INT16_MIN;
		for (auto i = 0; i < movecount; i++)
		{
			auto move = moves[i]; 
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

	int16_t Quiesence(Panzer::Board &board, int16_t alpha, int16_t beta)
	{
		auto stand_pat = Panzer::EvaluateBoard(board);

		if (stand_pat >= beta)
		{
			return beta;
		}

		if(alpha < stand_pat) 
		{
			alpha = stand_pat;
		}

		Move moves[256];
		auto movecount = board.GenerateMoves(moves, true);

		for (auto i = 0; i < movecount; i++)	
		{
			auto move = moves[i];
			board.MakeMove(move);
		
			if (board.IsChecked(board.GetSideToMove() == WHITE ? BLACK: WHITE))
			{
				board.UnmakeMove(move);
				continue;
			}
			qNodes++;
			auto score = -1 * Quiesence(board, -1 * beta, -1 * alpha );
			board.UnmakeMove(move);

			if(score >= beta) return beta;
			if(score > alpha) alpha = score;
		}
		return alpha;
	}
}
}