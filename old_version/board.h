#pragma once
#include "constants.h"
#include "zorbist.h"

namespace Panzer 
{
	class Board
	{
		const int fsi[64] = {
			A8, B8, C8, D8, E8, F8, G8, H8,
			A7, B7, C7, D7, E7, F7, G7, H7,
			A6, B6, C6, D6, E6, F6, G6, H6,
			A5, B5, C5, D5, E5, F5, G5, H5,
			A4, B4, C4, D4, E4, F4, G4, H4,
			A3, B3, C3, D3, E3, F3, G3, H3,
			A2, B2, C2, D2, E2, F2, G2, H2,
			A1, B1, C1, D1, E1, F1, G1, H1,
		};

		std::shared_ptr<std::array<piece, 128>> pieces = std::make_shared<std::array<piece, 128>>();
		std::shared_ptr<std::array<color, 128>> colors = std::make_shared<std::array<color, 128>>();
		std::shared_ptr<std::unordered_set<piece>> white_pieces = std::make_shared<std::unordered_set<piece>>();
		std::shared_ptr<std::unordered_set<piece>> black_pieces = std::make_shared<std::unordered_set<piece>>();
		color side_to_move = WHITE;
		char castle_moves = (1 | 2 | 4 | 8); // 1K 2 Q 4 k 4 q
		square ep_square = EMPTY;
		int ply = 0;
		hash zorbist = 0;
		square black_king = E8;
		square white_king = E1;
		Panzer::zorbist_lookup* zorbist_lookup = new Panzer::zorbist_lookup();

		public:
			std::string BoardToFen();
			void FenToBoard(const std::string& fen);
			void HashBoard();
			piece GetPieceAt(square square);
			void SetPieceAt(square square, piece piece);
			color GetColorAt(square square);
			void SetColorAt(square square, color color);
			std::shared_ptr<std::unordered_set<piece>> GetPieces(color color);
			color SideToMove();
			void SetSideToMove(color color);
			square EPSquare();
			void SetEPSquare(square square);
			int Ply();
			void SetPly(int ply);
			hash Zorbist();
			void SetZorbist(hash hash);
			square KingSquare(color color);
			void SetKingSquare(square square,  color color);
			void FillSquare(square square, piece piece, color color);
			void ClearSquare(square square);
			void ToggleColorHash();
			void ToggleCastleMoves(castle_flags flags);
			castle_flags CastleFlags();
			void SetCastleFlags(castle_flags flags);
	};
}

