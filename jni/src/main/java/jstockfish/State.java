package jstockfish;

/** States of a chess board position. */
public enum State {
    // The order is NOT similar to that of Cuckoo chess engine:
    // https://code.google.com/p/cuckoochess/source/browse/trunk/CuckooChessEngine/src/chess/Game.java#134

    ALIVE,

    /** White mates (white wins) */
    WHITE_MATE,
    /** Black mates (black wins) */
    BLACK_MATE,

    /** White is stalemated (white can't move); automatic draw */
    WHITE_STALEMATE,
    /** Black is stalemated (black can't move); automatic draw */
    BLACK_STALEMATE,
    /** Draw by insufficient material; automatic draw */
    DRAW_NO_MATE,

    /** Can draw by 50-move rule; players must claim explicitly */
    CAN_DRAW_50,
    /** Can draw by 3-fold repetition rule; players must claim explicitly */
    CAN_DRAW_REP;

    public boolean isAutomaticOver() {
        return
            this == WHITE_MATE || this == BLACK_MATE ||
            this == WHITE_STALEMATE || this == BLACK_STALEMATE ||
            this == DRAW_NO_MATE;
    }
}
