#include "eval.h"
#include "bitboard.h"
#include "piece_square_scores.h"
#include "board_utils.h"
#include "sliders.h"

#include <unordered_map>

namespace Panzer
{

EvalData data;
Panzer::Sliders *sliders;
std::unordered_map<bitboard, EvalScores> pawnHash;

void InitEvalData()
{

	for (int s = 0; s < 64; s++)
	{
		auto rank = index_white[s] /8;
		auto file = index_white[s] % 8;
		auto file_adjusted = std::min(file, 7 - file);

		data.EndGamePieceSquareScore[PAWN][WHITE][index_white[s]] = pawn_bonus_eg[rank][file];
		data.EndGamePieceSquareScore[PAWN][BLACK][index_black[s]] = pawn_bonus_eg[rank][file];

		data.MidGamePieceSquareScore[PAWN][WHITE][index_white[s]] = pawn_bonus_mg[rank][file];
		data.MidGamePieceSquareScore[PAWN][BLACK][index_black[s]] = pawn_bonus_mg[rank][file];

		data.EndGamePieceSquareScore[ROOK][WHITE][index_white[s]] = non_pawn_bonus_eg[ROOK - 2][rank][file_adjusted];
		data.EndGamePieceSquareScore[ROOK][BLACK][index_black[s]] = non_pawn_bonus_eg[ROOK - 2][rank][file_adjusted];

		data.MidGamePieceSquareScore[ROOK][WHITE][index_white[s]] = non_pawn_bonus_mg[ROOK - 2][rank][file_adjusted];
		data.MidGamePieceSquareScore[ROOK][BLACK][index_black[s]] = non_pawn_bonus_mg[ROOK - 2][rank][file_adjusted];

		data.EndGamePieceSquareScore[KNIGHT][WHITE][index_white[s]] = non_pawn_bonus_eg[KNIGHT - 2][rank][file_adjusted];
		data.EndGamePieceSquareScore[KNIGHT][BLACK][index_black[s]] = non_pawn_bonus_eg[KNIGHT - 2][rank][file_adjusted];

		data.MidGamePieceSquareScore[KNIGHT][WHITE][index_white[s]] = non_pawn_bonus_mg[KNIGHT - 2][rank][file_adjusted];
		data.MidGamePieceSquareScore[KNIGHT][BLACK][index_black[s]] = non_pawn_bonus_mg[KNIGHT - 2][rank][file_adjusted];

		data.EndGamePieceSquareScore[BISHOP][WHITE][index_white[s]] = non_pawn_bonus_eg[BISHOP - 2][rank][file_adjusted];
		data.EndGamePieceSquareScore[BISHOP][BLACK][index_black[s]] = non_pawn_bonus_eg[BISHOP - 2][rank][file_adjusted];

		data.MidGamePieceSquareScore[BISHOP][WHITE][index_white[s]] = non_pawn_bonus_mg[BISHOP - 2][rank][file_adjusted];
		data.MidGamePieceSquareScore[BISHOP][BLACK][index_black[s]] = non_pawn_bonus_mg[BISHOP - 2][rank][file_adjusted];

		data.EndGamePieceSquareScore[QUEEN][WHITE][index_white[s]] = non_pawn_bonus_eg[QUEEN - 2][rank][file_adjusted];
		data.EndGamePieceSquareScore[QUEEN][BLACK][index_black[s]] = non_pawn_bonus_eg[QUEEN - 2][rank][file_adjusted];

		data.MidGamePieceSquareScore[QUEEN][WHITE][index_white[s]] = non_pawn_bonus_mg[QUEEN - 2][rank][file_adjusted];
		data.MidGamePieceSquareScore[QUEEN][BLACK][index_black[s]] = non_pawn_bonus_mg[QUEEN - 2][rank][file_adjusted];

		data.EndGamePieceSquareScore[KING][WHITE][index_white[s]] = non_pawn_bonus_eg[KING - 2][rank][file_adjusted];
		data.EndGamePieceSquareScore[KING][BLACK][index_black[s]] = non_pawn_bonus_eg[KING - 2][rank][file_adjusted];

		data.MidGamePieceSquareScore[KING][WHITE][index_white[s]] = non_pawn_bonus_mg[KING - 2][rank][file_adjusted];
		data.MidGamePieceSquareScore[KING][BLACK][index_black[s]] = non_pawn_bonus_mg[KING - 2][rank][file_adjusted];
	}

	sliders = new Panzer::Sliders();
	sliders->Initialize();
};

int MaterialImbalance(int p, int k, int b, int r, int q)
{
	return (38 * p * p) + k * (-62 * k) + k * (318 * p)+ (b * (46 * k + 169 * p)) + ((-208 * r * r) + r * (81 * b + 71 * k + 37 *p)) +( -6 *q * q + q * (134 * r + 270 * b + 75 * k + 124 * p)) + (b > 0 ? 1 : 0) * (76 * p + 41 * k + 59 * b + 20 * r - 92 *q);
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
	//auto blackPieces = board.GetBlackPieces();
	auto phase = PAWN_PHASE * (whitePawnCount + blackPawnCount)
		+ QUEEN_PHASE * (whiteQueenCount + blackQueenCount)
		+ ROOK_PHASE * (whiteRookCount + blackRookCount)
		+ BISHOP_PHASE * (whiteBishopCount + blackBishopCount)
		+ KNIGHT_PHASE * (whiteKnightCount + blackKnightCount);
	
	phase = ((TOTAL_PHASE - phase) * 256 + (TOTAL_PHASE / 2)) / TOTAL_PHASE;

	EvalScores whiteScores;
	EvalScores blackScores;

	auto whiteMaterialImbalance = 0;
	whiteMaterialImbalance = MaterialImbalance(whitePawnCount, whiteKnightCount, whiteBishopCount, whiteRookCount, whiteQueenCount);
	whiteScores.mid += whiteMaterialImbalance;
	whiteScores.end += whiteMaterialImbalance;

	auto blackMaterialImbalance = 0;
	blackMaterialImbalance = MaterialImbalance(blackPawnCount, blackKnightCount, blackBishopCount, blackRookCount, blackQueenCount);

	blackScores.mid += blackMaterialImbalance;
	blackScores.end += blackMaterialImbalance;

	for (square s = A1; s <= H8; s++)
	{
		auto piece = board.GetPieceAtSquare(s);

		if (piece != NO_PIECE)
		{
			auto color = ((ONE_BIT << s) & whitePieces) ? WHITE : BLACK;
			if (color == WHITE)
			{
				whiteScores.mid += data.MidGamePieceSquareScore[piece][WHITE][index_white[s]];
				whiteScores.end += data.EndGamePieceSquareScore[piece][WHITE][index_white[s]];
			}
			else
			{
				whiteScores.mid += data.MidGamePieceSquareScore[piece][BLACK][s];
				whiteScores.end += data.EndGamePieceSquareScore[piece][BLACK][s];
			}
		}
	}


	// need to just returrn two values indicating Mid and End Scores
	EvaluatePawns<WHITE>(board, whiteScores);
	EvaluatePawns<BLACK>(board, blackScores);

	// TODO: Implement
	EvaluateOutposts<WHITE>(board, whiteScores);
	EvaluateOutposts<WHITE>(board, blackScores);

	MinorBehindPawns<WHITE>(board, whiteScores);
	MinorBehindPawns<BLACK>(board, blackScores);

	BishopXRayPawns<WHITE, BLACK>(board, whiteScores);
	BishopXRayPawns<BLACK, WHITE>(board, blackScores);

	RookScores<WHITE, BLACK>(board, whiteScores);
	RookScores<BLACK, WHITE>(board, blackScores);

	Mobility<WHITE>(board, whiteScores);
	Mobility<BLACK>(board, blackScores);

	int whiteAttackLookup[64] = {0};
	int blackAttackLookup[64] = {0};
	Attacks<WHITE>(board, whiteAttackLookup);
	Attacks<BLACK>(board, blackAttackLookup);
	// TODO: Trapped rook

	int whiteScore = 0;
	int blackScore = 0;


	whiteScore = (whiteScores.mid * (phase) + ((TOTAL_PHASE - phase) * whiteScores.end)) / TOTAL_PHASE;
	blackScore = (blackScores.mid + (phase) + ((TOTAL_PHASE - phase) * blackScores.end)) / TOTAL_PHASE;
	// TEMPO BONUS
	whiteScore += board.GetSideToMove() == WHITE ? 28 : 0;
	blackScore += board.GetSideToMove() == BLACK ? 28 : 0;

	whiteScore = (whiteScore * (100 - board.GetHalfClock())) / 100;
	blackScore = (blackScore * (100 - board.GetHalfClock())) / 100;
	auto score = whiteScore - blackScore;

	if (score == 0)  { 
		score = 1; 
	}
	//std::cout << "done eval" << std::endl;
	// equivalent to WHITE ? score : -1 * score
	score = board.GetSideToMove() == WHITE ? score : -1 * score;
	return score;
};

