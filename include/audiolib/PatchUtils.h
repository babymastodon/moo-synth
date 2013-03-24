#ifndef AUDIOLIB_PATCH_UTILS_H
#define AUDIOLIB_PATCH_UTILS_H

#ifdef MOO

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
      VirtualMidiInputPatch(const std::string & name);

    private:
      RtMidiIn midi_in_;
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
      static void callback(double, std::vector<unsigned char> *, void *);
  };


  /**
   * VirtualMidiOutputPatch
   *
   * Write Midi Messages to a virtual MIDI port
   * created by RtMidi (linux and mac only)
   * Input on all ports
   */
  class VirtualMidiOutputPatch : public Patch{
    public:
      //TODO: use constructor inheritance when it gets released
      VirtualMidiOutputPatch(const std::string & name);

    private:
      RtMidiOut midi_out_;
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
  };


  /**
   * MidiDemuxPatch
   *
   * Reads MIDI messages from all input ports.
   * Sends the MIDI message out the port corresponding
   * to the "channel" of the message. Only works certain
   * messages (noteon, noteoff, control change)
   */
  class MidiDemuxPatch : public Patch{
    public:
      //TODO: use constructor inheritance when it gets released
      MidiDemuxPatch(const std::string & name) : Patch(name) {}
    private:
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
  };
  

  /**
   * MidiMuxPatch
   *
   * Reads midi messages from all ports. If the
   * message is noteon/off or control change, it rewrites
   * the control number of the message to match
   * the in-port number.
   */
  class MidiMuxPatch : public Patch{
    public:
      //TODO: use constructor inheritance when it gets released
      MidiMuxPatch(const std::string & name) : Patch(name) {}
    private:
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
  };


  /**
   * MidiChannelSplitter
   *
   * Reads in MIDI messages. All MIDI messages
   * with channels (note on/off, control change)
   * get routed out port 0. All other messages
   * get routed out port 1.
   */
  class MidiChannelSplitter: public Patch{
    public:
      //TODO: use constructor inheritance when it gets released
      MidiChannelSplitter(const std::string & name) : Patch(name) {}
    private:
      virtual void processMessage(int in_port, const Message & m, SendMessageCallback & send);
  };
}

#endif

#endif
