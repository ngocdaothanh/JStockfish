package jstockfish;

/** States of a chess board position. */
public enum State {
    ALIVE,
    WHITE_MATE,       // White mates (white wins)
    BLACK_MATE,       // Black mates (black wins)
    WHITE_STALEMATE,  // White is stalemated (white can't move)
    BLACK_STALEMATE,  // Black is stalemated (black can't move)
    DRAW_50,          // Draw by 50-move rule
    DRAW_REPETITION   // Draw by 3-fold repetition
}
