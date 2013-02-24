#include <iostream>
#include <signal.h>
#include "audiolib/PatchUtils.h"
#include "audiolib/Utils.h"
#include <functional>
#include "stk/RtMidi.h"
#include <vector>

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
  MidiReaderPatch p2("Reader");
  VirtualMidiInputPatch p3("Midi Input");

  p3.connectMessagePort(1,p2,1);
  p2.connectMessagePort(1,p1,1);

  while (1){
    sleep(100);
  }
}
