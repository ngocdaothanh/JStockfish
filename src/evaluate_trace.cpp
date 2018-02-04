// Moved from original evaluate.cpp because we need to expose Trace::scores to JStockfish jni

#include <iomanip>
#include <iostream>

#include "evaluate_trace.h"

namespace Trace {
  double scores[TERM_NB][COLOR_NB][PHASE_NB];

  double to_cp(Value v) { return double(v) / PawnValueEg; }

  void add(int idx, Color c, Score s) {
    scores[idx][c][MG] = to_cp(mg_value(s));
    scores[idx][c][EG] = to_cp(eg_value(s));
  }

  void add(int idx, Score w, Score b) {
    add(idx, WHITE, w); add(idx, BLACK, b);
  }

  std::ostream& operator<<(std::ostream& os, Term t) {
    if (t == MATERIAL || t == IMBALANCE || t == Term(PAWN) || t == INITIATIVE || t == TOTAL)
        os << "  ---   --- |   ---   --- | ";
    else
        os << std::setw(5) << scores[t][WHITE][MG] << " "
            << std::setw(5) << scores[t][WHITE][EG] << " | "
            << std::setw(5) << scores[t][BLACK][MG] << " "
            << std::setw(5) << scores[t][BLACK][EG] << " | ";

    os << std::setw(5) << scores[t][WHITE][MG] - scores[t][BLACK][MG] << " "
        << std::setw(5) << scores[t][WHITE][EG] - scores[t][BLACK][EG] << " \n";

    return os;
  }
}
