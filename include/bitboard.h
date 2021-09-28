#pragma once
#include <stdlib.h>
#include <array>
#include <memory>
#include <vector>
#include <iostream>

#include "bitboard_constants.h"
#include "board_utils.h"
#include "sliders.h"
#include "zorbist.h"

namespace Panzer
{
	class Board
	{
		std::array<bitboard, 7> *Pieces = new std::array<bitboard, 7> { 0ULL };
		std::array<bitboard, 2> *Colors = new std::array<bitboard, 2> { 0ULL };
		std::array<piece, 64> *pieceLookup = new std::array<piece, 64> {NO_PIECE};
		std::shared_ptr<Sliders> slider_attacks = std::make_shared<Sliders>();
		std::vector<Move> *moveChain = new std::vector<Move>();
		color side_to_move = WHITE;
		square ep_square = NO_SQUARE;
		castle_flag castle_flags = (WHITEK|WHITEQ|BLACKK|BLACKQ);
		uint8_t ply = 1;
		hash boardHash = 0;
		zorbist_lookup* zorbist = new zorbist_lookup();
	public:
		Board() {
			slider_attacks->Initialize();
		}

		Board(const Board &board) : Board()
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
			boardHash = board.boardHash;
		}

		void FillSquare(square s, piece p, color c);
		void ClearSquare(square s,piece p, color c);
		void PrintBoard();

		int GenerateMoves(Move* moves, bool captures = false);
		int GenerateWhiteMoves(Move* moves, bool captures = false);
		int GenerateBlackMoves(Move* moves, bool captures = false);

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
		square GetEpSquare() { return ep_square; }
		int GetPly() { return ply; }
		hash GetHash() { return boardHash; }

		void MakeMove(const Move move);
		void UnmakeMove(const Move move);
		void ToggleBitBoards(square from, square to, piece p, color c);

		bool IsSquareAttacked(square s, color color);
		bool IsChecked(color color);
		void FenToBoard(const std::string& fen);
		std::string PrintMoveChain();
		std::string BoardToFen();
		piece GetPieceAtSquare(square s);
	private:
		int MakeWhitePawnMoves(Move* moves, int movecount, bool captures = false);
		int MakeWhiteRooksMoves(Move* moves, int movecount, bool captures = false);
		int MakeWhiteKnightMoves(Move* moves, int movecount, bool captures = false);
		int MakeWhiteBishopMoves(Move* moves, int movecount, bool captures = false);
		int MakeWhiteQueenMoves(Move* moves, int movecount, bool captures = false);
		int MakeWhiteKingMoves(Move* moves, int movecount, bool captures = false);
		int MakeBlackPawnMoves(Move* moves, int movecount, bool captures = false);
		int MakeBlackRooksMoves(Move* moves, int movecount, bool captures = false);
		int MakeBlackKnightMoves(Move* moves, int movecount, bool captures = false);
		int MakeBlackBishopMoves(Move* moves, int movecount, bool captures = false);
		int MakeBlackQueenMoves(Move* moves, int movecount, bool captures = false);
		int MakeBlackKingMoves(Move* moves, int movecount, bool captures = false);

		int MakeRookMoves(Move* moves, int movecount, bitboard rooks, bitboard same_side, bitboard other_side, bool captures = false);
		int MakeKnightMoves(Move* moves, int movecount, bitboard knights, bitboard same_side, bitboard other_side, bool captures = false);
		int MakeBishopMoves(Move* moves, int movecount, bitboard bishops, bitboard same_side, bitboard other_side, bool captures = false);
		int MakeQueenMoves(Move* moves, int movecount, bitboard queens, bitboard same_side, bitboard other_side, bool captures = false);
		int MakeKingMoves(Move* moves, int movecount, bitboard kings, bitboard same_side, bitboard other_side, bool captures = false);
        void PushMove(Move* moves, int movecount, square from, square to, move_flag flags, castle_flag castleFlags, piece captured = NO_PIECE, square epSquare = NO_SQUARE);
	}; //class Board
     
} //namespace Panzer