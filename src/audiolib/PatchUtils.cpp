#include "audiolib/PatchUtils.h"

namespace audiolib{

  /**
   * FunctionalPatch
   */
  FunctionalPatch::FunctionalPatch(const char * name, ProcessMessageCallback & callback)
    : Patch(name), callback_(callback) {}

  FunctionalPatch::FunctionalPatch(const std::string & name, ProcessMessageCallback & callback)
    : Patch(name), callback_(callback) {}

  FunctionalPatch::FunctionalPatch(const char * name, FunctionalPatch & other)
    : Patch(name), callback_(other.callback_) {}

  FunctionalPatch::FunctionalPatch(const std::string & name, FunctionalPatch & other)
    : Patch(name), callback_(other.callback_) {}


  void FunctionalPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    callback_(*this, in_port, m, send);
  }


  /**
   * PassthroughPatch
   */
  void PassthroughPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    send(in_port, m);
  }

  

  /**
   * JunctionPatch
   */
  void JunctionPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    for (auto out_port: getMessageOutputPorts()){
      send(out_port, m);
    }
  }

}
