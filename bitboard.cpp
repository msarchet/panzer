#include <stdlib.h>
#include <array>
#include <memory>
#include <vector>
#include <iostream>
#include "bitboard_constants.h"
#include "move.h"

namespace Panzer
{
	class Board_Bit
	{
		bitboard WHITE_PIECES = 0;
		bitboard WHITE_PAWNS = 0;
		bitboard WHITE_KINGS = 0;
		bitboard WHITE_ROOKS = 0;
		bitboard WHITE_BISHOPS = 0;
		bitboard WHITE_KNIGHTS = 0;
		bitboard WHITE_QUEENS = 0;
		bitboard BLACK_PIECES = 0;
		bitboard BLACK_PAWNS = 0;
		bitboard BLACK_KINGS = 0;
		bitboard BLACK_ROOKS = 0;
		bitboard BLACK_BISHOPS = 0;
		bitboard BLACK_KNIGHTS = 0;
		bitboard BLACK_QUEENS = 0;
		bitboard OCCUPIED = 0;

		char side_to_move = WHITE;
		square ep_square = NO_SQUARE;
		castle_flag castle_flags = (WHITEK|WHITEQ|BLACKK|BLACKQ);
		uint8_t ply = 1;
	public:
		void FillSquare(square s, piece p, color c);
		void ClearSquare(square s,piece p, color c);
		std::shared_ptr<std::vector<std::shared_ptr<Move>>> GenerateMoves();
		std::shared_ptr<std::vector<std::shared_ptr<Move>>> GenerateWhiteMoves();
		std::shared_ptr<std::vector<std::shared_ptr<Move>>> GenerateBlackMoves();
		void PrintBoard(bitboard b);

		bitboard GetWhitePieces() { return WHITE_PIECES; };
		bitboard GetWhitePawns() { return WHITE_PAWNS; };
		bitboard GetWhiteRooks() { return WHITE_ROOKS; };
		bitboard GetWhiteKnights() { return WHITE_KNIGHTS; };
		bitboard GetWhiteBishops() { return WHITE_BISHOPS; };
		bitboard GetWhiteQueens() { return WHITE_QUEENS; };
		bitboard GetWhiteKings() { return WHITE_KINGS; };

		bitboard GetBlackPieces() { return BLACK_PIECES; };
		bitboard GetBlackPawns() { return BLACK_PAWNS; };
		bitboard GetBlackRooks() { return BLACK_ROOKS; };
		bitboard GetBlackKnights() { return BLACK_KNIGHTS; };
		bitboard GetBlackBishops() { return BLACK_BISHOPS; };
		bitboard GetBlackQueens() { return BLACK_QUEENS; };
		bitboard GetBlackKings() { return BLACK_KINGS; };

		bitboard GetOccupancy() { return OCCUPIED; }
	private:
		int GetMSB(bitboard b);
		int GetLSB(bitboard b);
		piece GetPieceAtSquare(bitboard b, color c);
	};

	piece Board_Bit::GetPieceAtSquare(bitboard b, color c)
	{
		piece piece = NO_PIECE;

		if (c & WHITE)
		{
			piece |= !!(WHITE_PAWNS & b) << 1;
			piece |= !!(WHITE_ROOKS & b) << 2;
			piece |= !!(WHITE_KNIGHTS & b) << 3;
			piece |= !!(WHITE_BISHOPS & b) << 4;
			piece |= !!(WHITE_QUEENS & b) << 5;
			piece |= !!(WHITE_KINGS & b) << 6;
		}
		else
		{
			piece |= !!(BLACK_PAWNS & b) << 1;
			piece |= !!(BLACK_ROOKS & b) << 2;
			piece |= !!(BLACK_KNIGHTS & b) << 3;
			piece |= !!(BLACK_BISHOPS & b) << 4;
			piece |= !!(BLACK_QUEENS & b) << 5;
			piece |= !!(BLACK_KINGS & b) << 6;
		}

		return  piece;
	}

