#ifndef AUDIOLIB_MESSAGE_H
#define AUDIOLIB_MESSAGE_H

#include <string>
#include "audiolib/Iframes.h"

namespace audiolib{
  
  class Message{
    public:
      virtual std::string toString() const =0;
  };

  class IntMessage : public Message {
    public:
      int value_;

      IntMessage(int n) : value_(n) {}
      virtual std::string toString() const;
  };

  class FloatMessage : public Message {
    public:
      AudioFloat value_;

      FloatMessage(float n) : value_(n) {}
      virtual std::string toString() const;
  };

  class StringMessage : public Message{
    public:
      std::string value_;

      StringMessage(const char *s) : value_(s) {}
      StringMessage(const std::string &s) : value_(s) {}
      virtual std::string toString() const;
  };

  class MidiMessage : public Message{
    public:
      std::vector<unsigned char> value_;

      MidiMessage(){}
      MidiMessage(const std::vector<unsigned char> & value)
        : value_(value) {}
      virtual std::string toString() const;
  };

  class NoteMessage : public Message{
    public:
      int channel_;  
      AudioFloat note_; 
      bool on_;

      NoteMessage(int channel, AudioFloat note, bool on)
        : channel_(channel), note_(note), on_(on) {}
      NoteMessage(int channel, int note, bool on)
        : channel_(channel), note_((AudioFloat)note), on_(on) {}
      virtual std::string toString() const;
  };

  class ControlMessage : public Message{
    public:
      int channel_;
      int controller_;
      AudioFloat value_;

      ControlMessage(int channel, int controller, AudioFloat value)
        : channel_(channel), controller_(controller), value_(value) {}
      ControlMessage(int channel, int controller, int value)
        : channel_(channel), controller_(controller), value_((AudioFloat)value) {}
      virtual std::string toString() const;
  };

}

#endif
