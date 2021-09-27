#include "eval.h"
#include "bitboard.h"
#include "piece_square_scores.h"
#include "board_utils.h"
#include "sliders.h"

namespace Panzer
{

EvalData data;
Panzer::Sliders *sliders;
void InitEvalData()
{

	for (int s = 0; s < 64; s++)
	{
		data.EndGamePieceSquareScore[PAWN][WHITE][index_white[s]] = PAWN_SCORE_SQUARE_END[s];
		data.EndGamePieceSquareScore[PAWN][BLACK][index_black[s]] = PAWN_SCORE_SQUARE_END[s];
		data.MidGamePieceSquareScore[PAWN][WHITE][index_white[s]] = PAWN_SCORE_SQUARE_MID[s];
		data.MidGamePieceSquareScore[PAWN][BLACK][index_black[s]] = PAWN_SCORE_SQUARE_MID[s];

		data.EndGamePieceSquareScore[ROOK][WHITE][index_white[s]] = ROOK_SCORE_SQUARE_END[s];
		data.EndGamePieceSquareScore[ROOK][BLACK][index_black[s]] = ROOK_SCORE_SQUARE_END[s];
		data.MidGamePieceSquareScore[ROOK][WHITE][index_white[s]] = ROOK_SCORE_SQUARE_MID[s];
		data.MidGamePieceSquareScore[ROOK][BLACK][index_black[s]] = ROOK_SCORE_SQUARE_MID[s];

		data.EndGamePieceSquareScore[BISHOP][WHITE][index_white[s]] = BISHOP_SCORE_SQUARE_END[s];
		data.EndGamePieceSquareScore[BISHOP][BLACK][index_black[s]] = BISHOP_SCORE_SQUARE_END[s];
		data.MidGamePieceSquareScore[BISHOP][WHITE][index_white[s]] = BISHOP_SCORE_SQUARE_MID[s];
		data.MidGamePieceSquareScore[BISHOP][BLACK][index_black[s]] = BISHOP_SCORE_SQUARE_MID[s];

		data.EndGamePieceSquareScore[KNIGHT][WHITE][index_white[s]] = KNIGHT_SCORE_SQUARE_END[s];
		data.EndGamePieceSquareScore[KNIGHT][BLACK][index_black[s]] = KNIGHT_SCORE_SQUARE_END[s];
		data.MidGamePieceSquareScore[KNIGHT][WHITE][index_white[s]] = KNIGHT_SCORE_SQUARE_MID[s];
		data.MidGamePieceSquareScore[KNIGHT][BLACK][index_black[s]] = KNIGHT_SCORE_SQUARE_MID[s];

		data.EndGamePieceSquareScore[QUEEN][WHITE][index_white[s]] = QUEEN_SCORE_SQUARE_END[s];
		data.EndGamePieceSquareScore[QUEEN][BLACK][index_black[s]] = QUEEN_SCORE_SQUARE_END[s];
		data.MidGamePieceSquareScore[QUEEN][WHITE][index_white[s]] = QUEEN_SCORE_SQUARE_MID[s];
		data.MidGamePieceSquareScore[QUEEN][BLACK][index_black[s]] = QUEEN_SCORE_SQUARE_MID[s];

		data.EndGamePieceSquareScore[KING][WHITE][index_white[s]] = KING_SCORE_SQUARE_END[s];
		data.EndGamePieceSquareScore[KING][BLACK][index_black[s]] = KING_SCORE_SQUARE_END[s];
		data.MidGamePieceSquareScore[KING][WHITE][index_white[s]] = KING_SCORE_SQUARE_MID[s];
		data.MidGamePieceSquareScore[KING][BLACK][index_black[s]] = KING_SCORE_SQUARE_MID[s];
	}

	sliders = new Panzer::Sliders();
	sliders->Initialize();
};

int EvaluateBoard(Board &board)
{
	// score board for piece material only right now
	auto whitePawnCount = __builtin_popcountll(board.GetWhitePawns());
	auto whiteRookCount = __builtin_popcountll(board.GetWhiteRooks());
	auto whiteBishopCount = __builtin_popcountll(board.GetWhiteBishops());
	auto whiteKnightCount = __builtin_popcountll(board.GetWhiteKnights());
	auto whiteQueenCount = __builtin_popcountll(board.GetWhiteQueens());

	auto blackPawnCount = __builtin_popcountll(board.GetBlackPawns());
	auto blackRookCount = __builtin_popcountll(board.GetBlackRooks());
	auto blackBishopCount = __builtin_popcountll(board.GetBlackBishops());
	auto blackKnightCount = __builtin_popcountll(board.GetBlackKnights());
	auto blackQueenCount = __builtin_popcountll(board.GetBlackQueens());

	auto whitePieces = board.GetWhitePieces();
	auto blackPieces = board.GetBlackPieces();

	auto phase = PAWN_PHASE * (whitePawnCount + blackPawnCount)
		+ QUEEN_PHASE * (whiteQueenCount + blackQueenCount)
		+ ROOK_PHASE * (whiteRookCount + blackRookCount)
		+ BISHOP_PHASE * (whiteBishopCount + blackBishopCount)
		+ KNIGHT_PHASE * (whiteKnightCount + blackKnightCount);

	auto whiteScore = whitePawnCount * PAWN_SCORE 
		+ whiteRookCount * ROOK_SCORE
		+ whiteBishopCount * BISHOP_SCORE
		+ whiteKnightCount * KNIGHT_SCORE
		+ whiteQueenCount * QUEEN_SCORE;

	whiteScore += whiteRookCount > 1 ? ROOK_PAIR : 0;
	whiteScore += whiteBishopCount > 1 ? BISHOP_PAIR : 0;
	whiteScore += whiteKnightCount > 1 ? KNIGHT_PAIR : 0;

	auto blackScore = blackPawnCount * PAWN_SCORE 
		+ blackRookCount * ROOK_SCORE
		+ blackBishopCount * BISHOP_SCORE
		+ blackKnightCount * KNIGHT_SCORE
		+ blackQueenCount * QUEEN_SCORE;

	blackScore += blackRookCount > 1 ? ROOK_PAIR : 0;
	blackScore += blackBishopCount > 1 ? BISHOP_PAIR : 0;
	blackScore += blackKnightCount > 1 ? KNIGHT_PAIR : 0;

	auto whiteScoreMid = 0;
	auto whiteScoreEnd = 0;
	auto blackScoreMid = 0;
	auto blackScoreEnd = 0;

	for (square s = A1; s < H8; s++)
	{
		auto piece = board.GetPieceAtSquare(s);
		if (piece != NO_PIECE)
		{
			auto color = ((ONE_BIT << s) & whitePieces) ? WHITE : BLACK;
			if (color == WHITE)
			{
				whiteScoreMid += data.MidGamePieceSquareScore[piece][WHITE][s];
				whiteScoreEnd += data.EndGamePieceSquareScore[piece][WHITE][s];
			}
			else
			{
				blackScoreMid += data.MidGamePieceSquareScore[piece][BLACK][s];
				blackScoreEnd += data.EndGamePieceSquareScore[piece][BLACK][s];
			}
		}
	}

	auto whitePhaseScore = (whiteScoreMid * (TOTAL_PHASE - phase) + (phase * whiteScoreEnd)) / TOTAL_PHASE;
	auto blackPhaseScore = (blackScoreMid + (TOTAL_PHASE - phase) + (phase * blackScoreEnd)) / TOTAL_PHASE;

	whiteScore += whitePhaseScore;
	blackScore += blackPhaseScore;
	whiteScore += EvaluateRooks(board, WHITE);
	whiteScore += EvaluateKnights(board, WHITE);
	whiteScore += EvaluateBishops(board, WHITE);
	whiteScore += EvaluateQueens(board, WHITE);
	whiteScore += EvaluatePawns(board, WHITE);
	whiteScore += EvaluateKing(board, WHITE);

	whiteScore += KNIGHT_ADJUSTMENTS[whitePawnCount];
	whiteScore += ROOK_ADJUSTMENTS[whitePawnCount];

	blackScore += EvaluateRooks(board, BLACK);
	blackScore += EvaluateKnights(board, BLACK);
	blackScore += EvaluateBishops(board, BLACK);
	blackScore += EvaluateQueens(board, BLACK);
	blackScore += EvaluatePawns(board, BLACK);
	blackScore += EvaluateKing(board, BLACK);

	blackScore += KNIGHT_ADJUSTMENTS[blackPawnCount];
	blackScore += ROOK_ADJUSTMENTS[blackPawnCount];

	auto score = whiteScore - blackScore;

	// equivalent to WHITE ? score : -1 * score
	return board.GetSideToMove() == WHITE ? score : -1 * score;
};
	int EvaluateRooks(Board &board, color c) 
	{
		int score = 0;
		bitboard rooks = c == WHITE ? board.GetWhiteRooks() : board.GetBlackRooks();
		bitboard pawns = c == WHITE ? board.GetWhitePawns() : board.GetBlackPawns();
		bitboard occupancy = c == WHITE ? board.GetWhitePieces() : board.GetBlackPieces();
		// are the rooks paired
		bitboard rooks_saved = rooks;
		while (rooks != 0)
		{
			square s = Utils::GetLSB(rooks);
			// how many squares do the rooks have to moved
			auto allowed = sliders->GetRookAttacks(s, board.GetOccupancy());

			score += __builtin_popcount(allowed) * 2;

			if ((allowed & pawns) == 0)
			{
				score += 50;
			}

			mask rank = SQUARE_TO_RANK[s];
			mask file = SQUARE_TO_FILE[s];

			// are the rooks paired
			if ((rooks_saved) & (rank|file)) score += 40;

			rooks &= rooks - ONE_BIT;
		}

		return score;
	};

