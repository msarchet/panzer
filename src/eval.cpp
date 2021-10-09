#include "eval.h"
#include "bitboard.h"
#include "board_utils.h"
#include "sliders.h"

#include <unordered_map>

//#define PRINT_EVAL
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
	auto whitePawnCount = Utils::GetPopcount(board.GetWhitePawns());
	auto whiteRookCount = Utils::GetPopcount(board.GetWhiteRooks());
	auto whiteBishopCount = Utils::GetPopcount(board.GetWhiteBishops());
	auto whiteKnightCount = Utils::GetPopcount(board.GetWhiteKnights());
	auto whiteQueenCount = Utils::GetPopcount(board.GetWhiteQueens());

	auto blackPawnCount = Utils::GetPopcount(board.GetBlackPawns());
	auto blackRookCount = Utils::GetPopcount(board.GetBlackRooks());
	auto blackBishopCount = Utils::GetPopcount(board.GetBlackBishops());
	auto blackKnightCount = Utils::GetPopcount(board.GetBlackKnights());
	auto blackQueenCount = Utils::GetPopcount(board.GetBlackQueens());

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
	#ifdef PRINT_EVAL
		std::cout << "MATERIAL IMBALANCE" << std::endl;
		std::cout << "WHITE: " << whiteScores.mid << " " << whiteScores.end << std::endl;
		std::cout << "BLACK: " << blackScores.mid << " " << blackScores.end << std::endl;
	#endif

	auto blackPSMid = 0;
	auto whitePSMid = 0;
	auto blackPSEnd = 0;
	auto whitePSEnd = 0;

	for (square s = A1; s <= H8; s++)
	{
		auto piece = board.GetPieceAtSquare(s);

		if (piece != NO_PIECE)
		{
			auto color = ((ONE_BIT << s) & whitePieces) ? WHITE : BLACK;
			if (color == WHITE)
			{
				whitePSMid += data.MidGamePieceSquareScore[piece][WHITE][index_white[s]];
				whitePSEnd += data.EndGamePieceSquareScore[piece][WHITE][index_white[s]];
			}
			else
			{
				blackPSMid += data.MidGamePieceSquareScore[piece][BLACK][s];
				blackPSEnd += data.EndGamePieceSquareScore[piece][BLACK][s];
			}
		}
	}

	whiteScores.mid += whitePSMid;
	whiteScores.end += whitePSEnd;

	blackScores.mid += blackPSMid;
	blackScores.end += blackPSEnd;

	#ifdef PRINT_EVAL
		std::cout << "PST" << std::endl;
		std::cout << "WHITE: " << whiteScores.mid << " " << whiteScores.end << std::endl;
		std::cout << "BLACK: " << blackScores.mid << " " << blackScores.end << std::endl;
	#endif


	// need to just returrn two values indicating Mid and End Scores
	EvaluatePawns<WHITE>(board, whiteScores);
	EvaluatePawns<BLACK>(board, blackScores);

	#ifdef PRINT_EVAL
		std::cout << "PAWNS" << std::endl;
		std::cout << "WHITE: " << whiteScores.mid << " " << whiteScores.end << std::endl;
		std::cout << "BLACK: " << blackScores.mid << " " << blackScores.end << std::endl;
	#endif

	// TODO: Implement
	EvaluateOutposts<WHITE>(board, whiteScores);
	EvaluateOutposts<WHITE>(board, blackScores);

	MinorBehindPawns<WHITE>(board, whiteScores);
	MinorBehindPawns<BLACK>(board, blackScores);

	#ifdef PRINT_EVAL
		std::cout << "MINOR BEHIND PAWNS" << std::endl;
		std::cout << "WHITE: " << whiteScores.mid << " " << whiteScores.end << std::endl;
		std::cout << "BLACK: " << blackScores.mid << " " << blackScores.end << std::endl;
	#endif

	BishopXRayPawns<WHITE, BLACK>(board, whiteScores);
	BishopXRayPawns<BLACK, WHITE>(board, blackScores);

	#ifdef PRINT_EVAL
		std::cout << "BISHOP XRAY PAWNS" << std::endl;
		std::cout << "WHITE: " << whiteScores.mid << " " << whiteScores.end << std::endl;
		std::cout << "BLACK: " << blackScores.mid << " " << blackScores.end << std::endl;
	#endif

	RookScores<WHITE, BLACK>(board, whiteScores);
	RookScores<BLACK, WHITE>(board, blackScores);

	#ifdef PRINT_EVAL
		std::cout << "ROOK SCORES" << std::endl;
		std::cout << "WHITE: " << whiteScores.mid << " " << whiteScores.end << std::endl;
		std::cout << "BLACK: " << blackScores.mid << " " << blackScores.end << std::endl;
	#endif

	Mobility<WHITE>(board, whiteScores);
	Mobility<BLACK>(board, blackScores);

	int whiteAttackLookup[64] = {0};
	int blackAttackLookup[64] = {0};
	Attacks<WHITE>(board, whiteAttackLookup);
	Attacks<BLACK>(board, blackAttackLookup);
	int whiteWeakLookup[64] = {0};
	int blackWeakLookup[64] = {0};
	WeakAndHanging<WHITE>(board, whiteAttackLookup, blackAttackLookup, whiteWeakLookup);
	WeakAndHanging<BLACK>(board, blackAttackLookup, whiteAttackLookup, blackWeakLookup);

	Space<WHITE>(board, whiteScores, SpaceArea<WHITE>(board, whiteAttackLookup));
	Space<BLACK>(board, blackScores, SpaceArea<BLACK>(board, blackAttackLookup));
	#ifdef PRINT_EVAL
		std::cout << "SPACE" << std::endl;
		std::cout << "WHITE: " << whiteScores.mid << " " << whiteScores.end << std::endl;
		std::cout << "BLACK: " << blackScores.mid << " " << blackScores.end << std::endl;
	#endif

	int whiteScore = 0;
	int blackScore = 0;

	Threats<WHITE, BLACK>(board, whiteAttackLookup, blackAttackLookup, whiteWeakLookup, blackWeakLookup, whiteScores);
	Threats<BLACK, WHITE>(board, blackAttackLookup, whiteAttackLookup, blackWeakLookup, whiteWeakLookup, blackScores);
	#ifdef PRINT_EVAL
		std::cout << "THREATS" << std::endl;
		std::cout << "WHITE: " << whiteScores.mid << " " << whiteScores.end << std::endl;
		std::cout << "BLACK: " << blackScores.mid << " " << blackScores.end << std::endl;
	#endif

	whiteScore = (whiteScores.mid * (phase) + ((TOTAL_PHASE - phase) * whiteScores.end)) / TOTAL_PHASE;
	blackScore = (blackScores.mid + (phase) + ((TOTAL_PHASE - phase) * blackScores.end)) / TOTAL_PHASE;

	#ifdef PRINT_EVAL
		std::cout << "PHASE ADJUST" << std::endl;
		std::cout << "PHASE: " << phase << std::endl;
		std::cout << "WHITE: " << whiteScores.mid << " " << whiteScores.end << std::endl;
		std::cout << "BLACK: " << blackScores.mid << " " << blackScores.end << std::endl;
	#endif

	// TEMPO BONUS
	whiteScore += board.GetSideToMove() == WHITE ? 28 : 0;
	blackScore += board.GetSideToMove() == BLACK ? 28 : 0;

	whiteScore = (whiteScore * (100 - board.GetHalfClock())) / 100;
	blackScore = (blackScore * (100 - board.GetHalfClock())) / 100;

	auto score = whiteScore - blackScore;
	#ifdef PRINT_EVAL
		std::cout << "FINAL SCORE: " << score << std::endl;
	#endif

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
			pawns = Utils::SwapBits(pawns);
			other_pawns = Utils::SwapBits(other_pawns);
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
			supported = Utils::GetPopcount(PAWN_SUPPORT_MASK[s] & pawns);

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
			if ((fileNum < 4) && (other_pawns & file))
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
			pawns = Utils::SwapBits(pawns);
			minorpieces = Utils::SwapBits(minorpieces); 
		}

		auto behind = Utils::GetPopcount((minorpieces << N) & pawns);
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

			count += Utils::GetPopcount(xray ^ pawns);
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
		bitboard queens = Queens<us>(board) | Queens<them>(board);

		// do the rooks share a file with the queen
		while(queens != 0)
		{
			square s = Utils::GetLSB(queens);

			mask file = SQUARE_TO_FILE[s];
			count += Utils::GetPopcount(rooks & file);
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
		mask bishopRooks = (bishops | rooks);

		while (queens != 0)
		{
			square s = Utils::GetLSB(queens);

			bool isWeak;
			mask queenAttacks = sliders->GetQueenAttacks(s, occupancy);

			// are we running into our own bishops or rooks
			if (queenAttacks & bishopRooks)
			{
				queenAttacks = sliders->GetQueenAttacks(s, occupancy ^ bishopRooks);

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
	void Mobility(const Board &board, EvalScores &scores)
	{
		bitboard queens = Queens<c>(board);
		bitboard rooks = Rooks<c>(board);
		bitboard knights = Knights<c>(board);
		bitboard bishops = Bishops<c>(board);
		bitboard us = Pieces<c>(board);
		bitboard occupancy = board.GetOccupancy();

		while (rooks != 0)
		{
			square s = Utils::GetLSB(rooks);
			int num = Utils::GetPopcount(sliders->GetRookAttacks(s, occupancy ^ queens));
			scores.mid += ROOK_MOBILITY_BONUS[0][num];
			scores.end += ROOK_MOBILITY_BONUS[1][num];
			rooks &= rooks - 1;
		}

		while (knights != 0)
		{
			square s = Utils::GetLSB(knights);
			int num = Utils::GetPopcount(KNIGHT_SPANS[s] & ~(us ^ queens));
			scores.mid += ROOK_MOBILITY_BONUS[0][num];
			scores.end += ROOK_MOBILITY_BONUS[1][num];
			knights &= knights - 1;
		}

		while (bishops != 0)
		{
			square s = Utils::GetLSB(bishops);
			int num = Utils::GetPopcount(sliders->GetBishopAttacks(s, occupancy ^ queens));
			scores.mid += BISHOP_MOBILITY_BONUS[0][num];
			scores.end += BISHOP_MOBILITY_BONUS[1][num];
			bishops &= bishops - 1;
		}

		// queens last
		while (queens != 0)
		{
			square s = Utils::GetLSB(queens);
			int num = Utils::GetPopcount(sliders->GetQueenAttacks(s, occupancy));
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
			mask attacks = sliders->GetRookAttacks(s, occupancy ^ queens);
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

			bishops &= bishops - 1;
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

			knights &= knights - 1;
		}

		const mask *pawnAttacks = PAWN_ATTACKS[c];
		while (pawns != 0)
		{
			square s = Utils::GetLSB(pawns);
			mask attacks = pawnAttacks[s];
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
		auto pawns = Pawns<c>(board);
		const auto occupancy = Pieces<c>(board);

		const square *area = SPACE[c];
		for (int i = 0; i < 12; i++)
		{
			square s = area[i];
			mask m = ONE_BIT << s;
			bool isAttacked = (attackLookup[s] & 0b111) == 0;
			bool isAttackedByPawn = isAttacked && (attackLookup[s] & 0b0001);

			if (!isAttackedByPawn && (pawns & m) == 0)	
			{
				sum++;
				// not attacked gets a bonus for protected by a forward pawn
				if (!isAttacked && i > 4)
				{
					// TODO: file behind pawn (need to flip for black)
					mask fileahead = SQUARE_TO_FILE[s] & (~ONE_BIT << s);
					if (fileahead & pawns)
					{
						sum++;
					}
				}
			}

		}

		return sum;
	}

	template<color c>
	void Space(const Board& board, EvalScores &scores, int spaceArea)
	{
		const bitboard pieces = Pieces<c>(board);
		int pieceCount = Utils::GetPopcount(pieces);
		
		// blocked count is pawns we are stopping from advancing
		// and pawns they are stopping from advancing
		// stopped is pawn directly in front or a pawn on an adjacent file
		// and two squares back, where that pawn can't move into the square ahead of 
		// it due to being able to be captured by our pawn
		int blockedCount = 0;
		int weight = pieceCount - 3 + std::min(blockedCount, 9);
		scores.mid += weight * weight * spaceArea / 16;
	}

	template<color c>
	void WeakAndHanging(const Board &board, int *attacksUs, int *attacksThem, int *weakLookup)
	{
		auto pieces = Pieces<c>(board);
		while (pieces != 0)	
		{
			const auto s = Utils::GetLSB(pieces);
			if ((attacksThem[s] & 0b111) != 0 && (attacksUs[s] & 0b0001) != 0)
			{
				//weak
				weakLookup[s] = 0b1;
				// hanging
				weakLookup[s] = 0b01 && ((attacksThem[s] & 0b111) > 1);
			}
			pieces &= pieces - 1;
		}
	}

	template<color us, color them>
	void Threats(const Board& board, int *attacksUs, int *attacksThem, int *weakUs, int *weakThem, EvalScores &scores)
	{
		auto ourPawns = Pawns<us>(board);
		auto ourBishops = Bishops<us>(board);
		auto ourKnights = Knights<us>(board);
		auto ourRooks = Rooks<us>(board);
		auto ourKing = Kings<us>(board);
		auto theirKnights = Knights<them>(board);
		auto theirRooks = Rooks<them>(board);
		auto theirBishops = Bishops<them>(board);
		auto theirPieces = Pieces<them>(board);
		auto theirQueens = Queens<them>(board);
		auto occupancy = board.GetOccupancy();

		for (square s = A1; s < NO_SQUARE; s++)
		{
			auto weakSquareThem = weakThem[s];
			auto attackSquareThem = attacksThem[s];

			// hanging pieces
			if ((weakThem[s] & 0b11) == 3)
			{
				scores.mid += 69;
				scores.end += 36;
			}

			// king threat
			if (attacksUs[s] & 0b00000001)
			{
				scores.mid += 24;
				scores.end += 89;
			}

			// restricted 
			// this is an intersection of squares where we are both attacking
			int restricted = 0;
			if ((attacksUs[s] & 0b111) > 0 && (attacksThem[s] & 0b111) > 0)
			{
				restricted++;
			}

			scores.mid += 7 * restricted;
			scores.end += 7 * restricted;
		}

		// can we advance our pawns toward their pieces and attack safely
		auto pawnAttacks = PAWN_ATTACKS[us];
		mask pawnPushes = ourPawns;
		if (us == BLACK)
		{
			pawnPushes = ourPawns >> S;
		}
		else
		{
			pawnPushes = ourPawns << N;
		}

		// mask out pawns that would be promoted
		pawnPushes = (pawnPushes & ~(FIRST_RANK | EIGHTH_RANK));

		while (pawnPushes != 0)
		{
			square s = Utils::GetLSB(pawnPushes);
			auto potentialAttacks = pawnAttacks[s];

			while (potentialAttacks != 0 && (potentialAttacks & theirPieces))
			{
				square target = Utils::GetLSB(potentialAttacks);

				if ((attacksThem[target] & 0b111) == 0)
				{
					scores.mid += 48;
					scores.end += 48;
				}

				potentialAttacks &= potentialAttacks - 1;
			}
			pawnPushes &= pawnPushes - 1;
		}

		// do we have pawns that are attacking and are defended
		auto pawnsSafe = ourPawns;
		while (pawnsSafe != 0)
		{
			square s = Utils::GetLSB(pawnsSafe);

			// is the pawn attacking something
			// is the pawn not being attacked or is the pawn guarded in it's attacked
			auto potentialAttacks = (pawnAttacks[s] & theirPieces);
			bool isAttackedByThem = (attacksThem[s] & 0b111) > 0;
			if (!isAttackedByThem || (attacksUs[s] & 0b111) > 0)
			{
				auto safeAttacks = Utils::GetPopcount(potentialAttacks);
				scores.mid += safeAttacks * 173;
				scores.end += safeAttacks * 94;
			}

			pawnsSafe &= pawnsSafe - 1;
		}

		// safe slider threats on enemy queen
		// can a piece move into a square that attacks the queen
		// and is defended
		// for the occupancy we remove any of our sliders
		mask xrayOccupancy = occupancy ^ theirQueens;

		auto attackBishops = ourBishops;
		auto attackRooks = ourRooks;
		mask bishopXRay = 0ULL;
		mask rookXRay = 0ULL;
		mask attackMask = 0ULL;

		while (attackBishops != 0)
		{
			square s = Utils::GetLSB(attackBishops);
			bishopXRay |= sliders->GetBishopAttacks(s, xrayOccupancy);
			attackBishops &= attackBishops - 1;
		}

		while (attackRooks != 0)
		{
			square s = Utils::GetLSB(attackRooks);
			rookXRay |= sliders->GetRookAttacks(s, xrayOccupancy);
			attackRooks &= attackRooks - 1;
		}

		attackMask = (bishopXRay & rookXRay);

		int sliderAttackCount = 0;
		while (attackMask != 0)
		{
			// if the square is attacked by the enemy queen 
			// and deneded by us

			square s = Utils::GetLSB(attackMask);

			if ((attacksThem[s] & 0b00000010) && (attacksUs[s] & 0b111))
			{
				sliderAttackCount++;
			}

			attackMask &= attackMask - 1;
		}

		scores.mid += 60 * sliderAttackCount;
		scores.end += 18 * sliderAttackCount;

		mask bishopXrayTarget = bishopXRay & theirPieces;

		while (bishopXrayTarget != 0)
		{
			square s = Utils::GetLSB(bishopXrayTarget);

			piece target = board.GetPieceAtSquare(s);
			scores.mid += MINOR_THREATS_MID[target];
			scores.end += MINOR_THREATS_END[target];
			bishopXrayTarget &= bishopXrayTarget -1;
		}

		mask rookXrayTarget = rookXRay & theirPieces;

		while (rookXrayTarget != 0)
		{
			square s = Utils::GetLSB(rookXrayTarget);

			piece target = board.GetPieceAtSquare(s);
			scores.mid += ROOK_THREATS_MID[target];
			scores.end += ROOK_THREATS_END[target];
			rookXrayTarget &= rookXrayTarget -1;
		}

		// does our knight have any moves they can make
		// that attack the queen
		bitboard queenKnightCheck = theirQueens;
		bitboard knightsCopy = ourKnights;
		mask potentialKnightMoves = 0ULL;
		while (knightsCopy != 0)
		{
			square s = Utils::GetLSB(knightsCopy);
			potentialKnightMoves |= KNIGHT_SPANS[s];
			knightsCopy &= knightsCopy - 1;
		}
		mask potentialMoves = 0ULL; 

		while (queenKnightCheck != 0)
		{
			square s = Utils::GetLSB(queenKnightCheck);
			potentialMoves |= KNIGHT_SPANS[s];	
			queenKnightCheck &= queenKnightCheck - 1;
		}

		int intersects = Utils::GetPopcount(potentialKnightMoves & potentialMoves);

		scores.mid += 16 * intersects;
		scores.end += 11 * intersects;

	}
};
