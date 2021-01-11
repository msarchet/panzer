#include "bitboard.h"
#include "bitboard_constants.h"

namespace Panzer
{
	piece Board_Bit::GetPieceAtSquare(square s)
	{
		return pieces->at(s);
	}

	void Board_Bit::ToggleBitBoards(square from, square to, piece p, color c)
	{
		bitboard fromToBB = ONE_BIT << from | ONE_BIT << to;
		this->Colors->at(c) ^= fromToBB;
		this->Pieces->at(p) ^= fromToBB;
		this->pieces->at(from) = NO_PIECE;
		this->pieces->at(to) = p;
	}

	void Board_Bit::FillSquare(square s, piece p, color c)
	{
		bitboard bb = ONE_BIT << s;
		this->Colors->at(c) ^= bb;
		this->Pieces->at(p) ^= bb;
	}

	void Board_Bit::ClearSquare(square s,piece p, color c)
	{
		bitboard bb = ONE_BIT << s;
		this->Colors->at(c) ^= bb;
		this->Pieces->at(p) ^= bb;
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
		return std::make_shared<std::vector<std::shared_ptr<const Move>>>();
	}

	MoveVector Board_Bit::GenerateWhiteMoves()
	{
		auto moves = std::make_shared<std::vector<std::shared_ptr<const Move>>>();
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
		auto moves = std::make_shared<std::vector<std::shared_ptr<const Move>>>();
		this->MakeBlackPawnMoves(moves);
		std::cout << "Black Pawwn Moves" << moves->size() << "\n";
		this->MakeBlackRooksMoves(moves);
		this->MakeBlackKnightMoves(moves);
		std::cout << "Black Knight Moves" << moves->size() << "\n";
		this->MakeBlackBishopMoves(moves);
		this->MakeBlackQueenMoves(moves);
		this->MakeBlackKingMoves(moves);
		return moves;
	}

	int Board_Bit::GetMSB(bitboard b)
	{
		int leading_zeros = __builtin_clzll(b);
		return 63 - leading_zeros;
	}

	int Board_Bit::GetLSB(bitboard b)
	{
		return __builtin_ctzll(b);
	}

	void Board_Bit::PrintBoard(bitboard b)
	{
		Panzer::Utils::PrintBoard(b);
	}

