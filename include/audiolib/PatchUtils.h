#ifndef AUDIOLIB_PATCH_UTILS_H
#define AUDIOLIB_PATCH_UTILS_H

#include "audiolib/Patch.h"
#include "audiolib/Message.h"
#include <functional>
#include <string>

namespace audiolib{

  typedef std::function<void(int, const Message &, SendMessageCallback &)> ProcessMessageCallback;

  /**
   * FunctionalPatch
   *
   * Sends all incoming messages through a custom function
   * which is defined during instantiation
   */
  class FunctionalPatch : Patch{
    public:
      explicit FunctionalPatch(const char * name, ProcessMessageCallback);
      explicit FunctionalPatch(const std::string & name, ProcessMessageCallback);

    private:
      const ProcessMessageCallback callback_;
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
  };


  /**
   * PassthroughPatch
   *
   * Passes the input ports directoy to the output
   * ports. This is especially useful for spliting
   * and combining signals within a sub-patch.
   * (once sub-patches get implemented)
   */
  class PassthroughPatch : Patch{
    private:
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
  };


  /**
   * JunctionPatch
   *
   * Redirects all input ports to all output
   * ports. Calls output ports in serial order,
   * low to high.
   */
  class JunctionPatch : Patch{
    private:
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
  };

}

#endif
