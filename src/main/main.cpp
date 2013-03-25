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
  s.num_audio_inputs_ = 0;
  s.num_audio_outputs_ = 1;

  Graph n(s);

  cout << n.toDebugString() << endl;

  return 0;
}
