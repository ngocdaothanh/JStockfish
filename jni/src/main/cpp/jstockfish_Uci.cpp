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

void benchmark(const Position& current, istream& is);

// FEN string of the initial position, normal chess
const char* StartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// The root position
Position *pos;

// Stack to keep track of the position states along the setup moves (from the
// start position to the position just before the search starts). Needed by
// 'draw by repetition' detection.
Search::StateStackPtr SetupStates;

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

jint JNI_OnLoad(JavaVM *vm, void* reserved) {
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

  pos = new Position(StartFEN, false, Threads.main());

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_uci(JNIEnv *env, jclass klass) {
  sync_cout << "id name " << engine_info(true)
            << "\n"       << Options
            << "\nuciok"  << sync_endl;
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_isready(JNIEnv *env, jclass klass) {
  sync_cout << "readyok" << sync_endl;
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_setoption(JNIEnv *env, jclass klass, jstring name, jstring value) {
  const char *chars_name = env->GetStringUTFChars(name, NULL);
  string str_name = chars_name;
  env->ReleaseStringUTFChars(name, chars_name);

  const char *chars_value = env->GetStringUTFChars(value, NULL);
  string str_value = chars_value;
  env->ReleaseStringUTFChars(value, chars_value);

  if (Options.count(str_name))
      Options[str_name] = str_value;
  else
      sync_cout << "No such option: " << name << sync_endl;
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_ucinewgame(JNIEnv *env, jclass klass) {
  Search::reset();
  Time.availableNodes = 0;
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_position(JNIEnv *env, jclass klass, jstring position) {
  const char *chars = env->GetStringUTFChars(position, NULL);
  istringstream is(chars);
  env->ReleaseStringUTFChars(position, chars);

  Move m;
  string token, fen;

  is >> token;

  if (token == "startpos")
  {
    fen = StartFEN;
    is >> token; // Consume "moves" token if any
  }
  else if (token == "fen")
    while (is >> token && token != "moves")
      fen += token + " ";
  else
    return;

  pos->set(fen, Options["UCI_Chess960"], Threads.main());
  SetupStates = Search::StateStackPtr(new std::stack<StateInfo>);

  // Parse move list (if any)
  while (is >> token && (m = UCI::to_move(*pos, token)) != MOVE_NONE)
  {
    SetupStates->push(StateInfo());
    pos->do_move(m, SetupStates->top(), pos->gives_check(m, CheckInfo(*pos)));
  }
}

//------------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_jstockfish_Uci_go(JNIEnv *env, jclass klass, jstring options) {
  const char *chars = env->GetStringUTFChars(options, NULL);
  istringstream is(chars);
  env->ReleaseStringUTFChars(options, chars);

  Search::LimitsType limits;
  string token;

  while (is >> token)
    if (token == "searchmoves")
      while (is >> token)
        limits.searchmoves.push_back(UCI::to_move(*pos, token));

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

  Threads.start_thinking(*pos, limits, SetupStates);
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_stop(JNIEnv *env, jclass klass) {
  Search::Signals.stop = true;
  Threads.main()->notify_one();  // Could be sleeping
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_ponderhit(JNIEnv *env, jclass klass) {
  if (Search::Signals.stopOnPonderhit) {
    Search::Signals.stop = true;
    Threads.main()->notify_one();  // Could be sleeping
  } else {
    Search::Limits.ponder = false; // Switch to normal search
  }
}

//------------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_jstockfish_Uci_flip(JNIEnv *env, jclass klass) {
  pos->flip();
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_bench(JNIEnv *env, jclass klass, jstring options) {
  const char *chars = env->GetStringUTFChars(options, NULL);
  istringstream is(chars);
  env->ReleaseStringUTFChars(options, chars);

  benchmark(*pos, is);
}

JNIEXPORT jstring JNICALL Java_jstockfish_Uci_d(JNIEnv *env, jclass klass) {
  sync_cout << *pos << sync_endl;
  return NULL;  // FIXME
}

JNIEXPORT jstring JNICALL Java_jstockfish_Uci_eval(JNIEnv *env, jclass klass) {
  sync_cout << Eval::trace(*pos) << sync_endl;
  return NULL;  // FIXME
}

JNIEXPORT void JNICALL Java_jstockfish_Uci_perft(JNIEnv *env, jclass klass, jint depth) {
  stringstream ss;
  ss << Options["Hash"]    << " "
     << Options["Threads"] << " " << depth << " current perft";
  benchmark(*pos, ss);
}

//------------------------------------------------------------------------------

JNIEXPORT jboolean JNICALL Java_jstockfish_Uci_islegal(JNIEnv *env, jclass klass, jstring move) {
  const char *chars = env->GetStringUTFChars(move, NULL);
  string str = chars;
  env->ReleaseStringUTFChars(move, chars);

  Move m = UCI::to_move(*pos, str);
  return pos->pseudo_legal(m);
}

JNIEXPORT jstring JNICALL Java_jstockfish_Uci_fen(JNIEnv *env, jclass klass) {
  string fen = pos->fen();
  jstring ret = env->NewStringUTF(fen.c_str());
  return ret;
}
