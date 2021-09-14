#include "bitboard.h"
#include "bitboard_constants.h"
#include "utils/board_utils.h"
#include <sstream>

namespace Panzer
{
	piece Board_Bit::GetPieceAtSquare(square s)
	{
		return pieceLookup->at(s);
	}

	bool Board_Bit::IsChecked(color color)
	{
		if (color == WHITE)
		{
			bitboard king = this->GetWhiteKings();
			square kingSquare = GetLSB(king);
			bitboard pawnMask = ((king & ~A_FILE) << NW)| ((king & ~H_FILE) << NE); // shift NW
			bitboard diagonals = this->GetBlackBishops() | this->GetBlackQueens() | (this->GetBlackPawns() & pawnMask);
			bitboard occupancy = this->GetOccupancy();
			bitboard attackedOnDiagonal = slider_attacks->GetBishopAttacks(kingSquare, occupancy) & diagonals;

			if (attackedOnDiagonal != 0)
			{
				return true;
			}

			bitboard straights = this->GetBlackQueens() | this->GetBlackRooks();
			bitboard attackedOnStraights = slider_attacks->GetRookAttacks(kingSquare, occupancy) & straights;
			
			if (attackedOnStraights != 0)
			{
				return true;
			}

			bitboard knights = this->GetBlackKnights();


			// mask for file wraps
			auto attackedByKnight = this->GetKnightPossible(kingSquare) & knights;

			if (attackedByKnight != 0)
			{
				return true;
			}

			return false;
		}

		if (color == BLACK)
		{
			bitboard king = this->GetBlackKings();
			square kingSquare = GetLSB(king);
			bitboard pawnMask = ((king & ~A_FILE) << SW)| ((king & ~H_FILE) << SE);
			bitboard diagonals = this->GetWhiteBishops() | this->GetWhiteQueens() | (this->GetWhitePawns() & pawnMask);
			bitboard occupancy = this->GetOccupancy();
			bitboard attackedOnDiagonal = slider_attacks->GetBishopAttacks(kingSquare, occupancy) & diagonals;

			if (attackedOnDiagonal != 0)
			{
				return true;
			}

			bitboard straights = this->GetWhiteQueens() | this->GetWhiteRooks();
			bitboard attackedOnStraights = slider_attacks->GetRookAttacks(kingSquare, occupancy) & straights;
			
			if (attackedOnStraights != 0)
			{
				return true;
			}

			bitboard knights = this->GetWhiteKnights();

			// mask for file wraps
			auto attackedByKnight = this->GetKnightPossible(kingSquare) & knights;

			if (attackedByKnight != 0)
			{
				return true;
			}

			return false;
		}
	}

	bitboard Board_Bit::GetKnightPossible(square center)
	{
			auto span = KNIGHT_SPAN;
			if (center >= KNIGHT_SPAN_CENTER)
			{
				span = span << (center - KNIGHT_SPAN_CENTER);
			}
			else
			{
				span = span >> (KNIGHT_SPAN_CENTER - center);
			}

			return span & knight_move_masks[center % 8];
	}

	void Board_Bit::ToggleBitBoards(square from, square to, piece p, color c)
	{
		bitboard fromToBB = (ONE_BIT << from) | (ONE_BIT << to);
		this->Colors->at(c) ^= fromToBB;
		this->Pieces->at(p) ^= fromToBB;
		this->pieceLookup->at(from) = NO_PIECE;
		this->pieceLookup->at(to) = p;
	}

	void Board_Bit::FillSquare(square s, piece p, color c)
	{
		bitboard bb = ONE_BIT << s;
		this->Colors->at(c) ^= bb;
		this->Pieces->at(p) ^= bb;
		this->pieceLookup->at(s) = p;
	}

	void Board_Bit::ClearSquare(square s,piece p, color c)
	{
		bitboard bb = ONE_BIT << s;
		this->Colors->at(c) ^= bb;
		this->Pieces->at(p) ^= bb;
		this->pieceLookup->at(s) = NO_PIECE;
	}

	MoveVector Board_Bit::GenerateMoves()
	{
		switch (this->side_to_move)
		{
			case WHITE:
				return GenerateWhiteMoves();
			case BLACK:
				return GenerateBlackMoves();
		}
		return std::make_shared<std::vector<Move> >();
	}

