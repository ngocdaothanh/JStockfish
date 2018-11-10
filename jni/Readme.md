# JStockfish

Java JNI binding for Stockfish.

## Features

* [UCI commands](https://github.com/ngocdaothanh/JStockfish/blob/jni/engine-interface.txt)
* Additional commands convenient for checking if a move is legal, getting FEN
  and state from a position (check mate, stale mate, draw by 50-move rule or
  3-fold repetition rule)

## Build

You need [CMake](http://www.cmake.org/), JVM, and a C++ compiler to build:

```sh
./build.sh
```

`build` directory will be created, containing the compiled library.

If you see error like this:

```sh
Could NOT find JNI
```

Then try exporting `JAVA_HOME` and try building again:

```sh
export JAVA_HOME=/usr/lib/jvm/java-8-oracle
```

## Try with SBT

For details, please see [Javadoc](http://ngocdaothanh.github.io/JStockfish/).

```sh
sbt console -Djava.library.path=build
```

If there's error
"java.lang.UnsatisfiedLinkError: no jstockfish in java.library.path",
try using [this snippet](http://www.scala-lang.org/old/node/7542):

```scala
def unsafeAddDir(dir: String) = try {
  val field = classOf[ClassLoader].getDeclaredField("usr_paths")
  field.setAccessible(true)
  val paths = field.get(null).asInstanceOf[Array[String]]
  if (!(paths contains dir)) {
    field.set(null, paths :+ dir)
    System.setProperty(
      "java.library.path",
      System.getProperty("java.library.path") + java.io.File.pathSeparator + dir)
  }
} catch {
  case _: IllegalAccessException =>
    println("Insufficient permissions; can't modify private variables.")
  case _: NoSuchFieldException =>
    println("JVM implementation incompatible with path hack")
}

unsafeAddDir("./build")
```

### Standard UCI commands

```scala
import jstockfish.Uci

Uci.uci
Uci.setOption("Threads", Runtime.getRuntime.availableProcessors.toString)
Uci.setOption("UCI_Chess960", "true")

Uci.newGame()
Uci.position("startpos moves d2d4")
Uci.go("infinite")
Uci.stop()
Uci.ponderHit()
```

### Additional commands added by Stockfish

```scala
Uci.d
Uci.flip
Uci.eval
```

### Additional commands added by JStockfish

```scala
Uci.scores
Uci.isLegal("g8f6")
Uci.fen
Uci.state
```

State enum:

```java
ALIVE,

WHITE_MATE,       // White mates (white wins)
BLACK_MATE,       // Black mates (black wins)

// Automatic draw
WHITE_STALEMATE,  // White is stalemated (white can't move)
BLACK_STALEMATE,  // Black is stalemated (black can't move)
DRAW_NO_MATE,     // Draw by insufficient material

// Can draw, but players must claim
CAN_DRAW_50,      // Can draw by 50-move rule
CAN_DRAW_REP      // Can draw by 3-fold repetition rule
```

### Additional commands added by JStockfish, independent of the current game

Stockfish can't process multiple games at a time. Calls to methods of class
`Uci` should be synchronized. However, methods of class `Position` can be called
any time, no need to synchronize:

```scala
import jstockfish.Position

val chess960 = false
Position.scores(chess960, "startpos moves d2d4")
Position.isLegal(chess960, "startpos moves d2d4", "g8f6")
Position.fen(chess960, "startpos moves d2d4")
Position.state(chess960, "startpos moves d2d4")
```

### Get additional info together with bestmove

Instead of sending `Uci.go` to get `bestmove`, then `Position.state` to get
state of the position after the move is made, you can set option to tell `UCI.go`
to also output `info state <state enum>` before outputing `bestmove`:

```scala
Uci.setOption("Info State", "true")
```

This way, you can get better performance.

Similarly, to get `info fen <FEN>`:

```scala
Uci.setOption("Info FEN", "true")
```

### Capture Stockfish output

By default Stockfish output is printed to the console asynchronously.
You can capture:

```scala
import jstockfish.OutputListener

Uci.setOutputListener(new OutputListener {
  override def onOutput(output: String) {
    // Do something with output
  }
})
```