	int EvaluateBishops(Board &board, color c)
	{
		int score = 0;
		bitboard bishops = c == WHITE ? board.GetWhiteBishops() : board.GetBlackBishops();
		bitboard pawns = c == WHITE ? board.GetWhitePawns() : board.GetBlackPawns();
		bitboard occupancy = c == WHITE ? board.GetWhitePieces() : board.GetBlackPieces();
		// are the rooks paired
		bitboard bishops_saved = bishops;

		while (bishops != 0)
		{
			// how many square do the bishops have to move
			square s = Utils::GetLSB(bishops);
			// are the bishops blocked by lots of pawns on the same colors
			auto allowed = sliders->GetBishopAttacks(s, board.GetOccupancy());
			mask diagonals = NW_DIAGONALS[s] | NE_DIAGONALS[s];
			score += __builtin_popcount(allowed) * 2;

			auto pawnCount = __builtin_popcount(diagonals & pawns);

			if (pawnCount < 3)
			{
				score += 10;
			}
			bishops &= bishops - ONE_BIT; 
		}

		return score;
	};

	int EvaluateQueens(Board &board, color c)
	{
		int score = 0;
		bitboard queens = c == WHITE ? board.GetWhiteQueens() : board.GetBlackQueens();
		// how many squares do the queens have to move
		bitboard occupancy = c == WHITE ? board.GetWhitePieces() : board.GetBlackPieces();
		bitboard pawns = c == WHITE ? board.GetWhitePawns() : board.GetBlackPawns();
		// are the rooks paired
		bitboard queens_saved = queens;

		while (queens != 0)
		{
			// how many square do the queens have to move
			square s = Utils::GetLSB(queens);
			// are the queens blocked by lots of pawns on the same colors
			auto allowed = sliders->GetBishopAttacks(s, board.GetOccupancy());
			mask diagonals = (NW_DIAGONALS[s] | NE_DIAGONALS[s] | SQUARE_TO_FILE[s] | SQUARE_TO_RANK[s]);
			score += __builtin_popcount(allowed) * 2;

			auto pawnCount = __builtin_popcount(diagonals & pawns);

			if (pawnCount < 3)
			{
				score += 10;
			}
			queens &= queens - ONE_BIT; 
		}

		return score;

		// are the queens blocked by pawns
	};

	int EvaluatePawns(Board &board, color c)
	{
		int score = 0;
		bitboard pawns = c == WHITE ? board.GetWhitePawns() : board.GetBlackPawns();
		bitboard saved_pawns = pawns;
		while (pawns != 0)
		{
			square s = Utils::GetLSB(pawns);

			// how many pawns are on a single file
			auto pawns_in_file = SQUARE_TO_FILE[s] & pawns;
			auto pawncount = __builtin_popcount(pawns_in_file);
			score += PAWN_STACKED_SCORE[pawncount];
			pawns &= ~pawns_in_file;
		}

		// second loop to check for pawns guarding pieces
		return score;
	};

	int EvaluateKnights(Board &board, color c)
	{
		return 0;
	}

	int EvaluateKing(Board &board, color c)
	{
		return 0;
	}
};
