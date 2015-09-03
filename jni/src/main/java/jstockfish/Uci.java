package jstockfish;

/**
 * Stockfish can't process multiple games at a time. User of this class should
 * handle synchronization.
 */
public class Uci
{
    static {
       System.loadLibrary("jstockfish");
    }

    //--------------------------------------------------------------------------

    private static OutputListener listener = null;

    /** Only one listener is supported. */
    public static void setOutputListener(OutputListener listener) {
        Uci.listener = listener;
    }

    /** Called by native code. This method will in turn call the listener. */
    private static void onOutput(String output) {
        System.out.println("output: " + output);
        if (listener != null) listener.onOutput(output);
    }

    // Standard UCI commands ---------------------------------------------------
    // - debug: unavailable because Stockfish always outputs "info"
    // - isready: unavailable this lib is always ready

    /** Returns UCI id and options. */
    public static native String uci();

    /** Returns false if the option name is incorrect. */
    public static native boolean setoption(String name, String value);

    public static native void ucinewgame();

    /** Returns false if the position is invalid. */
    public static native boolean position(String position);

    /** The result will be output asynchronously. */
    public static native void go(String options);

    /** Stops when {@code go("infinite")} is called. */
    public static native void stop();

    public static native void ponderhit();

    // Additional commands added by Stockfish ----------------------------------

    public static native void flip();

    /** The result will be output asynchronously. */
    public static native void bench(String options);

    /** The result will be output asynchronously. */
    public static native void perft(int depth);

    public static native String d();

    public static native String eval();

    // Additional commands added by JStockfish ---------------------------------

    public static native boolean islegal(String move);

    public static native String fen();

    private static native int positionstate();

    public static State state() {
        int ordinal = positionstate();
        return State.values()[ordinal];
    }
}
