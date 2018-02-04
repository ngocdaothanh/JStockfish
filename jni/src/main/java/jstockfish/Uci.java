package jstockfish;

/**
 * This class contains commands that mimic UCI commands.
 *
 * <p>{@link #go}, {@link #ponderhit}, {@link #bench}, and {@link #perft} are
 * async. Use {@link #setOutputListener} to capture Stockfish output; by default
 * Stockfish output is printed to stdout. Other methods are sync, they return
 * results immediately.
 *
 * <p>Stockfish can't process multiple games at a time. User of this class must
 * handle synchronization.
 */
public class Uci {
    static {
        System.loadLibrary("jstockfish");
    }

    // By default, just print to stdout
    private static OutputListener listener = new OutputListener() {
        @Override
        public void onOutput(String output) {
            System.out.println(output);
        }
    };

    /** Only one listener is supported. */
    public static void setOutputListener(OutputListener listener) {
        Uci.listener = listener;
    }

    /** Called by native code. This method will in turn call the listener. */
    private static void onOutput(String output) {
        if (listener != null) {
            listener.onOutput(output);
        }
    }

    // Standard UCI commands ---------------------------------------------------
    // - debug: unavailable because Stockfish always outputs "info"
    // - isready: unavailable this lib is always ready

    /** Returns UCI id and options. */
    public static native String uci();

    /** Returns {@code false} if the option name is incorrect. */
    public static native boolean setOption(String name, String value);

    public static native void newGame();

    /** Returns false if the position (like {@code "startpos moves d2d4"}) is invalid. */
    public static native boolean position(String position);

    /** Searches for the best move. */
    public static native void go(String options);

    /** Stops when {@code go("infinite")} is called. */
    public static native void stop();

    public static native void ponderHit();

    // Additional commands added by Stockfish ----------------------------------

    /** Prints (visualize) the current position. */
    public static native String d();

    /** Flips the current position. */
    public static native void flip();

    /** Evaluates the current position. */
    public static native String eval();

    // Additional commands added by JStockfish ---------------------------------

    private static native double[] whiteScores();
    /**
     * Evaluates the current position.
     */
    public static Scores scores() {
        return new Scores(whiteScores());
    }

    /**
     * Given the current position (set by {@link #position}) and a move
     * (like {@code "g8f6"}), checks if the move is valid.
     */
    public static native boolean isLegal(String move);

    /**
     * Converts the current position (set by {@link #position}) to FEN
     * (like {@code "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1"}).
     */
    public static native String fen();

    private static native int ordinalState();
    /**
     * Returns {@link State} of the current position (set by {@link #position}).
     */
    public static State state() {
        int ordinal = ordinalState();
        return State.values()[ordinal];
    }
}
