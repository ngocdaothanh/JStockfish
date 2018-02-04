package jstockfish;

/** Evaluated relative scores of a position from white's point of view. */
public class Scores {
    private final double total;

    private final double pawn;
    private final double knight;
    private final double bishop;
    private final double rook;
    private final double queen;
    private final double king;

    private final double imbalance;
    private final double mobility;
    private final double threat;
    private final double passed;
    private final double space;

    public Scores(double[] scores) {
        if (scores.length != 12) {
            throw new IllegalArgumentException("scores have invalid length: " + scores.length);
        }

        total = scores[0];

        pawn = scores[1];

        knight = scores[2];
        bishop = scores[3];
        rook = scores[4];
        queen = scores[5];
        king = scores[6];

        imbalance = scores[7];
        mobility = scores[8];
        threat = scores[9];
        passed = scores[10];
        space = scores[11];
    }

    public double total() {
        return total;
    }

    public double pawn() {
        return pawn;
    }

    public double knight() {
        return knight;
    }

    public double bishop() {
        return bishop;
    }

    public double rook() {
        return rook;
    }

    public double queen() {
        return queen;
    }

    public double king() {
        return king;
    }

    public double imbalance() {
        return imbalance;
    }

    public double mobility() {
        return mobility;
    }

    public double threat() {
        return threat;
    }

    public double passed() {
        return passed;
    }

    public double space() {
        return space;
    }

    @Override
    public String toString() {
        return String.format(
            "total = %.02f\n" +
            "pawn = %.02f\nknight = %.02f\nbishop = %.02f\nrook = %.02f\nqueen = %.02f\nking = %.02f\n" +
            "imbalance = %.02f\nmobility = %.02f\nthreat = %.02f\npassed = %.02f\nspace = %.02f",
            total, pawn, knight, bishop, rook, queen, king, imbalance, mobility, threat, passed, space
        );
    }
}
