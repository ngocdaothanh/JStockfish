package jstockfish;

/** Listener to capture Stockfish async output. */
public interface OutputListener {
    void onOutput(String output);
}
