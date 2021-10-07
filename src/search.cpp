#include "search.h"
#include "board_utils.h"
#include "eval.h"
#include "com.h"

#include <climits>
#include <algorithm>
#include <chrono>
#include <unordered_map>

//#define USE_TT
//#define SCORES
#define USE_SEE

namespace Panzer
{
namespace Search
{
	int64_t nodes = 0;
	int64_t qNodes = 0;
	int64_t seeNodes = 0;

	std::unordered_map<hash, int> repitionHash = std::unordered_map<hash, int>();

	void AddHashToRepition(hash key) 
	{
		repitionHash[key] += 1;
	}

	void ClearRepitionHash() 
	{
		repitionHash.clear();
		repitionHash.reserve(10000);
	}

	bool IsDrawByRepition(hash key) 
	{
		return repitionHash[key] == 3;
	}

	void Search(Panzer::Board &board, int depth)
	{
		nodes = 0;
		qNodes = 0;
		seeNodes = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> start,end;

		start = std::chrono::high_resolution_clock::now();
		SearchIterate(board, depth);
		end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start; 
		auto time = elapsed_seconds.count();
		auto totalNodes = (nodes + qNodes + seeNodes);
		auto qNodePercentage = static_cast<double>(qNodes) / static_cast<double>(totalNodes);
		auto seeNodePercentage = static_cast<double>(seeNodes) / static_cast<double>(totalNodes);
		auto nps = totalNodes / time;
		Panzer::Com::SendMessageToUI("info  nps" + std::to_string(nps) + " nodes " + std::to_string(totalNodes) + "Root Nodes " + std::to_string(nodes) + "Quiesence Nodes " + std::to_string(qNodes) + " " + std::to_string(qNodePercentage)  + "SEE nodes " + std::to_string(seeNodes) + " " + std::to_string(seeNodePercentage)+ " time " + std::to_string(elapsed_seconds.count()));
	}

	void SearchIterate(Panzer::Board &board, int depth) {
		// score move for depth 1
		auto alpha = INT16_MIN;
		auto beta = INT16_MAX;

		TTTable.Clear();
		Panzer::Move bestMove = Panzer::Move(NO_SQUARE, NO_SQUARE, 0, 0);
		bestMove.id = -1;	

		Move moves[256];
		auto movecount = board.GenerateMoves(moves);

		Utils::SortMoves(moves, movecount);

		int bestMoveIndex = 0;
		int moveIndex = 0;
		// find best move at depth 1 and then use that at the start of each search
		for (auto i = 0; i < movecount; i++)
		{
			auto move = moves[i]; 
			board.MakeMove(move);
			repitionHash[board.GetHash()] += 1;
			int legalMoves = 0;
			if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				auto score = -AlphaBetaMinMax(board, alpha, beta, 0);
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

			repitionHash[board.GetHash()] -= 1;
			board.UnmakeMove(move);
			moveIndex++;
		}

		Panzer::Com::SendMessageToUI("info depth 1 score " + std::to_string(alpha) + " " + Panzer::Utils::PrintMove(bestMove));

		for (int iterative_depth = 2; iterative_depth <= depth; iterative_depth++)
		{
			alpha = INT16_MIN;
			beta = INT16_MAX;
			// I'd rather move the best move to index 1
			// and then insert new best move at the front
			// and shift all othere elements over 1
			if (bestMoveIndex != 0)
			{
				auto temp = moves[0];
				moves[0] = moves[bestMoveIndex];
				for (int i = 1; i <= bestMoveIndex; i++)
				{
					auto swap = moves[i];
					moves[i] = temp;
					temp = swap;
				}
			}

			bestMoveIndex = 0;
			moveIndex = 0;

			for (auto i = 0; i < movecount; i++)
			{
				auto move = moves[i]; 
				board.MakeMove(move);
				repitionHash[board.GetHash()] += 1;

				int legalMoves = 0;
				if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
				{
					int16_t score = 0;
					#ifdef USE_TT
					auto ttEntry = TTTable.Find(board.GetHash());
					if (!(ttEntry.score != TT_INVALID && ttEntry.type == EXACT && ttEntry.depth >= iterative_depth))
					{
					#endif

					score = -AlphaBetaMinMax(board, -beta, -alpha, iterative_depth - 1);

					#ifdef USE_TT
					}
					else
					{
						score = ttEntry.score;
					}
					#endif

					if (score > alpha)
					{
						alpha = score;
						#ifdef USE_TT
						TTTable.Insert(board.GetHash(), iterative_depth, score, EXACT);
						#endif
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

				repitionHash[board.GetHash()] -= 1;
				board.UnmakeMove(move);
				moveIndex++;
			}

			Panzer::Com::SendMessageToUI("info depth " + std::to_string(iterative_depth) +" score " + std::to_string(alpha) + " " + Panzer::Utils::PrintMove(bestMove));
		}

		if (movecount == 0)  
		{
			// draw
			if (!board.IsChecked(board.GetSideToMove()))
			{
					bestMove.m_to = A1;
			}

			Panzer::Com::OutputDebugFile("no moves");
		}

		if (board.isDrawBy50MoveRule() || IsDrawByRepition(board.GetHash()))
		{
			bestMove = Panzer::Move(NO_SQUARE, A1, EMPTY_MOVE_FLAGS, EMPTY_CASTLE_FLAGS);
		}

		Panzer::Com::SendMessageToUI("bestmove " + Panzer::Utils::PrintMove(bestMove));

		auto output = "FINAL ALPHA " + std::to_string(alpha);
		Panzer::Com::OutputDebugFile(output);
	}

	int16_t AlphaBetaMinMax(Panzer::Board &board, int16_t alpha, int16_t beta, int depth)
	{
		if (board.IsChecked(board.GetSideToMove())) depth++;
		if (IsDrawByRepition(board.GetHash()) || board.isDrawBy50MoveRule()) { 
			return 0; 
		}

		if (depth == 0)  { 
			nodes++;
			int16_t eval = Quiesence(board, alpha, beta); 

#ifdef SCORES
			if (Panzer::Com::GetDebug())
			{
				auto output = "\t\tQ Search " + std::to_string(eval) + " "+ board.PrintMoveChain();
				Panzer::Com::OutputDebugFile(output);
			}
#endif
			return eval;
		}

		Move moves[256];
		auto movecount = board.GenerateMoves(moves);
		Utils::SortMoves(moves, movecount);
		uint8_t legalMoves = 0;
		for (auto i = 0; i < movecount; i++)
		{
			auto move = moves[i]; 
			board.MakeMove(move);
			repitionHash[board.GetHash()] += 1;

			int16_t score = INT16_MIN;

			if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
					#ifdef USE_TT
					auto ttEntry = TTTable.Find(board.GetHash());
					if (ttEntry.score == TT_INVALID || ttEntry.depth <= depth)
					{
					#endif

					score = -1 * AlphaBetaMinMax(board, -beta, -alpha, depth - 1);

					#ifdef USE_TT
					}
					else
					{
						score = ttEntry.score;
					}
					#endif


				// this move improved alpha
				if (score > alpha)
				{
					// this move is a cutoff for the min player
					if (score >= beta)
					{
						repitionHash[board.GetHash()] -= 1;
						board.UnmakeMove(move);
						return score;
					}

					alpha = score;
				}

				legalMoves++;
			}

			repitionHash[board.GetHash()] -= 1;
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

				return 9999 - board.GetPly(); 
			}

			return 0;
		}


		return alpha;
	}

