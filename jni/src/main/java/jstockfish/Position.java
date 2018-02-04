package jstockfish;

/**
 * Additional commands added by JStockfish, independent of the current game,
 * can be called any time.
 */
public class Position {
    static {
        System.loadLibrary("jstockfish");
    }

    private static native double[] whiteScores(boolean chess960, String position);
    /**
     * Evaluates a UCI position.
     */
    public static Scores scores(boolean chess960, String position) {
        return new Scores(whiteScores(chess960, position));
    }

    /**
     * Given a UCI position (like {@code "startpos moves d2d4"}) and a move
     * (like {@code "g8f6"}), checks if the move is valid.
     */
    public static native boolean isLegal(boolean chess960, String position, String move);

    /**
     * Converts a UCI position (like {@code "startpos moves d2d4"}) to FEN
     * (like {@code "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1"}).
     */
    public static native String fen(boolean chess960, String position);

    private static native int ordinalState(boolean chess960, String position);
    /**
     * Returns {@link State} of a UCI position.
     */
    public static State state(boolean chess960, String position) {
        int ordinal = ordinalState(chess960, position);
        return State.values()[ordinal];
    }
}
