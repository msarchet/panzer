#include "bitboard_constants.h"

namespace Panzer 
{
    struct Move
    {
        unsigned int move;
        piece capturedPiece = NO_PIECE;
        castle_flag priorCastleFlags = EMPTY_CASTLE_FLAGS;
        square priorEP = NO_SQUARE;
        Move(square from, square to, move_flag flags, castle_flag castleFlags, piece captured = NO_PIECE, square epSquare = NO_SQUARE)
        {
            move = ((flags & 0x3f) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
            priorCastleFlags = castleFlags;
            capturedPiece = captured;
            priorEP = epSquare;
        }


        void operator=(Move a) 
        { 
            move = a.move; 
            capturedPiece = a.capturedPiece;
            priorCastleFlags = a.priorCastleFlags;
            priorEP = a.priorEP;
        };

        square getTo() const { return move & 0x3f; }
        square getFrom() const { return (move >> 6) & 0x3f; }
        move_flag getFlags() const { return (move >> 12) & 0x3f; }

        bool isCapture() const { return getFlags() & CAPTURE; }
        bool isEPCapture() const { return getFlags() & EP_CAPTURE; }
        bool isCastle() const { return getFlags() & KING_CASTLE || getFlags() & QUEEN_CASTLE ; }
        bool isPromo() const { return getFlags() >= KNIGHT_PROMO; }

        castle_flag getCastleFlags() const { return priorCastleFlags; }
        piece getCapturedPiece() const { return capturedPiece; }
        square getPriorEPSquare() const { return priorEP; }
    };
}