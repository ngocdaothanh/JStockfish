### Features

TODO

For convenience, the number of threads is set to the number of CPU cores.

### Build

```
./build.sh
```

`build` directory will be created, containing the compiled library.

### Try with SBT

```
sbt console -Djava.library.path=build

import jstockfish.Uci._

// Standard UCI commands
uci
isready
setoption("Threads", "4")

ucinewgame
position("startpos")
go("infinite")
stop
ponderhit

// Additional commands added by Stockfish
flip
bench("512 4 16")
d
eval
perft(16)

// Additional commands added by JStockfish
islegal("d2d4")
```
