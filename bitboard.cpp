
#include "bitboard.h"

namespace Panzer
{
	piece Board_Bit::GetPieceAtSquare(square s)
	{
		return pieces->at(s);
	}

	void Board_Bit::ToggleBitBoards(square s, piece p, color c)
	{
		bitboard shifted = ONE_BIT << s;
		OCCUPIED ^= shifted;

		WHITE_PIECES ^= static_cast<uint64_t>(!c & !WHITE) << s;
		BLACK_PIECES ^= static_cast<uint64_t>(c & BLACK) << s;

		// no branching

		WHITE_PAWNS   ^= static_cast<uint64_t>((!c & !WHITE)&&(p & PAWN)) << s;
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

	void Board_Bit::FillSquare(square s, piece p, color c)
	{
		ToggleBitBoards(s, p, c);
		pieces->at(s) = p;
	}

	void Board_Bit::ClearSquare(square s,piece p, color c)
	{
		ToggleBitBoards(s, p, c);
		pieces->at(s) = NO_PIECE;
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
			int index = GetLSB(pushes);
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
			pushes &= pushes-1ULL;
		}

		while (double_push != 0)
		{
			int index = GetLSB(double_push);
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
			double_push &= double_push - 1ULL;
		}

		while(right_captures != 0)
		{
			int index = GetLSB(right_captures);
			square to = ONE_BIT << index;
			square from = to >> SW;
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
			square to = ONE_BIT << index;
			square from = to >> SE;
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
			left_captures &= left_captures - 1;	
		}

		return moves;
	}

	std::shared_ptr< std::vector<std::shared_ptr<Move>>> Board_Bit::GenerateBlackMoves()
	{

	}

	void Board_Bit::MakeMove(std::shared_ptr<Move> move)
	{
		ToggleBitBoards(move->from, move->piece_from, side_to_move);
		ToggleBitBoards(move->to, move->piece_to, side_to_move);
		side_to_move != side_to_move;
	}

	void Board_Bit::UnmakeMove(std::shared_ptr<Move> move)
	{
		side_to_move != side_to_move;
		ToggleBitBoards(move->from, move->piece_from, side_to_move);
		ToggleBitBoards(move->to, move->piece_to, side_to_move);
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
