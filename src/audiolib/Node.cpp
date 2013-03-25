#include "audiolib/Node.h"
#include "audiolib/Iframes.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include <cassert>


namespace audiolib{

  /**
   * Node
   */
  int Node::id_counter_ = 0;

  Node::Node(const NodeSettings & ps) :
    id_(id_counter_++),
    settings_(ps)
  {
    //TODO: ensure non-negative counts. positive sample rate
  }

  std::string Node::toDescriptionString() const
  {
    std::stringstream ss;
    ss << "Settings:\n";
    if (getNumAudioInputs() > 0)
      ss << "  Audio Inputs: " << getNumAudioInputs() << "\n";
    if (getNumAudioOutputs() > 0)
      ss << "  Audio Outputs: " << getNumAudioOutputs() << "\n";
    if (getNumAudioInputs() > 0 || getNumAudioOutputs() > 0){
      ss << "  Sample Rate: " << getSampleRate() << "\n";
      ss << "  Block Size: " << getBlockSize() << "\n";
    }
    if (getNumMessageInputs() > 0)
      ss << "  Message Inputs: " << getNumMessageInputs() << "\n";
    if (getNumMessageOutputs() > 0)
      ss << "  Message Outputs: " << getNumMessageOutputs() << "\n";
    return ss.str();
  }

  /**
   * DummyNode
   */
  DummyNode::DummyNode(const NodeSettings & ps):
    Node(ps),
    null_audio_frames_(0.0, getBlockSize(), 1),
    output_buffer_(getNumAudioOutputs(), &null_audio_frames_)
  {
    DEBUG("Dummy constructor " << getId())
  }

  const ConstIframesVector & DummyNode::computeAudio(const ConstIframesVector & inputs)
  {
    return output_buffer_;
  }


  /**
   * AudioAdder
   */
  AudioAdder::AudioAdder(const NodeSettings & ps):
    Node(filterNodeSettings(ps)),
    internal_output_buffer_(1, getBlockSize(), getSampleRate()),
    external_output_buffer_(internal_output_buffer_)
  {}

  NodeSettings AudioAdder::filterNodeSettings(const NodeSettings & ps)
  {
    NodeSettings s;
    s.sample_rate_ = ps.sample_rate_;
    s.block_size_= ps.block_size_;
    s.num_audio_inputs_ = ps.num_audio_inputs_;
    s.num_audio_outputs_ = 1;
    return s;
  }

  const ConstIframesVector & AudioAdder::computeAudio(const ConstIframesVector & inputs)
  {
    Iframes & frames = *internal_output_buffer_.at(0);
    int block_size = getBlockSize();
    int num_audio_inputs = getNumAudioInputs();
    for (int i=0; i<block_size; i++){
      // zero the output buffer
      frames[i] = 0;
      for (int j=0; j<num_audio_inputs; j++){
        frames[i] += (*inputs.at(j))[i];
      }
    }
    return external_output_buffer_;
  }
}
