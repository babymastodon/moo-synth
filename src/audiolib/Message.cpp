#include "audiolib/Message.h"
#include <sstream>

namespace audiolib{

  Message::Message(MessageType t) : type_(t){
  }


  std::string IntMessage::toString() const {
    std::stringstream ss;
    ss << "IntMessage: " << value_;
    return ss.str();
  }

  std::string FloatMessage::toString() const {
    std::stringstream ss;
    ss << "FloatMessage: " << value_;
    return ss.str();
  }
  
  std::string StringMessage::toString() const {
    std::stringstream ss;
    ss << "StringMessage: " << value_;
    return ss.str();
  }
}