	MoveVector Board_Bit::GenerateWhiteMoves()
	{
		auto moves = std::make_shared<std::vector<Move> >();
		this->MakeWhitePawnMoves(moves);
		this->MakeWhiteRooksMoves(moves);
		this->MakeWhiteKnightMoves(moves);
		this->MakeWhiteBishopMoves(moves);
		this->MakeWhiteQueenMoves(moves);
		this->MakeWhiteKingMoves(moves);
		return moves;
	}

	MoveVector Board_Bit::GenerateBlackMoves()
	{
		auto moves = std::make_shared<std::vector<Move> >();
		this->MakeBlackPawnMoves(moves);
		this->MakeBlackRooksMoves(moves);
		this->MakeBlackKnightMoves(moves);
		this->MakeBlackBishopMoves(moves);
		this->MakeBlackQueenMoves(moves);
		this->MakeBlackKingMoves(moves);
		return moves;
	}

	int Board_Bit::GetMSB(bitboard b)
	{
#ifdef _MSC_VER
		unsigned long leading_zero = 0;

		if (_BitScanReverse64(&leading_zero, b))
		{
			return 63 - leading_zero;
		}

		return 63;
#else
		int leading_zeros = __builtin_clzll(b);
		return 63 - leading_zeros;
#endif
	}

	int Board_Bit::GetLSB(bitboard b)
	{
#ifdef _MSC_VER
		unsigned long trailing_zero = 0;

		if (_BitScanForward64(&trailing_zero, b))
		{
			return  trailing_zero;
		}

		return 0;
#else
		return __builtin_ctzll(b);
#endif
	}

	void Board_Bit::PrintBoard(bitboard b)
	{
		Panzer::Utils::PrintBoard(b);
	}

	void Board_Bit::MakeMove(const Move move)
	{
		// remove the from piece
		if (move.isCapture())
		{
			// clear the captured piece
			if (move.isEPCapture())
			{
				this->ClearSquare(this->ep_square, PAWN, !this->side_to_move);
			}
			else
			{
				this->ClearSquare(move.getTo(), this->GetPieceAtSquare(move.getTo()), !this->side_to_move);
			}
			
		}

		// fill the to square
		if (move.isPromo())
		{
			// get type from promotion type
			const auto flags = move.getFlags();
			if (flags & QUEEN_PROMO || flags & QUEEN_PROMO_CAPTURE)
			{
				this->FillSquare(move.getTo(), QUEEN, this->side_to_move);
			}
			else if (flags & KNIGHT_PROMO || flags & KNIGHT_PROMO_CAPTURE)
			{
				this->FillSquare(move.getTo(), KNIGHT, this->side_to_move);
			}
			else if (flags & ROOK_PROMO || flags & ROOK_PROMO_CAPTURE)
			{
				this->FillSquare(move.getTo(), ROOK, this->side_to_move);
			}
			else if (flags & BISHOP_PROMO || flags & BISHOP_PROMO_CAPTURE)
			{
				this->FillSquare(move.getTo(), BISHOP, this->side_to_move);
			}
			this->ClearSquare(move.getFrom(), PAWN, this->side_to_move);
		}
		else
		{
			this->ToggleBitBoards(move.getFrom(), move.getTo(), this->GetPieceAtSquare(move.getFrom()), this->side_to_move);
		}
		

		if (this->castle_flags != EMPTY_CASTLE_FLAGS)
		{
			// toggle off castle flags
			if (move.getFrom() == E1 || move.getTo() == E1) this->castle_flags ^= (WHITEK|WHITEQ);
			else if (move.getFrom() == A1 || move.getTo() == A1) this->castle_flags ^= (WHITEQ);
			else if (move.getFrom() == H1 || move.getTo() == H1) this->castle_flags ^= (WHITEK);
			else if (move.getFrom() == E8 || move.getTo() == E8) this->castle_flags ^= (BLACKK|BLACKQ);
			else if (move.getFrom() == A8 || move.getTo() == A8) this->castle_flags ^= (BLACKQ);
			else if (move.getFrom() == H8 || move.getTo() == H8) this->castle_flags ^= (BLACKK);
		}

		if (move.isCastle())
		{
			switch (move.getTo())
			{
				case C1:
					this->ToggleBitBoards(A1, D1, ROOK, WHITE);
					break;
				case G1:
					this->ToggleBitBoards(H1, F1, ROOK, WHITE);
					break;
				case C8:
					this->ToggleBitBoards(A8, D8, ROOK, WHITE);
					break;
				case G8:
					this->ToggleBitBoards(H8, F8, ROOK, WHITE);
					break;
			}
		}

		if (move.getFlags() == DOUBLE_PAWN_PUSH)
		{
			if (this->side_to_move == WHITE)
			{
				this->ep_square = move.getTo();
			}
			else
			{
				this->ep_square = move.getTo();
			}
		}

		this->side_to_move = !this->side_to_move;
		this->ply++;
	}

