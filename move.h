#include "bitboard_constants.h"

namespace Panzer 
{
    struct Move
    {
        unsigned int move;
        Move(square from, square to, move_flag flags)
        {
            move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
        }

        void operator=(Move a) { move = a.move; };

        square getTo() const { return move & 0x3ff; }
        square getFrom() const { return (move >> 6) & 0x3ff; }
        move_flag getFlags() const { return (move >> 12) & 0xf; }

        bool isCapture() const { return move & CAPTURE; }
        bool isCastle() const { return move & KING_CASTLE || move & QUEEN_CASTLE ; }
        bool isPromo() const { return move >= KNIGHT_PROMO; }
    };
}