// Moved from original evaluate.cpp because we need to expose Trace::scores to JStockfish jni

#ifndef EVALUATE_TRACE_H_INCLUDED
#define EVALUATE_TRACE_H_INCLUDED

#include "types.h"

namespace Trace {
  enum Tracing {NO_TRACE, TRACE};

  enum Term { // The first 8 entries are for PieceType
    MATERIAL = 8, IMBALANCE, MOBILITY, THREAT, PASSED, SPACE, INITIATIVE, TOTAL, TERM_NB
  };

  extern double scores[TERM_NB][COLOR_NB][PHASE_NB];

  double to_cp(Value v);

  void add(int idx, Color c, Score s);

  void add(int idx, Score w, Score b = SCORE_ZERO);

  std::ostream& operator<<(std::ostream& os, Term t);
}

#endif // #ifndef EVALUATE_TRACE_H_INCLUDED
