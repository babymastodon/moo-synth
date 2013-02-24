#include "audiolib/Message.h"
#include <sstream>

namespace audiolib{

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

  std::string MidiMessage::toString() const {
    std::stringstream ss;
    ss << "MidiMessage: ";
    for (auto c: value_){
      ss << std::hex << (int) c << ' ';
    }
    return ss.str();
  }

  std::string NoteMessage::toString() const {
    std::stringstream ss;
    ss << "NoteMessage: ch " << channel_ << " n " << note_ << " on " << on_;
    return ss.str();
  }

  std::string ControlMessage::toString() const {
    std::stringstream ss;
    ss << "ControlMessage: ch " << channel_ << " ctl " << controller_ << " val " << value_;
    return ss.str();
  }
}