	void Board_Bit::UnmakeMove(const Move move)
	{
		// intentionally ordered backwards from make move for 
		// debugging purposes
		this->ply--;
		this->side_to_move = !this->side_to_move;
		this->ep_square = move.getPriorEPSquare();

		if (move.isCastle())
		{
			switch (move.getTo())
			{
				case C1:
					this->ToggleBitBoards(D1, A1, ROOK, WHITE);
					break;
				case G1:
					this->ToggleBitBoards(F1, H1, ROOK, WHITE);
					break;
				case C8:
					this->ToggleBitBoards(D8, A8, ROOK, BLACK);
					break;
				case G8:
					this->ToggleBitBoards(F8, H8, ROOK, BLACK);
					break;
			}
		}

		this->castle_flags = move.getCastleFlags();

		// clear the to square
		// fill the from square
		if (move.isPromo())
		{
			// get type from promotion type
			const auto flags = move.getFlags();
			if (flags & QUEEN_PROMO || flags & QUEEN_PROMO_CAPTURE)
			{
				this->ClearSquare(move.getTo(), QUEEN, this->side_to_move);
			}
			else if (flags & KNIGHT_PROMO || flags & KNIGHT_PROMO_CAPTURE)
			{
				this->ClearSquare(move.getTo(), KNIGHT, this->side_to_move);
			}
			else if (flags & ROOK_PROMO || flags & ROOK_PROMO_CAPTURE)
			{
				this->ClearSquare(move.getTo(), ROOK, this->side_to_move);
			}
			else if (flags & BISHOP_PROMO || flags & BISHOP_PROMO_CAPTURE)
			{
				this->ClearSquare(move.getTo(), BISHOP, this->side_to_move);
			}
			this->FillSquare(move.getFrom(), PAWN, this->side_to_move);

		}
		else
		{
			this->ToggleBitBoards(move.getTo(), move.getFrom(), this->GetPieceAtSquare(move.getTo()), this->side_to_move);
		}
		

		// put captured piece back
		if (move.isCapture())
		{
			if (move.isEPCapture())
			{
				this->FillSquare(this->ep_square, PAWN, !this->side_to_move);
			}
			else
			{
				this->FillSquare(move.getTo(), move.getCapturedPiece(), !this->side_to_move);
			}
		}
	}

	void Board_Bit::MakeWhitePawnMoves(MoveVector moves)
	{
		// generate pawn moves
		// first shift pawns up one
		const bitboard pawns = this->GetWhitePawns();
		const bitboard black_pieces = this->GetBlackPieces();
		const bitboard occupied = this->GetOccupancy();
		bitboard pushes = (pawns << N) & ~occupied; // shift north
		// scan the bits and make the move
		// char from = bit_index - 8;
		// char to = bit_index;
		// flag move_type = PAWN_PUSH;
		bitboard double_push = ((pushes & THIRD_RANK) << N) & ~occupied; // moves forward that cant be occupied
		// scan the bits and make the moves
		// char from = bit_index - 16;
		// char to = bit_index;
		// flag move_type = PAWN_DOUBLE_PUSH;
		bitboard left_captures = ((pawns & ~A_FILE) << NW) & black_pieces; // shift NW

		bitboard right_captures = ((pawns & ~H_FILE) << NE) & black_pieces; // shift NE

		bitboard ep_captures = (pawns & rank_EP_masks[this->ep_square]);

		while (ep_captures != 0)
		{
			square from = GetLSB(ep_captures);
			square to = this->ep_square + N;
			const auto move = Move
			(
				from,
				to,
				EP_CAPTURE,
				this->castle_flags,
				PAWN
			);
			moves->push_back(move);
			ep_captures &= ep_captures - 1ULL;
		}

		while(right_captures != 0)
		{
			int index = GetLSB(right_captures);
			square to = index;
			square from = to - SW;
			const auto move = Move
			(
				from,
				to,
				CAPTURE,
				this->castle_flags,
				this->GetPieceAtSquare(to)
			);
			moves->push_back(move);
			right_captures &= right_captures - 1ULL;
		}

		while(left_captures != 0)
		{
			int index = GetLSB(left_captures);
			square to = index;
			square from = to - SE;
			const auto move = Move
			(
				from,
				to,
				CAPTURE,
				this->castle_flags,
				this->GetPieceAtSquare(to)
			);
			moves->push_back(move);
			left_captures &= left_captures - 1ULL;	
		}


		while (pushes != 0)
		{
			int index = GetLSB(pushes);
			square to = index;
			square from = to - S;
			const auto move = Move
			(
				from, 
				to, 
				NO_MOVE_FLAGS,
				this->castle_flags
			);
			moves->push_back(move);
			pushes &= pushes-1ULL;
		}

		while (double_push != 0)
		{
			int index = GetLSB(double_push);
			square to = index;
			square from = to - SS;
			const auto move = Move
			(
				from,
				to,
				DOUBLE_PAWN_PUSH,
				this->castle_flags
			);
			moves->push_back(move);
			double_push &= double_push - 1ULL;
		}

	}

