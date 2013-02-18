#include "PatchUtils.h"

namespace audiolib{

  /**
   * FunctionalPatch
   */
  FunctionalPatch::FunctionalPatch(const char * name, ProcessMessageCallback callback)
    : callback_(callback), Patch(name){}

  FunctionalPatch::FunctionalPatch(const string & name, ProcessMessageCallback callback)
    : callback_(callback), Patch(name){}

  FunctionalPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    callback_(in_port, m, send);
  }


  /**
   * PassthroughPatch
   */
  PassthroughPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    send(in_port, m);
  }

  

  /**
   * JunctionPatch
   */
  JunctionPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    for (auto out_port: getMessageOutputPorts()){
      send(out_port, m);
    }
  }

}
