#include "bitboard_constants.h"

namespace Panzer 
{
    struct Move
    {
        square m_from;
        square m_to;
        move_flag move_flags;
        piece capturedPiece = NO_PIECE;
        castle_flag priorCastleFlags = EMPTY_CASTLE_FLAGS;
        square priorEP = NO_SQUARE;
        Move(square from, square to, move_flag flags, castle_flag castleFlags, piece captured = NO_PIECE, square epSquare = NO_SQUARE)
        {
            m_from = from;
            m_to = to;
            move_flags = flags;
            priorCastleFlags = castleFlags;
            capturedPiece = captured;
            priorEP = epSquare;
        }


        square getTo() const { return m_to; }
        square getFrom() const { return m_from; }
        move_flag getFlags() const { return move_flags; }

        bool isCapture() const { return getFlags() == CAPTURE || getFlags() == EP_CAPTURE; }
        bool isEPCapture() const { return getFlags() == EP_CAPTURE; }
        bool isCastle() const { return getFlags() == KING_CASTLE || getFlags() == QUEEN_CASTLE ; }
        bool isPromo() const { return getFlags() >= KNIGHT_PROMO; }

        castle_flag getCastleFlags() const { return priorCastleFlags; }
        piece getCapturedPiece() const { return capturedPiece; }
        square getPriorEPSquare() const { return priorEP; }
    };
}