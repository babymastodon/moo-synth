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
      output.value_[0] = status_bit;
      output.value_[1] = (unsigned char) note->note_;
      output.value_[2] = (unsigned char) note->velocity_;
      for (auto out_port: getMessageOutputPorts())
        send(out_port, output);
      return;
    }

    /* Case ControlMessage */
    const ControlMessage * ctl = dynamic_cast<const ControlMessage*> (&m);
    if (note){
      MidiMessage output;
      unsigned char status_bit = ((unsigned char)ctl->channel_) & 0xF0;
      status_bit |= 0xB0;
      output.value_[0] = status_bit;
      output.value_[1] = (unsigned char) ctl->control_;
      output.value_[2] = (unsigned char) ctl->value_;
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
    : Patch(name), midi_in_(RtMidi::UNSPECIFIED, name)
  {
    midi_in_.openVirtualPort(getName());
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

}
