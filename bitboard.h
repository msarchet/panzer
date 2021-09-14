#pragma once
#include <stdlib.h>
#include <array>
#include <memory>
#include <vector>
#include <iostream>

#include "bitboard_constants.h"
#include "move.h"
#include "utils/board_utils.h"
#include "sliding-attacks/sliders.h"

typedef std::shared_ptr<std::vector<Panzer::Move> > MoveVector;

namespace Panzer
{
	class Board_Bit
	{
		std::array<bitboard, 7> *Pieces = new std::array<bitboard, 7> { 0ULL };
		std::array<bitboard, 2> *Colors = new std::array<bitboard, 2> { 0ULL };
		std::array<piece, 64> *pieceLookup = new std::array<piece, 64> {NO_PIECE};
		std::shared_ptr<Sliders> slider_attacks = std::make_shared<Sliders>();

		color side_to_move = WHITE;
		square ep_square = NO_SQUARE;
		castle_flag castle_flags = (WHITEK|WHITEQ|BLACKK|BLACKQ);
		uint8_t ply = 1;

	public:
		Board_Bit() {
			slider_attacks->Initialize();
		}

		Board_Bit(const Board_Bit &board)
		{
			for (int i = 0; i< 7; i++)
			{
				Pieces->at(i) = board.Pieces->at(i);
			}
			for (int i = 0; i< 2; i++)
			{
				Colors->at(i) = board.Colors->at(i);
			}
			for (int i = 0; i < 64; i++)
			{
				pieceLookup->at(i) = board.pieceLookup->at(i);
			}

			side_to_move = board.side_to_move;
			ep_square = board.ep_square;
			castle_flags = board.castle_flags;
			ply = board.ply;
			slider_attacks = board.slider_attacks;
		}

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
		color GetSideToMove() { return side_to_move; } 

		void MakeMove(const Move move);
		void UnmakeMove(const Move move);
		void ToggleBitBoards(square from, square to, piece p, color c);

		bool IsChecked(color color);
		void FenToBoard(const std::string& fen);
		std::string BoardToFen();
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

		void MakeRookMoves(MoveVector moves, bitboard rooks, bitboard same_side, bitboard other_side);
		void MakeKnightMoves(MoveVector moves, bitboard knights, bitboard same_side, bitboard other_side);
		void MakeBishopMoves(MoveVector moves, bitboard bishops, bitboard same_side, bitboard other_side);
		void MakeQueenMoves(MoveVector moves, bitboard queens, bitboard same_side, bitboard other_side);
		void MakeKingMoves(MoveVector moves, bitboard kings, bitboard same_side, bitboard other_side);

		bitboard GetKnightPossible(square center);
	}; //class Board_Bit
     
} //namespace Panzer