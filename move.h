#include "bitboard_constants.h"

namespace Panzer 
{
    struct Move
    {
        char id = 0;
        square from = NO_SQUARE;
        square to = NO_SQUARE;
        piece piece_from = NO_PIECE;
        piece piece_to = NO_PIECE;
        piece captured_piece = NO_PIECE;
        bool is_castle = false;
        bool is_capture= false;
        int ply = 0;
        int score = 0;
        square ep = NO_SQUARE;
        square prior_ep = NO_SQUARE;
        move_flag move_flags = EMPTY_MOVE_FLAGS;
        castle_flag prior_castle_flags = EMPTY_CASTLE_FLAGS;

        Move(square from,
            square to,
            piece from_piece,
            piece to_piece,
            piece captured_piece,
            square ep_square,
            square prior_ep,
            move_flag move_flags,
            castle_flag prior_castle_flags,
            int ply) :
                from(from),
                to(to),
                piece_from(from_piece),
                piece_to(to_piece),
                captured_piece(captured_piece),
                move_flags(move_flags),
                ep(ep_square),
                prior_ep(prior_ep),
                prior_castle_flags(prior_castle_flags),
                ply(ply)
        {
            is_castle = !!(move_flags & CASTLE);
            is_capture = !!(move_flags & CAPTURE);
        }
    };
}