#include "audiolib/Graph.h"
#include "audiolib/Iframes.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include <cassert>


namespace audiolib{

  /**
   * PortPair
   */
  bool PortPair::operator==(const PortPair & other){
    return node_id_ == other.node_id_ && port_ == other.port_;
  }

  /**
   * PortConnections
   */
  bool PortConnections::removeConnectionToNode(int node_id){
    bool flag = true;
    bool deleted_something = false;

    iterator it_low = begin();
    iterator it_up = end();

    while (flag){
      flag = false;
      for (auto it = it_low; it!=it_up && !flag; it++){
        if (it->second.node_id_ == node_id){
          erase(it);
          flag = true;
          deleted_something = true;
        }
      }
    }
    return deleted_something;
  }

  bool PortConnections::removeConnection(int local_port, const PortPair & pair){
    bool deleted_something = false;
    bool flag = true;

    iterator it_low = lower_bound(local_port);
    iterator it_up = upper_bound(local_port);

    while (flag){
      flag = false;
      for (auto it = it_low; it!=it_up && !flag; it++){
        if (it->second == pair){
          erase(it);
          flag = true;
          deleted_something = true;
        }
      }
    }

    return deleted_something;
  }

  bool PortConnections::isConnected(int local_port, const PortPair & pair){
    iterator it_low = lower_bound(local_port);
    iterator it_up = upper_bound(local_port);
    bool is_connected = false;
    for (auto it = it_low; it!=it_up; it++){
      if (it->second == pair){
        is_connected = true;
      }
    }
    return is_connected;
  }

  bool PortConnections::isConnected(int local_port){
    return count(local_port) != 0;
  }

  void PortConnections::connect(int local_port, const PortPair & pair){
    //assumes that the connection doesn't already exist
    insert(value_type(local_port, pair));
  }


  /**
   * NodeWrapper
   */

  NodeWrapper::NodeWrapper(std::unique_ptr<Node> && node, const Iframes * default_input_frames) :
    node_(std::move(node)),
    input_buffer_(node_->getNumAudioInputs(), default_input_frames)
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
    id_counter_(FIRST_EXTERNAL_NODE_ID),
    null_audio_frames_(0.0, block_size, 1),
    internal_output_buffer_(n_outputs, block_size, sample_rate),
    external_output_buffer_(internal_output_buffer_)
  {
    // register the dummy nodes for the input and output
    DummyNode input(mkNodeSettings(sample_rate, block_size, 0, n_inputs));
    DummyNode output(mkNodeSettings(sample_rate, block_size, n_outputs, 0));
    node_map_.emplace(INPUT_ID,
        NodeWrapper(std::unique_ptr<Node>(&input), &null_audio_frames_));
    node_map_.emplace(OUTPUT_ID,
        NodeWrapper(std::unique_ptr<Node>(&output), &null_audio_frames_));
  }


  Graph::~Graph()
  {
  }


  void Graph::validate()
  {
    //graph should be valid due to invariants during construction.
    //simply call validate on all children
    for (auto& pair: node_map_){
      NodeWrapper & nw = pair.second;
      nw.node_->validate();
    }
  }


  int Graph::registerNode(std::unique_ptr<Node> && node)
  {
    if (node->getBlockSize() != getBlockSize()){
      //TODO: raise an error
    }
    if (node->getSampleRate() != getSampleRate()){
      //TODO: raise an error
    }
    int id = id_counter_++;
    node_map_.emplace(id, NodeWrapper(std::move(node), &null_audio_frames_));
    return id;
  }

  std::unique_ptr<Node> Graph::deregisterNode(int id){
    requireNode(id);
    if (id == INPUT_ID || id == OUTPUT_ID){
      //TODO: raise an error
    }
    NodeWrapper& nw = node_map_.at(id);
    std::unique_ptr<Node> node_ptr = move(nw.node_);
    node_map_.erase(id);
    for(auto& pair: node_map_){
      NodeWrapper& nw2 = pair.second;
      nw2.output_audio_connections_.removeConnectionToNode(id);
      nw2.input_audio_connections_.removeConnectionToNode(id);
    }
    return node_ptr;
  }


  void Graph::connectAudio(const PortPair & source, const PortPair & sink){
    requireNode(source.node_id_);
    requireNode(sink.node_id_);
    NodeWrapper & source_nw = node_map_.at(source.node_id_);
    NodeWrapper & sink_nw = node_map_.at(sink.node_id_);
    //make sure the sink input port is not already connected to something (else)
    if (sink_nw.input_audio_connections_.isConnected(sink.port_)){
      //TODO: raise an error
    }
    //make sure the source and sink are not already connected
    if (sink_nw.input_audio_connections_.isConnected(sink.port_, source)){
      //TODO: raise an error
    }
    //make sure the source port is valid
    if (source.port_ >= source_nw.node_->getNumAudioOutputs() ||
        source.port_ < 0){
      //TODO: raise an error
    }
    //make sure the sink port is valid
    if (sink.port_ >= sink_nw.node_->getNumAudioInputs() ||
        sink.port_ < 0){
      //TODO: raise an error
    }
    sink_nw.input_audio_connections_.connect(sink.port_, source);
    source_nw.output_audio_connections_.connect(source.port_, sink);
  }

  void Graph::disconnectAudio(const PortPair & source, const PortPair & sink){
    requireNode(source.node_id_);
    requireNode(sink.node_id_);
    NodeWrapper & source_nw = node_map_.at(source.node_id_);
    NodeWrapper & sink_nw = node_map_.at(sink.node_id_);
    //make sure the source and sink are connected
    if (!sink_nw.input_audio_connections_.isConnected(sink.port_, source)){
      //TODO: raise an error
    }
    sink_nw.input_audio_connections_.removeConnection(sink.port_, source);
    source_nw.output_audio_connections_.removeConnection(source.port_, sink);
    sink_nw.input_buffer_[sink.port_] = &null_audio_frames_;
  }


  std::string Graph::toString() const
  {
    return "Graph " + id_;
  }

  std::string Graph::toDescriptionString() const
  {
    std::stringstream ss;
    ss << "Connections:\n";
    for (int id: sorted_node_list_){
      const NodeWrapper & src_nw = node_map_.at(id);
      std::string src_word = src_nw.node_->toString();
      for (auto& pair2: src_nw.output_audio_connections_){
        int src_port = pair2.first;
        const PortPair & pair3 = pair2.second;
        const NodeWrapper & dest_nw = node_map_.at(pair3.node_id_);
        std::string dest_word = dest_nw.node_->toString();
        int dest_port = pair3.port_;
        ss << "  " << src_word << " p " << src_port;
        ss << " -> " << dest_word << " p " << dest_port << "\n";
      }
    }
    ss << "Children:\n";
    for (auto& id: sorted_node_list_){
      const NodeWrapper & nw = node_map_.at(id);
      ss << "  " << id << " -> " << nw.node_->toString() << "\n";
      ss << indentString(nw.node_->toDescriptionString(), 4);
    }
    return ss.str();
  }

}
