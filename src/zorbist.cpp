#include <zorbist.h>

namespace Panzer {
void zorbist_lookup::FillHashLookup() {

  for (square s = 0; s < 64; s++) {
    for (piece p : {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, NO_PIECE}) {
      for (color c : {WHITE, BLACK, NO_COLOR}) {
        zorbist_hash_lookup[s][p][c] = dist(engine);
      }
    }

    zorbist_ep_hash[s] = dist(engine);
  }

  zorbist_ep_hash[NO_SQUARE] = dist(engine);

  color_hash = dist(engine);

  for (castle_flag c = 0; c <= ALL_CASTLE_FLAGS; c++) {
    zorbist_castle_hash[c] = dist(engine);
  }
}

} // namespace Panzer