	template <color c>
	void EvaluatePawns(const Board &board, EvalScores &scores)
	{
		bitboard pawns = Pawns<c>(board);

		bitboard other_pawns = Pawns<c == WHITE ? BLACK : WHITE>(board);

		if (pawnHash.contains(pawns ^ other_pawns))
		{
			auto hashed = pawnHash[pawns ^ other_pawns];
			scores.mid += hashed.mid;
			scores.end += hashed.end;
		}

		EvalScores pawnScores;

		if (c == BLACK) 
		{  
			pawns = __builtin_bswap64(pawns);
			other_pawns = __builtin_bswap64(other_pawns);
		}

		int isolated = 0;
		int supported = 0;
		int phalanx = 0;
		int opposed = 0;
		int outposts = 0;
		int connected_bonus = 0;
		int blockedScore = 0;
		int backwards = 0;
		int doubled = 0;


		while (pawns != 0)
		{
			square s = Utils::GetLSB(pawns);

			auto file = SQUARE_TO_FILE[s];
			bitboard adjacent = ADJACENT_FILE_MASKS[s % 8];
			// phalanx
			if (adjacent & SQUARE_TO_RANK[s])
			{
				phalanx++;
			}

			// isolated
			if ((adjacent & pawns) == 0)
			{
				isolated++;
			}

			// supported
			supported = __builtin_popcount(PAWN_SUPPORT_MASK[s] & pawns);

			auto rank = SQUARE_TO_FILE[s];

			if (pawns & rank & ~(ONE_BIT << s))
			{
				doubled++;
			}

			// backwards : Pawn is behind other pawns next to it and can't advance
			bitboard upper = (~ONE_BIT << s) & ~SQUARE_TO_RANK[s];
			bitboard attackingPawn = (other_pawns & (SQUARE_TO_RANK[s] << NN));

			if ((upper & adjacent & attackingPawn) != 0)
			{
				backwards++;
			}

			if (file & other_pawns)
			{
				opposed++;
			}

			// blocked loss
			int fileNum = s / 8;
			if ((fileNum < 4) & (other_pawns & file))
			{
				blockedScore -= PAWN_BLOCKED[4 - fileNum];
			}

			connected_bonus += PAWN_CONNECTED_BONUS[fileNum] * (2 + phalanx - opposed) + 21 * supported;

			pawns &= pawns - 1;
		}

		pawnHash[pawns ^ other_pawns] = pawnScores;
		scores.mid += pawnScores.mid;
		scores.end += pawnScores.end;
	};