	void Board_Bit::MakeWhiteRooksMoves(MoveVector moves)
	{
		auto rooks = this->GetWhiteRooks();
		auto white_pieces = this->GetWhitePieces();
		auto black_pieces = this->GetBlackPieces();
		this->MakeRookMoves(moves, rooks, white_pieces, black_pieces);
	}

	void Board_Bit::MakeWhiteKnightMoves(MoveVector moves)
	{
		auto knights = this->GetWhiteKnights();
		auto white_pieces = this->GetWhitePieces();
		auto black_pieces = this->GetBlackPieces();
		this->MakeKnightMoves(moves, knights, white_pieces, black_pieces);
	}

	void Board_Bit::MakeWhiteBishopMoves(MoveVector moves)
	{
		auto bishops = this->GetWhiteBishops();
		auto white_pieces = this->GetWhitePieces();
		auto black_pieces = this->GetBlackPieces();
		this->MakeBishopMoves(moves, bishops, white_pieces, black_pieces);
	}

	void Board_Bit::MakeWhiteQueenMoves(MoveVector moves)
	{
		auto queens = this->GetWhiteQueens();
		auto white_pieces = this->GetWhitePieces();
		auto black_pieces = this->GetBlackPieces();
		this->MakeQueenMoves(moves, queens, white_pieces, black_pieces);
	}

	void Board_Bit::MakeWhiteKingMoves(MoveVector moves)
	{
		auto kings = this->GetWhiteKings();
		auto white_pieces = this->GetWhitePieces();
		auto black_pieces = this->GetBlackPieces();
		this->MakeKingMoves(moves, kings, white_pieces, black_pieces);
	}

	void Board_Bit::MakeBlackPawnMoves(MoveVector moves)
	{
		// generate pawn moves
		// first shift pawns up one
		const bitboard pawns = this->GetBlackPawns();
		const bitboard white_pieces = this->GetWhitePieces();
		const bitboard occupied = this->GetOccupancy();
		bitboard pushes = (pawns >> S) & ~occupied; // shift north
		// scan the bits and make the move
		// char from = bit_index - 8;
		// char to = bit_index;
		// flag move_type = PAWN_PUSH;
		bitboard double_push = ((pushes & SIXTH_RANK) >> S) & ~occupied; // moves forward that cant be occupied
		// scan the bits and make the moves
		// char from = bit_index - 16;
		// char to = bit_index;
		// flag move_type = PAWN_DOUBLE_PUSH;
		bitboard left_captures = ((pawns & ~A_FILE) >> SW) & white_pieces; // shift NW

		bitboard right_captures = ((pawns & ~H_FILE) >> SE) & white_pieces; // shift NE

		bitboard ep_captures = (pawns & rank_EP_masks[this->ep_square]);
		while (ep_captures != 0)
		{
			square from = GetLSB(ep_captures);
			square to = this->ep_square + S;
			const auto move = Move
			(
				from,
				to,
				EP_CAPTURE,
				this->castle_flags,
				PAWN
			);
			moves->push_back(move);
			ep_captures &= ep_captures - 1ULL;
		}

		while(right_captures != 0)
		{
			int index = GetLSB(right_captures);
			square to = index;
			square from = to + NW;
			auto move = Move
			(
				from,
				to,
				CAPTURE,
				this->castle_flags,
				this->GetPieceAtSquare(to)
			);
			moves->push_back(move);
			right_captures &= right_captures - 1ULL;
		}

		while(left_captures != 0)
		{
			int index = GetLSB(left_captures);
			square to = index;
			square from = to + NE;
			auto move = Move
			(
				from,
				to,
				CAPTURE,
				this->castle_flags,
				this->GetPieceAtSquare(to)
			);
			moves->push_back(move);
			left_captures &= left_captures - 1ULL;	
		}


		while (pushes != 0)
		{
			int index = GetLSB(pushes);
			square to = index;
			square from = to + N;
			const auto move = Move
			(
				from, 
				to, 
				NO_MOVE_FLAGS,
				this->castle_flags
			); 
			moves->push_back(move);
			pushes &= pushes-1ULL;
		}

		while (double_push != 0)
		{
			int index = GetLSB(double_push);
			square to = index;
			square from = to + NN;
			const auto move = Move
			(
				from,
				to,
				DOUBLE_PAWN_PUSH,
				this->castle_flags
			);
			moves->push_back(move);
			double_push &= double_push - 1ULL;
		}

	}

