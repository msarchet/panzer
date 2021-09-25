#include "eval.h"
#include "bitboard.h"
#include "piece_square_scores.h"

namespace Panzer
{

EvalData data;

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

	for (square s = A1; s < H8; s++)
	{
		auto piece = board.GetPieceAtSquare(s);
		if (piece != NO_PIECE)
		{
			auto color = ((ONE_BIT << s) & board.GetWhitePieces()) ? WHITE : BLACK;
			if (color == WHITE)
			{
				whiteScore += data.MidGamePieceSquareScore[piece][WHITE][s];
			}
			else
			{
				blackScore += data.MidGamePieceSquareScore[piece][BLACK][s];
			}
		}
	}

	whiteScore += KNIGHT_ADJUSTMENTS[whitePawnCount];
	whiteScore += ROOK_ADJUSTMENTS[whitePawnCount];

	blackScore += KNIGHT_ADJUSTMENTS[blackPawnCount];
	blackScore += ROOK_ADJUSTMENTS[blackPawnCount];

	auto score = whiteScore - blackScore;

	return board.GetSideToMove() == WHITE ? score : -1 * score;
};

};