	template <color c>
	void EvaluateControl(const Board &board, EvalScores &scores)
	{
		// does this color control the center of the board at all?
		bitboard occupancy = board.GetOccupancy();

		// any pawns?
		bitboard pawns = Pawns<c>(board);
		bitboard rooks = Rooks<c>(board);
		bitboard queens = Queens<c>(board);
		bitboard bishops = Bishops<c>(board);
		bitboard knights = Knights<c>(board);

		if (pawns & CENTER_PAWN_MASKS[c])
		{
			scores.mid += 15;
			scores.end += 10;
		}
		int centerSquare[4] = {E4, D4, E5, D5};
		for (auto square: centerSquare)
		{
			// any rooks or queens attacking center
			auto lineAttacks = sliders->GetRookAttacks(square, occupancy);
			if (lineAttacks & (rooks)) {
				scores.mid += 2;
			}
			// any bishops or queens attacking center
			auto diagonalAttacks = sliders->GetBishopAttacks(square, occupancy);
			if (diagonalAttacks & (bishops)) {
				scores.mid += 2;
			}

			if ((lineAttacks | diagonalAttacks) & queens)
			{
				scores.mid += 2;
			}

			// any knights attacking center
			if (KNIGHT_SPANS[square] & knights)
			{
				scores.mid += 10;
			}
		}
	};

	template <color c>
	void EvaluateOutposts(const Board& board, EvalScores &scores)
	{
	}

	template<color c>
	void MinorBehindPawns(const Board& board, EvalScores &scores)
	{
		bitboard minorpieces = Bishops<c>(board) | Knights<c>(board);
		bitboard pawns = Pawns<c>(board);
		if (c == BLACK) { 
			pawns = __builtin_bswap64(pawns);
			minorpieces = __builtin_bswap64(minorpieces); 
		}

		auto behind = __builtin_popcount((minorpieces << N) & pawns);
		scores.mid += 18 * behind;
		scores.end += 3 * behind;
	}