	void Board_Bit::MakeBlackRooksMoves(MoveVector moves)
	{
		auto rooks = this->GetBlackRooks();
		auto white_pieces = this->GetWhitePieces();
		auto black_pieces = this->GetBlackPieces();
		this->MakeRookMoves(moves, rooks, black_pieces, white_pieces);
	}

	void Board_Bit::MakeBlackKnightMoves(MoveVector moves)
	{
		auto knights = this->GetBlackKnights();
		auto white_pieces = this->GetWhitePieces();
		auto black_pieces = this->GetBlackPieces();
		this->MakeKnightMoves(moves, knights, black_pieces, white_pieces);
	}

	void Board_Bit::MakeBlackBishopMoves(MoveVector moves)
	{
		auto bishops = this->GetBlackBishops();
		auto white_pieces = this->GetWhitePieces();
		auto black_pieces = this->GetBlackPieces();
		this->MakeBishopMoves(moves, bishops, black_pieces, white_pieces);
	}

	void Board_Bit::MakeBlackQueenMoves(MoveVector moves)
	{
		auto queens = this->GetBlackQueens();
		auto white_pieces = this->GetWhitePieces();
		auto black_pieces = this->GetBlackPieces();
		this->MakeQueenMoves(moves, queens, black_pieces, white_pieces);
	}

	void Board_Bit::MakeBlackKingMoves(MoveVector moves)
	{
		auto kings = this->GetBlackKings();
		auto white_pieces = this->GetWhitePieces();
		auto black_pieces = this->GetBlackPieces();
		this->MakeKingMoves(moves, kings, black_pieces, white_pieces);
	}

	void Board_Bit::MakeRookMoves(MoveVector moves, bitboard rooks, bitboard same_side, bitboard other_side)
	{
		auto occupancy = same_side | other_side;
		while (rooks != 0)
		{
			square from = GetLSB(rooks);
			auto possible = this->slider_attacks->GetRookAttacks(from, occupancy);
			auto captures = possible & other_side;

			while (captures != 0)
			{
				square to = GetLSB(captures);
				const auto move = Move(
					from,
					to,
					CAPTURE,
					this->castle_flags,
					this->GetPieceAtSquare(to)
				);
				moves->push_back(move);
				captures &= captures - 1;
			}

			auto slides = possible & ~occupancy;
			while (slides != 0)
			{
				square to = GetLSB(slides);
				const auto move = Move(
					from,
					to,
					NO_MOVE_FLAGS,
					this->castle_flags
				);
				moves->push_back(move);
				slides &= slides - 1ULL;
			}

			rooks &= rooks - 1ULL;
		}
	}

