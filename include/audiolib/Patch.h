#ifndef AUDIOLIB_PATCH_H
#define AUDIOLIB_PATCH_H

#ifdef MOO

#include "audiolib/Message.h"
#include <unordered_map>
#include <set>
#include <functional>
#include <string>

namespace audiolib{

  class Patch;

  struct PortPair{
    Patch& patch;
    int port;
    PortPair(Patch & a, int b) : patch(a), port(b) {}
  };

  typedef std::function<void(int, const Message &)> SendMessageCallback;


  /**
   * Note:
   * It doesn't make any sense for a patch
   * to be moved or copied. Things will explode
   * if the user tries to move or copy a patch
   * (eg, put it in an STL container).
   */

  class Patch{
    public:
      explicit Patch(const std::string & name);

      /* Virtual destructor to stop memory leaks */
      virtual ~Patch() {}

      /**
       * Direct all messages coming from out_port into the
       * in_port of patch. Note, there can only be one
       * connection coming out of an out_port, but multiple
       * connections can go into an in_port.
       */
      void connectMessagePort(int out_port, Patch & patch, int in_port);
      void unconnectMessagePort(int out_port);

      /**
       * receives a message. checks the TTL,
       * and then passes control to the processMessage
       * function
       */
      void receiveMessage(int in_port, const Message & m, int ttl=2048);

      const std::string & getName() { return name_;}

    protected:
      const std::set<int> & getMessageOutputPorts(){return message_output_ports_;}

    private:
      /**
       * A list of output ports on the current object that
       * are connected to something (technically this is almost
       * the same data as in message_output_map_, but this
       * set is provided for convenience)
       */
      std::set<int> message_output_ports_;

      /**
      * mapping between output ports on the current object
      * to (object, input port) pairs representing their
      * destinations
      */
      std::unordered_map<int, PortPair> message_output_map_;
      const std::string name_;

      static int id_counter;


      /**
       * Subclasses should define this function
       * for custom behavior.
       *
       * Warning: multiple threads can be executing
       * the processMessage function at the same time,
       * so subclasses should use mutexes when necessary
       */
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send){}


      /**
       * look up the port in the "message_output_map_"
       * and sends a message there. Subclasses should not
       * directly invoke this function. Instead, it gets
       * wrapped in a closure (to capture TTL) and gets
       * passed as an argument to processMessage()
       */
      void sendMessage(int out_port, const Message & m, int ttl);
  };

}

#endif

#endif
