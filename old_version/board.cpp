#include "board.h"
#include <string>

void Panzer::Board::HashBoard()
{
	for (square s = 0; s < 128; s++)
	{
		if (IS_SQ(s))
		{
			if (this->pieces->at(s) != NONE)
			{
				this->zorbist ^= zorbist_lookup->Get_Hash_Value(s,this->pieces->at(s), this->colors->at(s));
			}
		}
	}
}

void Panzer::Board::FillSquare(square square, piece piece, color color) 
{
	this->pieces->at(square) = piece;
	this->colors->at(square) = color;
	this->zorbist ^= zorbist_lookup->Get_Hash_Value(square, piece, color);
}

void Panzer::Board::ClearSquare(square square)
{
	this->zorbist ^= zorbist_lookup->Get_Hash_Value(square, this->pieces->at(square), this->colors->at(square));
	this->pieces->at(square) = NONE;
	this->colors->at(square) = NO_COLOR;
}

piece Panzer::Board::GetPieceAt(square square)
{
	return this->pieces->at(square);
}

void Panzer::Board::SetPieceAt(square square, piece piece)
{
	this->pieces->at(square) = piece;
}

color Panzer::Board::GetColorAt(square square)
{
	return this->colors->at(square);
}

void Panzer::Board::SetColorAt(square square, color color)
{
	this->colors->at(square) = color;
}

std::shared_ptr<std::unordered_set<piece>> Panzer::Board::GetPieces(color color)
{
	switch (color) {
	case WHITE:
		return this->white_pieces;
	case BLACK:
		return this->black_pieces;
	}
}

color Panzer::Board::SideToMove()
{
	return this->side_to_move;
}

void Panzer::Board::SetSideToMove(color color)
{
	this->side_to_move = color;
}

square Panzer::Board::EPSquare()
{
	return this->ep_square;
}

void Panzer::Board::SetEPSquare(square square)
{
	this->ep_square = square;
}

int Panzer::Board::Ply()
{
	return this->ply;
}

void Panzer::Board::SetPly(int ply)
{
	this->ply = ply;
}

hash Panzer::Board::Zorbist() 
{
	return this->zorbist;
}

void Panzer::Board::SetZorbist(hash hash)
{
	this->zorbist = hash;
}

square Panzer::Board::KingSquare(color color) 
{
	switch(color) {
	case WHITE:
		return this->white_king;
	case BLACK:
		return this->black_king;
	}
}

void Panzer::Board::SetKingSquare(square square, color color)
{
	switch (color)
	{
	case WHITE:
		this->white_king = square;
		break;
	case BLACK:
		this->black_king = square;
		break;
	}
}

void Panzer::Board::ToggleColorHash()
{
	this->zorbist ^= zorbist_lookup->color_hash;
}

void Panzer::Board::ToggleCastleMoves(castle_flags flags)
{
	this->castle_moves ^= flags;
}

castle_flags Panzer::Board::CastleFlags()
{
	return this->castle_moves;
}

void Panzer::Board::SetCastleFlags(castle_flags flags)
{
	this->castle_moves = flags;
}

std::string Panzer::Board::BoardToFen()
{
	auto b = this;
	std::string fen = "";
	int empty_squares = 0;
	for (square s: this->fsi) 
	{
		if (IS_SQ(s))
		{
			color c = this->colors->at(s);
			piece p = this->pieces->at(s);
			if (p == NONE)
			{
				empty_squares++;
			}
			else
			{
				if (empty_squares != 0)
				{
					fen += std::to_string(empty_squares);
					empty_squares = 0;
				}
				switch (p) 
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
	}

	fen += " ";
	fen += this->side_to_move == WHITE ? "W" : "B";
	return fen;
}

void Panzer::Board::FenToBoard(const std::string& fen)
{
	bool board_done = false;
	for (int i = 0; i < 128; i++) 
	{
		this->pieces->at(i) = NONE;
		this->colors->at(i) = NO_COLOR;
	}

	int index = 0;
	for (char const& c : fen) 
	{
		if (board_done) break;
		switch (c) {
			case 'r':
				this->pieces->at(fsi[index]) = ROOK;
				this->colors->at(fsi[index]) = BLACK;
				index++;
				break;
			case 'n':
				this->pieces->at(fsi[index]) = KNIGHT;
				this->colors->at(fsi[index]) = BLACK;
				index++;
				break;
			case 'b':
				this->pieces->at(fsi[index]) = BISHOP;
				this->colors->at(fsi[index]) = BLACK;
				index++;
				break;
			case 'q':
				this->pieces->at(fsi[index]) = QUEEN;
				this->colors->at(fsi[index]) = BLACK;
				index++;
				break;
			case 'k':
				this->pieces->at(fsi[index]) = KING;
				this->colors->at(fsi[index]) = BLACK;
				index++;
				break;
			case 'p':
				this->pieces->at(fsi[index]) = PAWN;
				this->colors->at(fsi[index]) = BLACK;
				index++;
				break;
			case 'R':
				this->pieces->at(fsi[index]) = ROOK;
				this->colors->at(fsi[index]) = WHITE;
				index++;
				break;
			case 'N':
				this->pieces->at(fsi[index]) = KNIGHT;
				this->colors->at(fsi[index]) = WHITE;
				index++;
				break;
			case 'B':
				this->pieces->at(fsi[index]) = BISHOP;
				this->colors->at(fsi[index]) = WHITE;
				index++;
				break;
			case 'Q':
				this->pieces->at(fsi[index]) = QUEEN;
				this->colors->at(fsi[index]) = WHITE;
				index++;
				break;
			case 'K':
				this->pieces->at(fsi[index]) = KING;
				this->colors->at(fsi[index]) = WHITE;
				index++;
				break;
			case 'P':
				this->pieces->at(fsi[index]) = PAWN;
				this->colors->at(fsi[index]) = WHITE;
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

	for (int index = 0; index < 128; index++) 
	{
		if (this->colors->at(index) == WHITE) 
		{
			if (this->pieces->at(index) == KING) 
			{
				this->white_king = square(index);
			}

			this->white_pieces->insert(index);
		}
		else if (this->colors->at(index) == BLACK) 
		{
			this->black_pieces->insert(index);
			if (this->pieces->at(index) == KING) 
			{
				this->black_king = square(index);
			}
		}
	}
} 