#pragma once
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

		color side_to_move = WHITE;
		square ep_square = NO_SQUARE;
		castle_flag castle_flags = (WHITEK | WHITEQ | BLACKK | BLACKQ);
		uint8_t ply = 1;
		std::array<piece, 64>* pieces = new std::array<piece, 64> {NO_PIECE};

	public:
		Board_Bit() {};
		Board_Bit(const Board_Bit& board)
		{
			WHITE_PIECES = board.WHITE_PIECES;
			WHITE_PAWNS = board.WHITE_PAWNS;
			WHITE_KINGS = board.WHITE_KINGS;
			WHITE_ROOKS = board.WHITE_ROOKS;
			WHITE_BISHOPS = board.WHITE_BISHOPS;
			WHITE_KNIGHTS = board.WHITE_KNIGHTS;
			WHITE_QUEENS = board.WHITE_KINGS;
			BLACK_PIECES = board.BLACK_PIECES;
			BLACK_PAWNS = board.BLACK_PAWNS;
			BLACK_KINGS = board.BLACK_KINGS;
			BLACK_ROOKS = board.BLACK_ROOKS;
			BLACK_BISHOPS = board.BLACK_BISHOPS;
			BLACK_KNIGHTS = board.BLACK_KNIGHTS;
			BLACK_QUEENS = board.BLACK_KINGS;
			OCCUPIED = board.OCCUPIED;

			side_to_move = board.side_to_move;
			ep_square = board.ep_square;
			castle_flags = board.castle_flags;
			ply = board.ply;
			//pieces = board.pieces;
		};

		void FillSquare(square s, piece p, color c);
		void ClearSquare(square s, piece p, color c);
		std::shared_ptr<std::vector<std::shared_ptr<Move>>> GenerateMoves();
		std::shared_ptr<std::vector<std::shared_ptr<Move>>> GenerateWhiteMoves();
		std::shared_ptr<std::vector<std::shared_ptr<Move>>> GenerateBlackMoves();
		void PrintBoard(bitboard b);
		void MakeMove(std::shared_ptr<Move> move);
		void UnmakeMove(std::shared_ptr<Move> move);

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
		piece GetPieceAtSquare(square s);
		void ToggleBitBoards(square s, piece p, color c);
	};
}
