#include "positionstate.h"
#include "movegen.h"

int positionstate(Position& pos) {
  Bitboard checkers = pos.checkers();
  int noLegalMove = MoveList<LEGAL>(pos).size() == 0;

  if (noLegalMove) {
    if (checkers) {
      return (pos.side_to_move() == BLACK)? WHITE_MATE : BLACK_MATE;
    } else {
      return (pos.side_to_move() == BLACK)? BLACK_STALEMATE : WHITE_STALEMATE;
    }
  }

  // Don't use Posistion::is_draw_rule50 to avoid checking noLegalMove again,
  // see the implementation of is_draw_rule50
  if (pos.rule50_count() > 99 && !checkers) return CAN_DRAW_50;

  if (pos.is_draw_repetition()) return CAN_DRAW_REP;

  return ALIVE;
}