	void Board_Bit::MakeKnightMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side)
	{

		while (knights != 0)
		{
			auto from = this->GetLSB(knights);
			auto span = KNIGHT_SPAN;
			if (from >= KNIGHT_SPAN_CENTER)
			{
				span = span << (from - KNIGHT_SPAN_CENTER);
			}
			else
			{
				span = span >> (KNIGHT_SPAN_CENTER - from);
			}

			// mask for file wraps
			auto all_moves = span & ~same_side & knight_move_masks[from % 8];
			auto captures = all_moves & other_side;
			auto regular = all_moves & ~captures;

			while (captures != 0)
			{
				int to = this->GetLSB(captures);
				const auto move = Move
				(
					from,
					to,
					CAPTURE,
					this->castle_flags,
					this->GetPieceAtSquare(to)
				);
				moves->push_back(move);
				captures &= captures - 1ULL;
			}

			while (regular != 0)
			{
				int to = this->GetLSB(regular);
				const auto move = Move
				(
					from,
					to,
					NO_MOVE_FLAGS,
					this->castle_flags
				);

				moves->push_back(move);
				regular &= regular - 1ULL;
			}
			knights &= knights - 1ULL;
		}
	}

	void Board_Bit::MakeBishopMoves(MoveVector moves, bitboard bishops, bitboard same_side, bitboard other_side)
	{
		auto occupancy = same_side | other_side;
		while (bishops != 0)
		{
			square from = GetLSB(bishops);
			auto possible = this->slider_attacks->GetBishopAttacks(from, occupancy);
			auto captures = possible & other_side;

			while (captures != 0)
			{
				square to = GetLSB(captures);
				const auto move = Move(
					from,
					to,
					CAPTURE,
					this->castle_flags,
					this->GetPieceAtSquare(to)
				);
				moves->push_back(move);
				captures &= captures - 1;
			}

			auto slides = possible & ~occupancy;
			while (slides != 0)
			{
				square to = GetLSB(slides);
				const auto move = Move(
					from,
					to,
					NO_MOVE_FLAGS,
					this->castle_flags
				);
				moves->push_back(move);
				slides &= slides - 1ULL;
			}

			bishops &= bishops - 1ULL;
		}
	}

	void Board_Bit::MakeQueenMoves(MoveVector moves, bitboard queens, bitboard same_side, bitboard other_side)
	{
		auto occupancy = same_side | other_side;
		while (queens != 0)
		{
			square from = GetLSB(queens);
			auto possible = this->slider_attacks->GetQueenAttacks(from, occupancy) & ~same_side;
			auto captures = possible & other_side;

			while (captures != 0)
			{
				square to = GetLSB(captures);
				const auto move = Move(
					from,
					to,
					CAPTURE,
					this->castle_flags,
					this->GetPieceAtSquare(to)
				);
				moves->push_back(move);
				captures &= captures - 1ULL;
			}

			auto slides = possible & ~occupancy;
			while (slides != 0)
			{
				square to = GetLSB(slides);
				const auto move = Move(
					from,
					to,
					NO_MOVE_FLAGS,
					this->castle_flags
				);
				moves->push_back(move);
				slides &= slides - 1ULL;
			}
			queens &= queens - 1ULL;
		}
	}

	void Board_Bit::MakeKingMoves(MoveVector moves, bitboard kings, bitboard same_side, bitboard other_side)
	{
		while (kings != 0)
		{
			int from = this->GetLSB(kings);
			auto king_span = KING_SPAN;

			if (from >= KING_SPAN_CENTER)
			{
				king_span = king_span << (from - KING_SPAN_CENTER);
			}
			else
			{
				king_span = king_span >> (KING_SPAN_CENTER - from);
			}

			auto all_moves = king_span & ~same_side;
			auto captures = all_moves & other_side;
			auto regular_moves = all_moves &~captures;

			while (captures != 0)
			{
				int to = this->GetLSB(captures);
				const auto move = Move
				(
					from,
					to,
					CAPTURE,
					this->castle_flags,
					this->GetPieceAtSquare(to)
				);

				moves->push_back(move);
				captures &= captures - ONE_BIT;
			}

			while (regular_moves != 0)
			{
				int to = this->GetLSB(regular_moves);
				const auto move = Move
				(
					from,
					to,
					NO_MOVE_FLAGS,
					this->castle_flags
				);

				moves->push_back(move);
				regular_moves &= regular_moves - ONE_BIT;
			}

			kings &= kings - ONE_BIT;
		}
	}

