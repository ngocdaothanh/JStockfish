#include <iostream>
#include <sstream>
#include <string>

#include "evaluate.h"
#include "movegen.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "timeman.h"
#include "tt.h"
#include "uci.h"
#include "syzygy/tbprobe.h"

#include "jstockfish_Uci.h"
#include "jstockfish_Position.h"

using namespace std;

//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

jint JNI_OnLoad(JavaVM* vm, void* reserved);

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------

static JavaVM* jvm = NULL;
static jclass uciClass = NULL;
static jmethodID onOutput = NULL;

// Call jstockfish.Uci.onOutput
void uci_out(string output) {
  // Should another thread need to access the Java VM, it must first call
  // AttachCurrentThread() to attach itself to the VM and obtain a JNI interface pointer
  JNIEnv *env;
  if (jvm->AttachCurrentThread((void **) &env, NULL) != 0) {
    cout << "[JNI] Could not AttachCurrentThread" << endl;
    return;
  }

  jstring js = env->NewStringUTF(output.c_str());
  env->CallStaticVoidMethod(uciClass, onOutput, js);
  jvm->DetachCurrentThread();
}

//------------------------------------------------------------------------------

// In Stockfish's benchmark.cpp
void benchmark(const Position& current, istream& is);

// FEN string of the initial position, normal chess
static const char* gStartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// The root position
static Position *gpos;

// Stack to keep track of the position states along the setup moves (from the
// start position to the position just before the search starts). Needed by
// 'draw by repetition' detection.
static Search::StateStackPtr gSetupStates;

bool initJvm(JavaVM *vm) {
  jvm = vm;

  JNIEnv *env;
  int stat = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
	if (stat != JNI_OK) {
    cout << "[JNI] Could not GetEnv" << endl;
    return false;
  }

  uciClass = env->FindClass("Ljstockfish/Uci;");
	if (!uciClass) {
    cout << "[JNI] Could not find class jstockfish.Uci" << endl;
    return false;
  }

  onOutput = env->GetStaticMethodID(uciClass, "onOutput", "(Ljava/lang/String;)V");
	if (!onOutput) {
    cout << "[JNI] Could not get method jstockfish.Uci.onOutput" << endl;
    return false;
  }

  return true;
}

bool read_position(
  JNIEnv *env, jboolean chess960, jstring position,
  Position& pos, Search::StateStackPtr& setupStates
) {
  const char *chars = env->GetStringUTFChars(position, NULL);
  istringstream is(chars);
  env->ReleaseStringUTFChars(position, chars);

  Move m;
  string token, fen;

  is >> token;

  if (token == "startpos")
  {
    fen = gStartFEN;
    is >> token; // Consume "moves" token if any
  }
  else if (token == "fen")
    while (is >> token && token != "moves")
      fen += token + " ";
  else
    return false;

  pos.set(fen, chess960, Threads.main());

  // Parse move list (if any)
  while (is >> token)
  {
    m = UCI::to_move(pos, token);
    if (m == MOVE_NONE) return false;

    setupStates->push(StateInfo());
    pos.do_move(m, setupStates->top(), pos.gives_check(m, CheckInfo(pos)));
  }

  return true;
}

bool islegal(Position& pos, JNIEnv *env, jstring move) {
  const char *chars = env->GetStringUTFChars(move, NULL);
  string str = chars;
  env->ReleaseStringUTFChars(move, chars);

  Move m = UCI::to_move(pos, str);
  return pos.pseudo_legal(m);
}

//------------------------------------------------------------------------------

jint JNI_OnLoad(JavaVM *vm, void*) {
	if (!initJvm(vm)) return JNI_VERSION_1_6;

  UCI::init(Options);
  PSQT::init();
  Bitboards::init();
  Position::init();
  Bitbases::init();
  Search::init();
  Eval::init();
  Pawns::init();
  Threads.init();
  Tablebases::init(Options["SyzygyPath"]);
  TT.resize(Options["Hash"]);

  gpos = new Position(gStartFEN, false, Threads.main());

  return JNI_VERSION_1_6;
}

JNIEXPORT jstring JNICALL Java_jstockfish_Uci_uci(JNIEnv *env, jclass) {
  stringstream uci;
  uci << "id name " << engine_info(true)
      << "\n"       << Options
      << "\nuciok";

  jstring ret = env->NewStringUTF(uci.str().c_str());
  return ret;
}