	template<color us, color them>
	void BishopXRayPawns(const Board& board, EvalScores &scores)
	{
		int count = 0;
		bitboard bishops = Bishops<us>(board);
		bitboard pawns = Pawns<them>(board);
		if (pawns == 0) return;

		while (bishops != 0)
		{
			int s = Utils::GetLSB(bishops);

			int xray = sliders->GetBishopAttacks(s, 0);

			count += __builtin_popcount(xray ^ pawns);
			bishops &= bishops - 1;
		}

		scores.mid -= 4 * count;
		scores.end -= 5 * count;
	}

	template<color us, color them>
	void RookScores(const Board& board, EvalScores &scores)
	{
		bitboard rooks = Rooks<us>(board);
		bitboard pawnsUs = Pawns<us>(board);
		bitboard pawnsThem = Pawns<them>(board);
		bitboard kingsThem = Kings<them>(board);

		int count = 0;
		bitboard queens = Queens<us>(board);

		// do the rooks share a file with the queen
		while(queens != 0)
		{
			square s = Utils::GetLSB(queens);

			mask file = SQUARE_TO_FILE[s];
			count += __builtin_popcount(rooks ^ file);
			queens &= queens - 1;
		}

		scores.mid += 6 * count;
		scores.end += 11 * count;

		while (rooks != 0)
		{
			square s = Utils::GetLSB(rooks);
			mask file = SQUARE_TO_FILE[s];

			// is the rook on a semiopen/open file
			if ((file & pawnsUs) == 0) 
			{
				if ((file & pawnsThem) == 0)	
				{
					scores.mid += 48;
					scores.end += 29;
				}
				else
				{
					scores.mid += 19;
					scores.end += 7;
				}
			}

			square ks = Utils::GetLSB(kingsThem);
			
			// is the rook on the king ring but not attacking
			mask kingRing = KING_SPANS[ks];
			mask rank = SQUARE_TO_RANK[s];
			if ((kingRing & (ONE_BIT << s)) == 0 && ((rank | file) & kingRing) != 0)
			{
				scores.mid += 16;
			}

			// TODO: is the rook trapped

			rooks &= rooks - 1;
		}

	}

	template<color us, color them>
	void WeakQueen(const Board& board, EvalScores &scores)
	{
		bitboard queens = Queens<us>(board);
		bitboard bishops = Bishops<us>(board);
		bitboard rooks = Rooks<us>(board);

		bitboard otherBishops = Bishops<them>(board);
		bitboard otherRooks = Rooks<them>(board);

		bitboard occupancy = board.GetOccupancy();

		while (queens != 0)
		{
			square s = Utils::GetLSB(queens);

			bool isWeak;
			mask queenAttacks = sliders->GetQueenAttacks(s, occupancy);

			// are we running into our own bishops or rooks
			if (queenAttacks & (bishops | rooks))
			{
				queenAttacks ^= (bishops | rooks);
				bitboard xrayoccupancy = (occupancy ^ queenAttacks);

				mask rookDirections = sliders->GetRookAttacks(s, xrayoccupancy);
				if (rookDirections & otherRooks)
				{
					isWeak = true;
				}
				else 
				{
					mask bishopDirections = sliders->GetBishopAttacks(s, xrayoccupancy);
					if (bishopDirections & otherBishops)
					{
						isWeak = true;
					}
				}

				if (isWeak)
				{
					scores.mid -= 56;
					scores.end -= 15;
				}
			}
			
			queens &= queens - 1;
		}
	}