	void Board_Bit::MakeMove(std::shared_ptr<const Move> move)
	{
		// remove the from piece
		if (move->is_capture)
		{
			// clear the captured piece
			if (move->move_flags & EP)
			{
				this->ClearSquare(this->ep_square, move->captured_piece, !this->side_to_move);
			}
			else
			{
				this->ClearSquare(move->to, move->captured_piece, !this->side_to_move);
			}
			
		}

		// fill the to square
		if (move->move_flags & PROMOTION)
		{
			this->FillSquare(move->to, move->piece_to, this->side_to_move);
			this->ClearSquare(move->from, move->piece_from, this->side_to_move);
		}
		else
		{
			this->ToggleBitBoards(move->from, move->to, move->piece_from, this->side_to_move);
		}
		

		if (this->castle_flags != NO_FLAGS)
		{
			// toggle off castle flags
			if (move->from == E1 || move->to == E1) this->castle_flags ^= (WHITEK|WHITEQ);
			else if (move->from == A1 || move->to == A1) this->castle_flags ^= (WHITEQ);
			else if (move->from == H1 || move->to == H1) this->castle_flags ^= (WHITEK);
			else if (move->from == E8 || move->to == E8) this->castle_flags ^= (BLACKK|BLACKQ);
			else if (move->from == A8 || move->to == A8) this->castle_flags ^= (BLACKQ);
			else if (move->from == H8 || move->to == H8) this->castle_flags ^= (BLACKK);
		}

		if (move->is_castle)
		{
			switch (move->to)
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

		this->ep_square = move->ep;
		this->side_to_move = !this->side_to_move;
		this->ply++;
	}

	void Board_Bit::UnmakeMove(std::shared_ptr<const Move> move)
	{
		// intentionally ordered backwards from make move for 
		// debugging purposes
		this->ply--;
		this->side_to_move = !this->side_to_move;
		this->ep_square = move->prior_ep;

		if (move->is_castle)
		{
			switch (move->to)
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

		this->castle_flags = move->prior_castle_flags;

		// clear the to square
		// fill the from square
		if (move->move_flags & PROMOTION)
		{
			this->FillSquare(move->from, move->piece_from, this->side_to_move);
			this->ClearSquare(move->to, move->piece_to, this->side_to_move);
		}
		else
		{
			this->ToggleBitBoards(move->from, move->to, move->piece_from, this->side_to_move);
		}
		

		// put captured piece back
		if (move->is_capture)
		{
			if (move->move_flags & EP)
			{
				this->FillSquare(this->ep_square, move->captured_piece, !this->side_to_move);
			}
			else
			{
				this->FillSquare(move->to, move->captured_piece, !this->side_to_move);
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
			const auto move = std::make_shared<Move>
			(
				from,
				to,
				EP_CAPTURE
			);
			moves->push_back(move);
			ep_captures &= ep_captures - 1;
		}

		while (pushes != 0)
		{
			int index = GetLSB(pushes);
			square to = index;
			square from = to - S;
			const auto move = std::make_shared<Move>
			(
				from, 
				to, 
				NO_MOVE_FLAGS
			);
			moves->push_back(move);
			pushes &= pushes-1ULL;
		}

		while (double_push != 0)
		{
			int index = GetLSB(double_push);
			square to = index;
			square from = to - SS;
			const auto move = std::make_shared<Move>
			(
				from,
				to,
				DOUBLE_PAWN_PUSH
			);
			moves->push_back(move);
			double_push &= double_push - 1ULL;
		}

		while(right_captures != 0)
		{
			int index = GetLSB(right_captures);
			square to = index;
			square from = to - SW;
			auto move = std::make_shared<Move>
			(
				from,
				to,
				CAPTURE
			);
			moves->push_back(move);
			right_captures &= to;
		}

		while(left_captures != 0)
		{
			int index = GetLSB(left_captures);
			square to = index;
			square from = to - SE;
			auto move = std::make_shared<Move>
			(
				from,
				to,
				CAPTURE
			);
			moves->push_back(move);
			left_captures &= left_captures - 1ULL;	
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
		auto queens = this->GetWhiteBishops();
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
			const auto move = std::make_shared<Move>
			(
				from,
				to,
				EP_CAPTURE
			);
			moves->push_back(move);
			ep_captures &= ep_captures - 1;
		}

		while (pushes != 0)
		{
			int index = GetLSB(pushes);
			square to = index;
			square from = to + N;
			const auto move = std::make_shared<Move>
				(from, 
				to, 
				PAWN, 
				NO_PIECE, 
				NO_PIECE , 
				NO_SQUARE, 
				this->ep_square,
				EMPTY_MOVE_FLAGS, 
				this->castle_flags, 
				this->ply); 
			moves->push_back(move);
			pushes &= pushes-1ULL;
		}

		while (double_push != 0)
		{
			int index = GetLSB(double_push);
			square to = index;
			square from = to + NN;
			const auto move = std::make_shared<Move>
			(
				from,
				to,
				DOUBLE_PAWN_PUSH
			);
			moves->push_back(move);
			double_push &= double_push - 1ULL;
		}

		while(right_captures != 0)
		{
			int index = GetLSB(right_captures);
			square to = index;
			square from = to + NW;
			auto move = std::make_shared<Move>
			(
				from,
				to,
				CAPTURE
			);
			moves->push_back(move);
			right_captures &= to;
		}

		while(left_captures != 0)
		{
			int index = GetLSB(left_captures);
			square to = index;
			square from = to + NE;
			auto move = std::make_shared<Move>
			(
				from,
				to,
				CAPTURE
			);
			moves->push_back(move);
			left_captures &= left_captures - 1ULL;	
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
				const auto move = std::make_shared<Move>(
					from,
					to,
					CAPTURE
				);
				moves->push_back(move);
				captures &= captures - 1;
			}

			auto slides = possible & ~captures & ~same_side;
			while (slides != 0)
			{
				square to = GetLSB(slides);
				const auto move = std::make_shared<Move>(
					from,
					to,
					NO_MOVE_FLAGS
				);
				moves->push_back(move);
				slides &= slides - 1;
			}

			rooks &= rooks - 1;
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
				const auto move = std::make_shared<Move>
				(
					from,
					to,
					CAPTURE
				);
				moves->push_back(move);
				captures &= captures - 1ULL;
			}

			while (regular != 0)
			{
				int to = this->GetLSB(regular);
				const auto move = std::make_shared<Move>
				(
					from,
					to,
					NO_MOVE_FLAGS
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
				const auto move = std::make_shared<Move>(
					from,
					to,
					CAPTURE
				);
				moves->push_back(move);
				captures &= captures - 1;
			}

			auto slides = possible & ~captures & ~same_side;
			while (slides != 0)
			{
				square to = GetLSB(slides);
				const auto move = std::make_shared<Move>(
					from,
					to,
					BISHOP,
					NO_PIECE,
					NO_PIECE,
					NO_SQUARE,
					this->ep_square,
					EMPTY_MOVE_FLAGS,
					this->castle_flags,
					this->ply
				);
				moves->push_back(move);
				slides &= slides - 1;
			}

			bishops &= bishops - 1;
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
				const auto move = std::make_shared<Move>(
					from,
					to,
					QUEEN,
					NO_PIECE,
					this->GetPieceAtSquare(to),
					NO_SQUARE,
					this->ep_square,
					CAPTURE,
					this->castle_flags,
					this->ply
				);
				moves->push_back(move);
				captures &= captures - 1;
			}

			auto slides = possible & ~captures & ~same_side;
			while (slides != 0)
			{
				square to = GetLSB(slides);
				const auto move = std::make_shared<Move>(
					from,
					to,
					QUEEN,
					NO_PIECE,
					NO_PIECE,
					NO_SQUARE,
					this->ep_square,
					EMPTY_MOVE_FLAGS,
					this->castle_flags,
					this->ply
				);
				moves->push_back(move);
				slides &= slides - 1;
			}
			queens &= queens - 1;
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
				const auto move = std::make_shared<Move>
				(
					from,
					to,
					KING,
					NO_PIECE,
					this->GetPieceAtSquare(to),
					NO_SQUARE,
					this->ep_square,
					CAPTURE,
					this->castle_flags,
					this->ply
				);

				moves->push_back(move);
				captures &= captures - ONE_BIT;
			}

			while (regular_moves != 0)
			{
				int to = this->GetLSB(regular_moves);
				const auto move = std::make_shared<Move>
				(
					from,
					to,
					KING,
					NO_PIECE,
					NO_PIECE,
					NO_SQUARE,
					this->ep_square,
					EMPTY_MOVE_FLAGS,
					this->castle_flags,
					this->ply
				);

				moves->push_back(move);
				regular_moves &= regular_moves - ONE_BIT;
			}

			kings &= kings - ONE_BIT;
		}
	}

}
