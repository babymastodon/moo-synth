#include "audiolib/Graph.h"
#include "audiolib/Iframes.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include <cassert>


namespace audiolib{

  /**
   * NodeWrapper
   */
  
  NodeWrapper::NodeWrapper(std::unique_ptr<Node> && node) :
    node_(std::move(node)),
    input_buffer_(node_->getNumAudioInputs(), node_->getBlockSize(), node_->getSampleRate())
  {
  }


  /**
   * NodeSettings
   */

  NodeSettings Graph::mkNodeSettings(int sample_rate, int block_size,
      int n_inputs, int n_outputs)
  {
    NodeSettings s;
    s.sample_rate_ = sample_rate;
    s.block_size_ = block_size;
    s.num_audio_inputs_ = n_inputs;
    s.num_audio_outputs_ = n_outputs;
    return s;
  }


  /**
   * Graph
   */

  Graph::Graph(int sample_rate, int block_size,
      int n_inputs, int n_outputs) :
    Node(mkNodeSettings(sample_rate, block_size, n_inputs, n_outputs)),
    null_frames_(0.0, block_size, 1),
    internal_output_buffer_(n_outputs, block_size, sample_rate),
    external_output_buffer_(internal_output_buffer_)
  {
  }


  Graph::~Graph()
  {
  }


  void Graph::validate()
  {
  }


  int Graph::registerNode(std::unique_ptr<Node> && node)
  {
    int id = node->getId();
    node_map_.emplace(id, NodeWrapper(move(node)));
    return id;
  }

}
