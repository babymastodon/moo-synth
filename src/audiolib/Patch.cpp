#include "audiolib/Patch.h"
#include <stdexcept>
#include <stringstream>
#include <string>
#include <utilities>
#include <stdexcept>

namespace audiolib{

  Patch::Patch(const char * name) name_(name){
  }

  Patch::Patch(const string & name): name_(name){
  }

  Patch::~Patch(){
    // free the message_output_map_
    // Destructs all of the int and "PortPair" objects
    // in the map. The PortPair struct does not have
    // an explicit destructor, so this will not recursively destruct
    // the Patch (as expected)
    delete message_output_map_;

    // free the list of output ports
    delete message_output_ports_;

    // Free the name_ string
    delete name_;
  }

  void Patch::connectMessagePort(int out_port, Patch & patch, int in_port){
    if (message_output_map_.count(out_port) == 1){
      std::stringstream s;
      s << "Cannot make connection. " << name_ << " message port ";
      s << out_port << " is already connected.";
      throw std::runtime_error(s.str());
    }

    // Add the patch and port number to the map
    PortPair pair(patch, in_port);
    message_output_map_.insert(
        message_output_map.value_type(
          out_port,pair));

    // Add the output port to the list of output ports
    message_output_ports_.insert(out_port);
  }

  void Patch::unconnectMessagePort(int out_port){
    if (message_output_map_.count(out_port) == 0){
      std::stringstream s;
      s << "Cannot disconnect. " << name_ << " message port ";
      s << out_port << " is not connected.";
      throw std::runtime_error(s.str());
    }

    pair.port = in_port;
    message_output_map_.erase(out_port);
    message_output_ports_.erase(out_port);
  }


  void Patch::sendMessage(int out_port, const Message & m, int ttl){
    // if the specified output port is not connected, 
    // then nothing happens
    if (message_outpus_map_.count(out_port) != 0){
      PortPair p = message_output_map_.at(out_port);
      p.patch.process_message(p.port, m);
    }
  }

  void Patch::receiveMessage(int in_port, const Message & m, int ttl=2048){
    // If the ttl is zero, then this message has been stuck in
    // an infinite loop. Raise an error
    if (ttl == 0){
      std::stringstream s;
      s << "Message with ttl == 0 detected. Your patch bay might ";
      s << "have an infinite loop";
      throw std::runtime_error(s.str());
    }
    ttl --;

    // Create closure that caputures the current ttl
    // and wraps the "sendMessage" function
    SendMessageCallback callback = [ttl, this](int out_port, const Message & m_prime){
      sendMessage(out_port, m_prime, ttl);
    };

    // Pass control to the subclass-defined "processMessage"
    processMessage(in_port, m, callback);
  }
}
