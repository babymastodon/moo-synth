#include <iostream>
#include <signal.h>
#include "audiolib/PatchUtils.h"
#include "audiolib/Utils.h"
#include <functional>
#include "stk/RtMidi.h"

using namespace audiolib;
using namespace std;

int main( int argc, char *argv[]){

  ProcessMessageCallback c1 = [](Patch& self, int in_port, const Message & m, SendMessageCallback & c){
    cout << "Message Received: " << self.getName() << endl;
    cout << "Port: " << in_port << endl;
    cout << m.toString() << endl;
    cout << endl;
    return;
  };
  FunctionalPatch p1("OutputPatch", c1);

  JunctionPatch p2("Junction");
  PassthroughPatch p3("Passthrough");

  IntMessage i(1);

  p3.connectMessagePort(1, p2, 2);

  p2.connectMessagePort(1,p1,3);

  p3.receiveMessage(1, i);

  RtMidiIn midi_in;

  midi_in.openVirtualPort();

  while (1){
    MidiMessage m;
    midi_in.getMessage(&m.value_);

    if (m.value_.size() > 0){
      p3.receiveMessage(1, m);
    }

    sleep(100);
  }
}
