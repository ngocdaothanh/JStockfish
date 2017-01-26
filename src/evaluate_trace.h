/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2017 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EVALUATE_TRACE_H_INCLUDED
#define EVALUATE_TRACE_H_INCLUDED

#include "types.h"

namespace Trace {
  enum Term { // The first 8 entries are for PieceType
    MATERIAL = 8, IMBALANCE, MOBILITY, THREAT, PASSED, SPACE, TOTAL, TERM_NB
  };

  extern double scores[TERM_NB][COLOR_NB][PHASE_NB];

  double to_cp(Value v);

  void add(int idx, Color c, Score s);

  void add(int idx, Score w, Score b = SCORE_ZERO);

  std::ostream& operator<<(std::ostream& os, Term t);
}

#endif // #ifndef EVALUATE_TRACE_H_INCLUDED
