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

#include <iomanip>
#include <iostream>

#include "evaluate_trace.h"

namespace Trace {
  double scores[TERM_NB][COLOR_NB][PHASE_NB];

  double to_cp(Value v) {
    return double(v) / PawnValueEg;
  }

  void add(int idx, Color c, Score s) {
    scores[idx][c][MG] = to_cp(mg_value(s));
    scores[idx][c][EG] = to_cp(eg_value(s));
  }

  void add(int idx, Score w, Score b) {
    add(idx, WHITE, w);
    add(idx, BLACK, b);
  }

  std::ostream& operator<<(std::ostream& os, Term t) {
    if (t == MATERIAL || t == IMBALANCE || t == Term(PAWN) || t == TOTAL)
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
