### Features

* UCI commands
* Additional commands convenient for checking if a move is legal, getting FEN
  and state from a position (check mate, stale mate, draw by 50-move rule or
  3-fold repetition rule)

### Build

```
./build.sh
```

`build` directory will be created, containing the compiled library.

### Try with SBT

For details, please see [Javadoc](http://ngocdaothanh.github.io/JStockfish/).

```
sbt console -Djava.library.path=build
```

Standard UCI commands:

```
import jstockfish.Uci

Uci.uci
Uci.setoption("Threads", Runtime.getRuntime.availableProcessors.toString)
Uci.setoption("UCI_Chess960", "true")

Uci.ucinewgame()
Uci.position("startpos moves d2d4")
Uci.go("infinite")
Uci.stop()
Uci.ponderhit()
```

Additional commands added by Stockfish:

```
Uci.flip
Uci.bench("512 4 16")
Uci.d
Uci.eval
Uci.perft(16)
```

Additional commands added by JStockfish:

```
Uci.islegal("g8f6")
Uci.fen
Uci.state
```

Stockfish can't process multiple games at a time. User of class `Uci` should
handle synchronization.

Additional commands added by JStockfish, independent of the current game,
can be called any time:

```
import jstockfish.Position

val chess960 = false
Position.islegal(chess960, "startpos moves d2d4", "g8f6")
Position.fen(chess960, "startpos moves d2d4")
Position.state(chess960, "startpos moves d2d4")
```
