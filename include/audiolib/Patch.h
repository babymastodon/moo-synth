#ifndef AUDIOLIB_PATCH_H
#define AUDIOLIB_PATCH_H

#include "Message.h"
#include <unordered_map>
#include <functional>
#include <string>

namespace audiolib{

  struct PortPair{
    Patch& patch;
    int port;
  }

  typedef std::functional<void(int, const Message &)> SendMessageCallback;

  class Patch{
    public:
      explicit Patch(const char * name)
      explicit Patch(const string & name)

      ~Patch()

      // Direct all messages coming from out_port into the
      // in_port of patch. Note, there can only be one
      // connection coming out of an out_port, but multiple
      // connections can go into an in_port.
      void connectMessagePort(int out_port, Patch & patch, int in_port)
      void unconnectMessagePort(int out_port)

      // receives a message. checks the TTL,
      // and then passes control to the processMessage
      // function
      void receiveMessage(int in_port, const Message & m, int ttl)

    protected:
      const set<int> & getMessageOutputPorts(){return message_output_ports;}

    private:
      set<int> message_output_ports_;
      unordered_map<int, PortPair> message_output_map_;
      const string & name_;

      static int id_counter;


      // Subclasses should define this function
      // for custom behavior.
      //
      // Warning: multiple threads can be executing
      // the processMessage function at the same time,
      // so subclasses should use mutexes when necessary
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send){}


      // look up the port in the "message_output_map_"
      // and sends a message there. Subclasses should not
      // directly invoke this function. Instead, it gets
      // wrapped in a closure (to capture TTL) and gets
      // passed as an argument to processMessage()
      void sendMessage(int out_port, const Message & m, int ttl)


      // It doesn't make any sense for a patch
      // to be moved or copied. Things will explode
      // if the user tries to move or copy a patch
      // (eg, put it in an STL container). So,
      // we make these constructors private so that the
      // compiler throws an error
      Patch& operator=(const Patch & other){}
      Patch(const Patch && other){}
      Patch(const Patch & other){}

  };

}

#endif
