package jstockfish;

public class Uci
{
    static {
       System.loadLibrary("jstockfish");
    }

    // Standard UCI commands
    public static native void uci();
    public static native void isready();
    public static native void setoption(String name, String value);
    public static native void ucinewgame();
    public static native void position(String position);
    public static native void go(String options);
    public static native void stop();
    public static native void ponderhit();

    // Additional commands added by Stockfish
    public static native void flip();
    public static native void bench(String options);
    public static native String d();
    public static native String eval();
    public static native void perft(int depth);

    // Additional commands added by JStockfish
    public static native boolean islegal(String move);
    public static native String fen();

    public static void onOutput(String output) {
        System.out.println("output: " + output);
    }
}
