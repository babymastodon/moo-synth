#include "audiolib/Node.h"
#include "audiolib/Iframes.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include <cassert>


namespace audiolib{
  int Node::id_counter_ = 0;

  Node::Node(const NodeSettings & ps) :
    node_settings_(ps),
    id_(id_counter_++)
  {}

  DummyNode::DummyNode(const NodeSettings & ps):
    Node(ps),
    null_audio_frames_(0.0, getBlockSize(), 1),
    output_buffer_(getNumAudioOutputs(), &null_audio_frames_)
  {}

  const ConstIframesVector & DummyNode::computeAudio(const ConstIframesVector & inputs)
  {
    return output_buffer_;
  }
}
