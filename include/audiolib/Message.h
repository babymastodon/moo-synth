#ifndef AUDIOLIB_MESSAGE_H
#define AUDIOLIB_MESSAGE_H

#include <string>

namespace audiolib{
  
  // Identifiers for the various Message classes.
  // Processors are expected to check the message type
  // and cast it to the appropriate subclass.
  typedef enum {
    // The Midi enum values correspond directly to the
    // start nibble in the specification. Also, the top
    // three bytes are set to 0
    NoteOff=0x8, NoteOn, AfterTouch, ControlChange,
    ProgramChange,
    ChannelPressure,
    PitchWheel,

    // Number enum values have the second lsb set to 1
    // TODO: deal with more data types
    IntData=0x10,
    FloatData=0x11,

    // String enum values have the second lsb set to 2
    StringData=0x20

  } MessageType;

  class Message{
    public:
      const MessageType type_;

      explicit Message(MessageType t);
      
      bool isMidi() const {return (type_&0xFFF0) == 0;}
      bool isNumber() const {return (type_&0xFFF0) == 1;}
      bool isInt() const {return type_ == IntData;}
      bool isFloat() const {return type_ == FloatData;}
      bool isString() const {return type_ == StringData;}

    private:
  };

  class IntMessage : public Message {
    public:
      int value_;

      IntMessage(int n) : Message(IntData) {value_ = n;}
  };

  class FloatMessage : public Message {
    public:
      float value_;

      FloatMessage(float n) : Message(FloatData) {value_ = n;}
  };

  class StringMessage : public Message{
    public:
      std::string value_;

      StringMessage(const char *s) : Message(StringData) {value_ = s;}
      StringMessage(const std::string &s) : Message(StringData) {value_ = s;}
  };

}

#endif