JNIEXPORT jboolean JNICALL Java_jstockfish_Uci_setoption(JNIEnv *env, jclass, jstring name, jstring value) {
  const char *chars_name = env->GetStringUTFChars(name, NULL);
  string str_name = chars_name;
  env->ReleaseStringUTFChars(name, chars_name);

  const char *chars_value = env->GetStringUTFChars(value, NULL);
  string str_value = chars_value;
  env->ReleaseStringUTFChars(value, chars_value);

  if (Options.count(str_name)) {
      Options[str_name] = str_value;
      return true;
  } else {
      return false;
  }
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_ucinewgame(JNIEnv *, jclass) {
  Search::reset();
  Time.availableNodes = 0;
}

JNIEXPORT jboolean JNICALL Java_jstockfish_Uci_position(JNIEnv *env, jclass, jstring position) {
  gSetupStates = Search::StateStackPtr(new std::stack<StateInfo>);
  return read_position(
    env, Options["UCI_Chess960"], position,
    *gpos, gSetupStates
  );
}

//------------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_jstockfish_Uci_go(JNIEnv *env, jclass, jstring options) {
  const char *chars = env->GetStringUTFChars(options, NULL);
  istringstream is(chars);
  env->ReleaseStringUTFChars(options, chars);

  Search::LimitsType limits;
  string token;

  while (is >> token)
    if (token == "searchmoves")
      while (is >> token)
        limits.searchmoves.push_back(UCI::to_move(*gpos, token));

    else if (token == "wtime")     is >> limits.time[WHITE];
    else if (token == "btime")     is >> limits.time[BLACK];
    else if (token == "winc")      is >> limits.inc[WHITE];
    else if (token == "binc")      is >> limits.inc[BLACK];
    else if (token == "movestogo") is >> limits.movestogo;
    else if (token == "depth")     is >> limits.depth;
    else if (token == "nodes")     is >> limits.nodes;
    else if (token == "movetime")  is >> limits.movetime;
    else if (token == "mate")      is >> limits.mate;
    else if (token == "infinite")  limits.infinite = true;
    else if (token == "ponder")    limits.ponder = true;

  Threads.start_thinking(*gpos, limits, gSetupStates);
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_stop(JNIEnv *, jclass) {
  Search::Signals.stop = true;
  Threads.main()->notify_one();  // Could be sleeping
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_ponderhit(JNIEnv *, jclass) {
  if (Search::Signals.stopOnPonderhit) {
    Search::Signals.stop = true;
    Threads.main()->notify_one();  // Could be sleeping
  } else {
    Search::Limits.ponder = false; // Switch to normal search
  }
}

//------------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_jstockfish_Uci_flip(JNIEnv *, jclass) {
  gpos->flip();
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_bench(JNIEnv *env, jclass, jstring options) {
  const char *chars = env->GetStringUTFChars(options, NULL);
  istringstream is(chars);
  env->ReleaseStringUTFChars(options, chars);

  benchmark(*gpos, is);
}

JNIEXPORT jstring JNICALL Java_jstockfish_Uci_d(JNIEnv *env, jclass) {
  stringstream ss;
  ss << *gpos;
  jstring ret = env->NewStringUTF(ss.str().c_str());
  return ret;
}

JNIEXPORT jstring JNICALL Java_jstockfish_Uci_eval(JNIEnv *env, jclass) {
  stringstream ss;
  ss << Eval::trace(*gpos);
  jstring ret = env->NewStringUTF(ss.str().c_str());
  return ret;
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_perft(JNIEnv *, jclass, jint depth) {
  stringstream ss;
  ss << Options["Hash"]    << " "
     << Options["Threads"] << " " << depth << " current perft";
  benchmark(*gpos, ss);
}

//------------------------------------------------------------------------------

JNIEXPORT jboolean JNICALL Java_jstockfish_Uci_islegal(
  JNIEnv *env, jclass, jstring move
) {
  return islegal(*gpos, env, move);
}

JNIEXPORT jstring JNICALL Java_jstockfish_Uci_fen(JNIEnv *env, jclass) {
  string fen = gpos->fen();
  jstring ret = env->NewStringUTF(fen.c_str());
  return ret;
}

JNIEXPORT jint JNICALL Java_jstockfish_Uci_positionstate(JNIEnv *, jclass) {
  return positionstate(*gpos);
}

//------------------------------------------------------------------------------

JNIEXPORT jboolean JNICALL Java_jstockfish_Position_islegal(
  JNIEnv *env, jclass, jboolean chess960, jstring position, jstring move
) {
  Position pos(gStartFEN, chess960, Threads.main());
  Search::StateStackPtr setupStates = Search::StateStackPtr(new std::stack<StateInfo>);
  return
    read_position(env, chess960, position, pos, setupStates) &&
    islegal(pos, env, move);
}

JNIEXPORT jstring JNICALL Java_jstockfish_Position_fen(
  JNIEnv *env, jclass, jboolean chess960, jstring position
) {
  Position pos(gStartFEN, chess960, Threads.main());
  Search::StateStackPtr setupStates = Search::StateStackPtr(new std::stack<StateInfo>);
  read_position(env, chess960, position, pos, setupStates);

  // When read_position above returns false, the result is somewhere in the middle
  string fen = pos.fen();
  jstring ret = env->NewStringUTF(fen.c_str());
  return ret;
}

JNIEXPORT jint JNICALL Java_jstockfish_Position_positionstate(
  JNIEnv *env, jclass, jboolean chess960, jstring position
) {
  Position pos(gStartFEN, chess960, Threads.main());
  Search::StateStackPtr setupStates = Search::StateStackPtr(new std::stack<StateInfo>);
  read_position(env, chess960, position, pos, setupStates);

  // When read_position above returns false, the result is somewhere in the middle
  return positionstate(pos);
}
