#include <stdlib.h>
#include <array>
#include <memory>
#include <vector>
#include <iostream>
#include "bitboard_constants.h"
#include "move.h"

typedef std::shared_ptr<std::vector<std::shared_ptr<const Panzer::Move>>> MoveVector;
namespace Panzer
{
	class Board_Bit
	{
		bitboard OCCUPIED = 0;
		std::array<bitboard, 7> *Pieces = new std::array<bitboard, 7> { 0ULL };
		std::array<bitboard, 2> *Colors = new std::array<bitboard, 2> { 0ULL };

		char side_to_move = WHITE;
		square ep_square = NO_SQUARE;
		castle_flag castle_flags = (WHITEK|WHITEQ|BLACKK|BLACKQ);
		uint8_t ply = 1;
		std::array<piece, 64> *pieces = new std::array<piece, 64> {NO_PIECE};

	public:
		void FillSquare(square s, piece p, color c);
		void ClearSquare(square s,piece p, color c);
		void PrintBoard(bitboard b);

		MoveVector GenerateMoves();
		MoveVector GenerateWhiteMoves();
		MoveVector GenerateBlackMoves();

		bitboard GetWhitePieces() { return Colors->at(WHITE); };
		bitboard GetWhitePawns() { return Pieces->at(PAWN) & Colors->at(WHITE); };
		bitboard GetWhiteRooks() { return Pieces->at(ROOK) & Colors->at(WHITE); };
		bitboard GetWhiteKnights() { return Pieces->at(KNIGHT) & Colors->at(WHITE); };
		bitboard GetWhiteBishops() { return Pieces->at(BISHOP) & Colors->at(WHITE); };
		bitboard GetWhiteQueens() { return Pieces->at(QUEEN) & Colors->at(WHITE); };
		bitboard GetWhiteKings() { return Pieces->at(KING) & Colors->at(WHITE); };

		bitboard GetBlackPieces() { return Colors->at(BLACK); };
		bitboard GetBlackPawns() { return Pieces->at(PAWN) & Colors->at(BLACK); };
		bitboard GetBlackRooks() { return Pieces->at(ROOK) & Colors->at(BLACK); };
		bitboard GetBlackKnights() { return Pieces->at(KNIGHT) & Colors->at(BLACK); };
		bitboard GetBlackBishops() { return Pieces->at(BISHOP) & Colors->at(BLACK); };
		bitboard GetBlackQueens() { return Pieces->at(QUEEN) & Colors->at(BLACK); };
		bitboard GetBlackKings() { return Pieces->at(KING) & Colors->at(BLACK); };

		bitboard GetOccupancy() { return Colors->at(WHITE) | Colors->at(BLACK); }

		void MakeMove(std::shared_ptr<const Move> move);
		void UnmakeMove(std::shared_ptr<const Move> move);
		void ToggleBitBoards(square from, square to, piece p, color c);
	private:
		int GetMSB(bitboard b);
		int GetLSB(bitboard b);
		piece GetPieceAtSquare(square s);
		void MakeWhitePawnMoves(MoveVector moves);
		void MakeWhiteRooksMoves(MoveVector moves);
		void MakeWhiteKnightMoves(MoveVector moves);
		void MakeWhiteBishopMoves(MoveVector moves);
		void MakeWhiteQueenMoves(MoveVector moves);
		void MakeWhiteKingMoves(MoveVector moves);
		void MakeBlackPawnMoves(MoveVector moves);
		void MakeBlackRooksMoves(MoveVector moves);
		void MakeBlackKnightMoves(MoveVector moves);
		void MakeBlackBishopMoves(MoveVector moves);
		void MakeBlackQueenMoves(MoveVector moves);
		void MakeBlackKingMoves(MoveVector moves);

		void MakeRookMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side);
		void MakeKnightMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side);
		void MakeBishopMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side);
		void MakeQueenMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side);
		void MakeKingMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side);
	};

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
		ToggleBitBoards(NO_SQUARE, s, p, c);
	}

	void Board_Bit::ClearSquare(square s,piece p, color c)
	{
		ToggleBitBoards(s, NO_SQUARE, p, c);
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
		for (int row = 7; row >= 0; --row) 
		{
			for (int col = 0; col <= 7; ++col) 
			{
				if (b & (1ULL << ((row * 8) + col))) 
				{
					std::cout << "1 ";
				} else 
				{
					std::cout << "0 ";
				}
			}

			std::cout << std::endl;
		}
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
		bitboard pushes = pawns << N; // shift north
		pushes = pushes & ~occupied; // space cant be occupied
		// scan the bits and make the move
		// char from = bit_index - 8;
		// char to = bit_index;
		// flag move_type = PAWN_PUSH;
		bitboard double_push = ((pushes & THIRD_RANK) << N) & ~occupied; // moves forward that cant be occupied
		// scan the bits and make the moves
		// char from = bit_index - 16;
		// char to = bit_index;
		// flag move_type = PAWN_DOUBLE_PUSH;
		bitboard left_captures = (pawns & ~A_FILE) << NW; // shift NW
		left_captures = left_captures & black_pieces;

		bitboard right_captures = (pawns & ~H_FILE) << NE; // shift NE
		right_captures = right_captures & black_pieces;

		while (pushes != 0)
		{
			int index = GetLSB(pushes);
			square to = index;
			square from = to - S;
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
			square from = to - SS;
			const auto move = std::make_shared<Move>
				(from,
				to,
				PAWN,
				NO_PIECE,
				NO_PIECE,
				to,
				this->ep_square,
				EMPTY_MOVE_FLAGS,
				this->castle_flags,
				this->ply);
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
				PAWN,
				NO_PIECE,
				this->GetPieceAtSquare(index),
				NO_SQUARE,
				this->ep_square,
				CAPTURE,
				this->castle_flags,
				this->ply);
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
				PAWN,
				NO_PIECE,
				this->GetPieceAtSquare(index),
				NO_SQUARE,
				this->ep_square,
				CAPTURE,
				this->castle_flags,
				this->ply);
			moves->push_back(move);
			left_captures &= left_captures - 1ULL;	
		}

	}

	void Board_Bit::MakeWhiteRooksMoves(MoveVector moves)
	{

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

	}

	void Board_Bit::MakeWhiteQueenMoves(MoveVector moves)
	{

	}

	void Board_Bit::MakeWhiteKingMoves(MoveVector moves)
	{

	}

	void Board_Bit::MakeBlackPawnMoves(MoveVector moves)
	{

	}

	void Board_Bit::MakeBlackRooksMoves(MoveVector moves)
	{

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

	}

	void Board_Bit::MakeBlackQueenMoves(MoveVector moves)
	{

	}

	void Board_Bit::MakeBlackKingMoves(MoveVector moves)
	{

	}

	void Board_Bit::MakeRookMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side)
	{

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
					KNIGHT,
					NO_PIECE,
					this->GetPieceAtSquare(to),
					NO_SQUARE,
					this->ep_square,
					EMPTY_MOVE_FLAGS,
					this->castle_flags,
					this->ply
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
					KNIGHT,
					NO_PIECE,
					NO_PIECE,
					NO_SQUARE,
					this->ep_square,
					EMPTY_MOVE_FLAGS,
					this->castle_flags,
					this->ply	
				);

				moves->push_back(move);
				regular &= regular - 1ULL;
			}
			knights &= knights - 1ULL;
		}
	}

	void Board_Bit::MakeBishopMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side)
	{

	}

	void Board_Bit::MakeQueenMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side)
	{

	}

	void Board_Bit::MakeKingMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side)
	{

	}

}