	void Board_Bit::FillSquare(square s, piece p, color c)
	{
		bitboard shifted = ONE_BIT << s;
		OCCUPIED ^= shifted;

		WHITE_PIECES ^= static_cast<uint64_t>(!c & !WHITE) << s;
		BLACK_PIECES ^= static_cast<uint64_t>(c & BLACK) << s;

		// no branching

		WHITE_PAWNS   ^= static_cast<uint64_t>((!c & !WHITE) && (p & PAWN)) << s;
		WHITE_ROOKS   ^= static_cast<uint64_t>((!c & !WHITE)&&(p & ROOK)) << s;
		WHITE_KNIGHTS ^= static_cast<uint64_t>((!c & !WHITE)&&(p & KNIGHT)) << s;
		WHITE_BISHOPS ^= static_cast<uint64_t>((!c & !WHITE)&&(p & BISHOP)) << s;
		WHITE_QUEENS  ^= static_cast<uint64_t>((!c & !WHITE)&&(p & QUEEN)) << s;
		WHITE_KINGS   ^= static_cast<uint64_t>((!c & !WHITE)&&(p & KING)) << s;

		BLACK_PAWNS   ^= static_cast<uint64_t>((c & BLACK)&&(p & PAWN)) << s;
		BLACK_ROOKS   ^= static_cast<uint64_t>((c & BLACK)&&(p & ROOK)) << s;
		BLACK_KNIGHTS ^= static_cast<uint64_t>((c & BLACK)&&(p & KNIGHT)) << s;
		BLACK_BISHOPS ^= static_cast<uint64_t>((c & BLACK)&&(p & BISHOP)) << s;
		BLACK_QUEENS  ^= static_cast<uint64_t>((c & BLACK)&&(p & QUEEN)) << s;
		BLACK_KINGS   ^= static_cast<uint64_t>((c & BLACK)&&(p & KING)) << s;
	}

	void Board_Bit::ClearSquare(square s,piece p, color c)
	{
		FillSquare(s, p, c);
	}

	std::shared_ptr<std::vector<std::shared_ptr<Move>>> Board_Bit::GenerateMoves()
	{
		switch (this->side_to_move)
		{
			case WHITE:
				return GenerateWhiteMoves();
			case BLACK:
				return GenerateBlackMoves();
		}
		return std::make_shared<std::vector<std::shared_ptr<Move>>>();
	}

	std::shared_ptr< std::vector<std::shared_ptr<Move>>> Board_Bit::GenerateWhiteMoves()
	{
		auto moves = std::make_shared<std::vector<std::shared_ptr<Move>>>();

		// generate pawn moves
		// first shift pawns up one
		bitboard pawns = WHITE_PAWNS;
		bitboard pushes = pawns << N; // shift north
		pushes = pushes & ~OCCUPIED; // space cant be occupied
		// scan the bits and make the move
		// char from = bit_index - 8;
		// char to = bit_index;
		// flag move_type = PAWN_PUSH;
		bitboard double_push = ((pushes & THIRD_RANK) << N) & ~OCCUPIED; // moves forward that cant be occupied
		// scan the bits and make the moves
		// char from = bit_index - 16;
		// char to = bit_index;
		// flag move_type = PAWN_DOUBLE_PUSH;
		bitboard left_captures = (pawns & ~A_FILE) << NW; // shift NW
		left_captures = left_captures & BLACK_PIECES;

		bitboard right_captures = (pawns & ~H_FILE) << NE; // shift NE
		right_captures = right_captures & BLACK_PIECES;

		while (pushes != 0)
		{
			int index = GetMSB(pushes);
			square to = ONE_BIT << index;
			square from = to >> S;
			auto move = std::make_shared<Move>
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
			pushes ^= to;
		}

		while (double_push != 0)
		{
			int index = GetMSB(double_push);
			square to = ONE_BIT << index;
			square from = to >> SS;
			auto move = std::make_shared<Move>
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
			double_push ^= to;
		}

		while(right_captures != 0)
		{
			int index = GetMSB(right_captures);
			square to = ONE_BIT << index;
			square from = to >> SW;
			auto move = std::make_shared<Move>
			(
				from,
				to,
				PAWN,
				NO_PIECE,
				this->GetPieceAtSquare(to, BLACK),
				NO_SQUARE,
				this->ep_square,
				CAPTURE,
				this->castle_flags,
				this->ply);
			moves->push_back(move);
			right_captures ^= to;
		}

		while(left_captures != 0)
		{
			int index = GetMSB(left_captures);
			square to = ONE_BIT << index;
			square from = to >> SE;
			auto move = std::make_shared<Move>
			(
				from,
				to,
				PAWN,
				NO_PIECE,
				this->GetPieceAtSquare(to, BLACK),
				NO_SQUARE,
				this->ep_square,
				CAPTURE,
				this->castle_flags,
				this->ply);
			moves->push_back(move);
			left_captures ^= to;	
		}

		return moves;
	}

	std::shared_ptr< std::vector<std::shared_ptr<Move>>> Board_Bit::GenerateBlackMoves()
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
}
