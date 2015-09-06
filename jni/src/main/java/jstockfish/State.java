package jstockfish;

/**
 * States of a chess board position.
 * The order is similar to that of Cuckoo chess engine:
 * https://code.google.com/p/cuckoochess/source/browse/trunk/CuckooChessEngine/src/chess/Game.java#134
 */
public enum State {
    ALIVE,
    WHITE_MATE,       // White mates (white wins)
    BLACK_MATE,       // Black mates (black wins)
    WHITE_STALEMATE,  // White is stalemated (white can't move)
    BLACK_STALEMATE,  // Black is stalemated (black can't move)
    CAN_DRAW_REP,     // Can draw by 3-fold repetition rule
    CAN_DRAW_50;      // Can draw by 50-move rule

    public boolean isOver() {
        return this == WHITE_MATE || this == BLACK_MATE || this == WHITE_STALEMATE || this == BLACK_STALEMATE;
    }
}
