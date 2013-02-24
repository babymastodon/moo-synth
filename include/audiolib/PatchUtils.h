#ifndef AUDIOLIB_PATCH_UTILS_H
#define AUDIOLIB_PATCH_UTILS_H

#include "audiolib/Patch.h"
#include "audiolib/Message.h"
#include <functional>
#include <string>
#include <vector>
#include "stk/RtMidi.h"

namespace audiolib{
  
  typedef std::function<void(Patch &, int, const Message &, SendMessageCallback &)> ProcessMessageCallback;

  /**
   * FunctionalPatch
   *
   * Sends all incoming messages through a custom function
   * which is defined during instantiation
   */
  class FunctionalPatch : public Patch{
    public:
      explicit FunctionalPatch(const char * name, ProcessMessageCallback &);
      explicit FunctionalPatch(const std::string & name, ProcessMessageCallback &);

      /**
       * Copy constructors
       */
      explicit FunctionalPatch(const char * name, FunctionalPatch &);
      explicit FunctionalPatch(const std::string & name, FunctionalPatch &);

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
  class PassthroughPatch : public Patch{
    public:
      //TODO: use constructor inheritance when it gets released
      PassthroughPatch(const char * name) : Patch(name) {}
      PassthroughPatch(const std::string & name) : Patch(name) {}
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
  class JunctionPatch : public Patch{
    public:
      //TODO: use constructor inheritance when it gets released
      JunctionPatch(const char * name) : Patch(name) {}
      JunctionPatch(const std::string & name) : Patch(name) {}

    private:
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
  };


  /**
   * MidiReaderPatch
   *
   * Converts Midi messages into internal messages.
   * Eg: NoteMessage, ControlMessage
   * Input/Output on all ports
   */
  class MidiReaderPatch : public Patch{
    public:
      //TODO: use constructor inheritance when it gets released
      MidiReaderPatch(const char * name) : Patch(name) {}
      MidiReaderPatch(const std::string & name) : Patch(name) {}

    private:
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
  };


  /**
   * MidiWriterPatch
   *
   * Converts internal messages into MidiMessages
   * Eg: NoteMessage, ControlMessage
   * Input/Output on all ports
   */
  class MidiWriterPatch : public Patch{
    public:
      //TODO: use constructor inheritance when it gets released
      MidiWriterPatch(const char * name) : Patch(name) {}
      MidiWriterPatch(const std::string & name) : Patch(name) {}

    private:
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
  };


  /**
   * VirtualMidiInputPatch
   *
   * Reads Midi Message from a virtual MIDI port
   * created by RtMidi (linux and mac only)
   * Output on all ports
   */
  class VirtualMidiInputPatch : public Patch{
    public:
      //TODO: use constructor inheritance when it gets released
      VirtualMidiInputPatch(const char * name);
      VirtualMidiInputPatch(const std::string & name);

    private:
      RtMidiIn midi_in_;
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
      static void callback(double, std::vector<unsigned char> *, void *);
  };

}

#endif