	template<color c>
	void Mobility(const Board &board, const EvalScores &scores)
	{
		int mobility = 0;
		bitboard queens = Queens<c>(board);
		bitboard rooks = Rooks<c>(board);
		bitboard knights = Knights<c>(board);
		bitboard bishops = Bishops<c>(board);
		bitboard us = Pieces<c>(board);
		bitboard occupancy = board.GetOccupancy();

		while (rooks != 0)
		{
			square s = Utils::GetLSB(rooks);
			int num = __builtin_popcount(sliders->GetRookAttacks(s, occupancy ^ queens));
			scores.mid += ROOK_MOBILITY_BONUS[0][num];
			scores.end += ROOK_MOBILITY_BONUS[1][num];

			rooks &= rooks - 1;
		}

		while (knights != 0)
		{
			square s = Utils::GetLSB(knights);
			int num = __builtin_popcount(KNIGHT_SPANS[s] & ~(us ^ queens));
			scores.mid += ROOK_MOBILITY_BONUS[0][num];
			scores.end += ROOK_MOBILITY_BONUS[1][num];

			knights &= knights - 1;
		}

		while (bishops != 0)
		{
			square s = Utils::GetLSB(bishops);
			int num = __builtin_popcount(sliders->GetBishopAttacks(s, occupancy ^ queens));
			scores.mid = BISHOP_MOBILITY_BONUS[0][num];
			scores.end = BISHOP_MOBILITY_BONUS[1][num];
			bishops &= bishops - 1;
		}

		// queens last
		while (queens != 0)
		{
			square s = Utils::GetLSB(queens);
			int num = __builtin_popcount(sliders->GetQueenAttacks(s, occupancy));
			scores.mid += QUEEN_MOBILITY_BONUS[0][num];
			scores.end += QUEEN_MOBILITY_BONUS[1][num];
			queens &= queens - 1;
		}
	}

	template<color c>
	void Attacks(const Board &board, int* attackLookup)
	{
		auto queens = Queens<c>(board);
		auto rooks = Rooks<c>(board);
		auto bishops = Bishops<c>(board);
		auto knights = Knights<c>(board);
		auto king = Kings<c>(board);
		auto pawns = Pawns<c>(board);
		auto occupancy = board.GetOccupancy();


		while (rooks != 0)
		{
			square s = Utils::GetLSB(rooks);
			mask attacks = sliders->GetRooksAttacks(s, occupancy ^ queens);
			while (attacks != 0)
			{
				square a = Utils::GetLSB(attacks);
				attackLookup[a]++;
				attackLookup[a] |= 0b00000100;
				attacks &= attacks - 1;
			}

			rooks &= rooks - 1;
		}

		while (bishops != 0)
		{
			square s = Utils::GetLSB(bishops);
			mask attacks = sliders->GetBishopAttacks(s, occupancy ^ queens);
			while (attacks != 0)
			{
				square a = Utils::GetLSB(attacks);
				attackLookup[a]++;
				attackLookup[a] |= 0b00001000;
				attacks &= attacks - 1;
			}
		}

		while (knights != 0)
		{
			square s = Utils::GetLSB(knights);
			mask attacks = knight_move_masks[s];
			while (attacks != 0)
			{
				square a = Utils::GetLSB(attacks);
				attackLookup[a]++;
				attackLookup[a] |= 0b00001000;
				attacks &= attacks - 1;
			}
		}

		mask* pawnsAttacks = PAWN_ATTACKS[c];
		while (pawns != 0)
		{
			square s = Utils::GetLSB(pawns);
			mask attacks = PAWN_ATTACKS[s];
			while (attacks != 0)
			{
				square a = Utils::GetLSB(attacks);
				attackLookup[a]++;
				attackLookup[a] |= 0b00010000;
				attacks &= attacks - 1;
			}
			
			pawns &= pawns - 1;
		}

		while (king != 0)
		{
			square s = Utils::GetLSB(king);
			mask attacks = KING_SPANS[s];
			while (attacks != 0)
			{
				square a = Utils::GetLSB(attacks);
				attackLookup[a]++;
				attackLookup[a] |= 0b00000001;
				attacks &= attacks - 1;
			}

			king &= king - 1;
		}

		while (queens != 0)
		{
			square s = Utils::GetLSB(queens);
			mask attacks = sliders->GetQueenAttacks(s, occupancy);
			while (attacks != 0)
			{
				square a = Utils::GetLSB(attacks);
				attackLookup[a]++;
				attackLookup[a] |= 0b00000010;
				attacks &= attacks - 1;
			}

			queens &= queens - 1;
		}
	}

	template <color c>
	int SpaceArea(const Board& board, int* attackLookup)
	{
		int sum = 0;
		const pawns = Pawns<c>(board);
		square *area = SPACE[c];
		for (int i = 0; i < 12; i++)
		{
			square s = area[i];
			// not attacked and not filled with a apwn
			if ((attackLookup[s] & 0b111) == 0 && (pawns & (ONE_BIT << s)) == 0)	
			{
				sum++;
			}
		}

		return sum;
	}

	template<color c>
	void Space(const Board& board, EvalScores &scores)
	{
		
	}
};