std::string Board_Bit::BoardToFen()
{
	std::string fen = "";
	int empty_squares = 0;
	for (int index = 0; index < 64; index++) 
	{
		square s = fenIndexToSquare[index];
		bitboard targetSquare = ONE_BIT << s;
		piece piece = this->GetPieceAtSquare(s);
		// if there is no piece at this index then increase empty square count
		if (piece == NO_PIECE)
		{
			empty_squares++;
		}
		else
		{
			color c = (this->GetWhitePieces() & targetSquare) != 0;
			if (empty_squares != 0)
			{
				fen += std::to_string(empty_squares);
				empty_squares = 0;
			}
			switch (piece) 
			{
				case PAWN:
					fen += c == BLACK ? "p" : "P";
					break;
				case ROOK:
					fen += c == BLACK ? "r" : "R";
					break;
				case KNIGHT:
					fen += c == BLACK ? "n" : "N";
					break;
				case BISHOP:
					fen += c == BLACK ? "b" : "B";
					break;
				case QUEEN:
					fen += c == BLACK ? "q" : "Q";
					break;
				case KING:
					fen += c == BLACK ? "k" : "K";
					break;
			}
		}

		if ((s & 7) == 7)
		{
			if (empty_squares != 0)
			{
				fen += std::to_string(empty_squares);
				empty_squares = 0;
			}
			if (s != H1)
			{
				fen += "/";
			}
		}
	}

	fen += " ";
	fen += this->side_to_move == WHITE ? "w" : "b";

	fen += " ";
	if (this->castle_flags == EMPTY_CASTLE_FLAGS)
	{
		fen += "-";
	}
	else
	{
		if ((this->castle_flags & WHITEK) != 0)
		{
			fen += "K";
		}

		if ((this->castle_flags & WHITEQ) != 0)
		{
			fen += "Q";
		}

		if ((this->castle_flags & BLACKK) != 0)
		{
			fen += "k";
		}

		if ((this->castle_flags & BLACKQ) != 0)
		{
			fen += "q";
		}
	}

	fen += " ";

	if (this->ep_square != NO_SQUARE)
	{
		fen += squareToString[this->ep_square];
	}
	else
	{
		fen += "-";
	}

	// TODO: implement move clocks
	return fen;
}

	void Board_Bit::FenToBoard(const std::string& fen)
	{
		bool board_done = false;
		this->pieceLookup = new std::array<piece, 64> { NO_PIECE };
		this->Pieces = new std::array<bitboard, 7> { 0ULL };
		this->Colors = new std::array<bitboard, 2> { 0ULL };

		int index = 0;
		for (char const& c : fen)
		{
			auto s = fenIndexToSquare[index];
			if (board_done)
			{
				// first space is side
				// second spacce is castle flags
				// third space is ep
				// last two digits are moves and plys
				switch (c)
				{
					case 'b':
						this->side_to_move = BLACK;
						std::cout << "set side to move to black" << std::endl;
						break;
					case 'w':
						this->side_to_move = WHITE;
						std::cout << "set side to move to white" << std::endl;
						break;
					case 'k':
						this->castle_flags |= BLACKK;
						break;
					case 'q':
						this->castle_flags |= BLACKQ;
						break;
					case 'K':
						this->castle_flags |= WHITEK;
						break;
					case 'Q':
						this->castle_flags |= WHITEQ;
					case '\0':
						return;
				}
			}
			else 
			{
				switch (c) {
				case 'r':
					this->FillSquare(s, ROOK, BLACK);
					index++;
					break;
				case 'n':
					this->FillSquare(s, KNIGHT, BLACK);
					index++;
					break;
				case 'b':
					this->FillSquare(s, BISHOP, BLACK);
					index++;
					break;
				case 'q':
					this->FillSquare(s, QUEEN, BLACK);
					index++;
					break;
				case 'k':
					this->FillSquare(s, KING, BLACK);
					index++;
					break;
				case 'p':
					this->FillSquare(s, PAWN, BLACK);
					index++;
					break;
				case 'R':
					this->FillSquare(s, ROOK, WHITE);
					index++;
					break;
				case 'N':
					this->FillSquare(s, KNIGHT, WHITE);
					index++;
					break;
				case 'B':
					this->FillSquare(s, BISHOP, WHITE);
					index++;
					break;
				case 'Q':
					this->FillSquare(s, QUEEN, WHITE);
					index++;
					break;
				case 'K':
					this->FillSquare(s, KING, WHITE);
					index++;
					break;
				case 'P':
					this->FillSquare(s, PAWN, WHITE);
					index++;
					break;
				case '/':
					break;
				case '1':
					index += 1;
					break;
				case '2':
					index += 2;
					break;
				case '3':
					index += 3;
					break;
				case '4':
					index += 4;
					break;
				case '5':
					index += 5;
					break;
				case '6':
					index += 6;
					break;
				case '7':
					index += 7;
					break;
				case '8':
					index += 8;
					break;
				case ' ':
					board_done = true;
					break;
				}
			}
		}
	}
}
