#include <iostream>
#include <signal.h>
#include "audiolib/Node.h"
#include "audiolib/Graph.h"
#include "audiolib/Utils.h"
#include <functional>
#include "stk/RtMidi.h"
#include <vector>

using namespace audiolib;
using namespace std;

int main( int argc, char *argv[]){

  NodeSettings s;
  s.sample_rate_ = 44100;
  s.block_size_ = 64;
  s.num_audio_inputs_ = 3;
  s.num_audio_outputs_ = 1;

  Graph g(s);

  s.num_audio_inputs_ = 3;
  int d_id = g.registerNode(new AudioAdder(s));

  g.connectAudio(g.INPUT_ID, 0, d_id, 0);
  g.connectAudio(g.INPUT_ID, 1, d_id, 1);
  g.connectAudio(g.INPUT_ID, 2, d_id, 2);
  g.connectAudio(d_id, 0, g.OUTPUT_ID, 0);

  cout << g.toDebugString() << endl;

  return 0;
}
