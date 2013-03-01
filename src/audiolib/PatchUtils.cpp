#include "audiolib/PatchUtils.h"

namespace audiolib{

  /**
   * FunctionalPatch
   */
  FunctionalPatch::FunctionalPatch(const char * name, ProcessMessageCallback & callback)
    : Patch(name), callback_(callback) {}

  FunctionalPatch::FunctionalPatch(const std::string & name, ProcessMessageCallback & callback)
    : Patch(name), callback_(callback) {}

  FunctionalPatch::FunctionalPatch(const char * name, FunctionalPatch & other)
    : Patch(name), callback_(other.callback_) {}

  FunctionalPatch::FunctionalPatch(const std::string & name, FunctionalPatch & other)
    : Patch(name), callback_(other.callback_) {}


  void FunctionalPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    callback_(*this, in_port, m, send);
  }


  /**
   * PassthroughPatch
   */
  void PassthroughPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    send(in_port, m);
  }

  

  /**
   * JunctionPatch
   */
  void JunctionPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    for (auto out_port: getMessageOutputPorts())
      send(out_port, m);
  }


  /**
   * MidiReaderPatch
   */
  void MidiReaderPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    const MidiMessage * m2 = dynamic_cast<const MidiMessage*> (&m);
    if (m2){
      const Message* output;
      if (m2->value_.size() == 0)
        return;
      unsigned char high_bit = (m2->value_[0] & 0xF0) >> 4;
      unsigned char low_bit = (m2->value_[0] & 0x0F);
      switch (high_bit) {
        case 0x8:   //note off
          output = new NoteMessage(low_bit, m2->value_[1], m2->value_[2], false);
          break;
        case 0x9:   //note on
          output = new NoteMessage(low_bit, m2->value_[1], m2->value_[2], true);
          break;
        case 0xB:   //control change
          output = new ControlMessage(low_bit, m2->value_[1], m2->value_[2]);
          break;
        default:
          return;
      }
      for (auto out_port: getMessageOutputPorts())
        send(out_port, *output);
      delete output;
      return;
    }
  }


  /**
   * MidiWriterPatch
   */
  void MidiWriterPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    /* Case NoteMessage */
    const NoteMessage * note = dynamic_cast<const NoteMessage*> (&m);
    if (note){
      MidiMessage output;
      unsigned char status_bit = ((unsigned char)note->channel_) & 0xF0;
      if (note->on_) // Note On has high nibble = 9
        status_bit |= 0x90;
      else          // Note Off has high nibble = 8
        status_bit |= 0x80;
      output.value_.push_back(status_bit);
      output.value_.push_back((unsigned char) note->note_);
      output.value_.push_back((unsigned char) note->velocity_);
      for (auto out_port: getMessageOutputPorts())
        send(out_port, output);
      return;
    }

    /* Case ControlMessage */
    const ControlMessage * ctl = dynamic_cast<const ControlMessage*> (&m);
    if (ctl){
      MidiMessage output;
      unsigned char status_bit = ((unsigned char)ctl->channel_) & 0xF0;
      status_bit |= 0xB0;
      output.value_.push_back(status_bit);
      output.value_.push_back((unsigned char) ctl->control_);
      output.value_.push_back((unsigned char) ctl->value_);
      for (auto out_port: getMessageOutputPorts())
        send(out_port, output);
      return;
    }
  }


  /**
   * VirtualMidiInputPatch
   */
  VirtualMidiInputPatch::VirtualMidiInputPatch(const char * name)
    : VirtualMidiInputPatch(std::string(name)) {}

  VirtualMidiInputPatch::VirtualMidiInputPatch(const std::string & name)
    : Patch(name), midi_in_(RtMidi::UNSPECIFIED, "Cool MIDI")
  {
    midi_in_.openVirtualPort(name);
    midi_in_.setCallback(VirtualMidiInputPatch::callback, this);
  }

  void VirtualMidiInputPatch::callback(double delay, std::vector<unsigned char> * message, void * userdata){
    VirtualMidiInputPatch & self = *(VirtualMidiInputPatch*) userdata;
    MidiMessage m(*message);
    self.receiveMessage(1,m);
  }

  void VirtualMidiInputPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    for (auto out_port: getMessageOutputPorts())
      send(out_port, m);
  }


  /**
   * VirtualMidiOutputPatch
   */
  VirtualMidiOutputPatch::VirtualMidiOutputPatch(const char * name)
    : VirtualMidiOutputPatch(std::string(name)) {}

  VirtualMidiOutputPatch::VirtualMidiOutputPatch(const std::string & name)
    : Patch(name), midi_out_(RtMidi::UNSPECIFIED, "Cool MIDI")
  {
    midi_out_.openVirtualPort(name);
  }

  void VirtualMidiOutputPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    const MidiMessage * mm = dynamic_cast<const MidiMessage*> (&m);
    if (mm){
      /* unfortunately, RtMidiOut.sendMessage doesn't accept const parameters
       * (who knows why?)
       */
      std::vector<unsigned char> output = mm->value_;
      midi_out_.sendMessage(&output);
    }
  }


  /**
   * MidiDemuxPatch
   */
  void MidiDemuxPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    /* Note messages */
    const NoteMessage * note = dynamic_cast<const NoteMessage*> (&m);
    if (note){
      send(note->channel_, *note);
      return;
    }
    /* Control messages */
    const ControlMessage * control = dynamic_cast<const ControlMessage*> (&m);
    if (control){
      send(control->channel_, *control);
      return;
    }
  }


  /**
   * MidiMuxPatch
   */
  void MidiMuxPatch::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    /* Note messages */
    const NoteMessage * note = dynamic_cast<const NoteMessage*> (&m);
    if (note){
      NoteMessage n = *note;
      n.channel_ = in_port;
      for (auto out_port: getMessageOutputPorts())
        send(out_port, n);
      return;
    }
    /* Control messages */
    const ControlMessage * control = dynamic_cast<const ControlMessage*> (&m);
    if (control){
      ControlMessage c = *control;
      c.channel_ = in_port;
      for (auto out_port: getMessageOutputPorts())
        send(out_port, c);
      return;
    }
  }


  /**
   * MidiChannelSplitter
   */
  void MidiChannelSplitter::processMessage(int in_port, const Message & m, SendMessageCallback & send){
    /* Note messages */
    const NoteMessage * note = dynamic_cast<const NoteMessage*> (&m);
    if (note){
      send(0, *note);
      return;
    }
    /* Control messages */
    const ControlMessage * control = dynamic_cast<const ControlMessage*> (&m);
    if (control){
      send(0, *control);
      return;
    }
    send(0, m);
  }
}