	int16_t Quiesence(Panzer::Board &board, int16_t alpha, int16_t beta)
	{
		// if out of time return alpha
		// return alpha
		if (board.isDrawBy50MoveRule()) { 
			return 0; 
		}

		auto bestScore = Panzer::EvaluateBoard(board);

		if (bestScore >= beta)
		{
			return beta;
		}

		if (bestScore > alpha) alpha = bestScore;

		Move moves[256];
		auto movecount = board.GenerateMoves(moves, true);

		Utils::SortMoves(moves, movecount);

		int legalMoves = 0;
		for (auto i = 0; i < movecount; i++)	
		{
			auto move = moves[i];
			board.MakeMove(move);
		
			if (board.IsChecked(board.GetSideToMove() == WHITE ? BLACK: WHITE))
			{
				board.UnmakeMove(move);

				continue;
			}

			legalMoves++;
			qNodes++;

			#ifdef USE_SEE
			// don't search nodes that have a negative static exchange
			if (SEE(board, move.getTo()) < 0)
			{
			#endif
				auto score = -1 * Quiesence(board, -1 * beta, -1 * alpha );
				board.UnmakeMove(move);

				if(score > bestScore) bestScore = score;
				if (score >= beta) {
					return beta;
				}
			#ifdef USE_SEE
			}
			else
			{
				board.UnmakeMove(move);
			}
			#endif
		}

		if (movecount != 0 && legalMoves == 0)
		{
			if (board.IsChecked(board.GetSideToMove()))
			{
				return -9999;
			}
		}

		return bestScore;
	}

	int16_t SEE(Panzer::Board &board, square to)
	{
		int16_t value = 0;
		Move moves[256];
		auto movecount = board.GenerateMoves(moves, true);
		int lowestCapturerScore = INT_MIN;
		Move lowestCapturer = Panzer::Move(NO_SQUARE, NO_SQUARE, EMPTY_MOVE_FLAGS, EMPTY_CASTLE_FLAGS);
		for (int i = 0; i < movecount; i++)
		{
			auto move = moves[i];
			if (move.getTo() == to)
			{
				if (move.m_score > lowestCapturerScore)
				{
					lowestCapturer = move;
					lowestCapturerScore = move.m_score;
				}
			}
		}

		if (lowestCapturerScore != INT_MIN)
		{
			board.MakeMove(lowestCapturer);
			if (!board.IsChecked(board.GetSideToMove() == WHITE ? BLACK : WHITE))
			{
				auto see = CAPTURE_SCORES[lowestCapturer.capturedPiece] - SEE(board, to);
				value = see;
			}
			board.UnmakeMove(lowestCapturer);
			seeNodes++;
		}

		return value;
	}
}
}