#include "audiolib/Graph.h"
#include "audiolib/Iframes.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <cmath>


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

  NodeSettings Graph::filterNodeSettings(const NodeSettings & ps)
  {
    NodeSettings s;
    s.sample_rate_ = ps.sample_rate_;
    s.block_size_ = ps.block_size_;
    s.num_audio_inputs_ = ps.num_audio_inputs_;
    s.num_audio_outputs_ = ps.num_audio_outputs_;
    return s;
  }


  /**
   * Graph
   */
  const int Graph::INPUT_ID = 0;
  const int Graph::OUTPUT_ID = 1;
  const int Graph::FIRST_EXTERNAL_NODE_ID = 2;

  Graph::Graph(const NodeSettings & ps) :
    Node(filterNodeSettings(ps)),
    id_counter_(FIRST_EXTERNAL_NODE_ID),
    null_audio_frames_(0.0, getBlockSize(), 1)
  {
    DEBUG("Begin graph constructor " << getId())
    // register the dummy nodes for the input and output
    NodeSettings s = getSettings();

    s.num_audio_inputs_ = 0;
    s.num_audio_outputs_ = getNumAudioInputs();
    DummyNode * input = new DummyNode(s);

    s.num_audio_inputs_ = getNumAudioOutputs();
    s.num_audio_outputs_ = 0;
    DummyNode * output = new DummyNode(s);

    node_map_.emplace(INPUT_ID,
        NodeWrapper(std::unique_ptr<Node>(input), &null_audio_frames_));
    node_map_.emplace(OUTPUT_ID,
        NodeWrapper(std::unique_ptr<Node>(output), &null_audio_frames_));

    recomputeNodeOrder();
    DEBUG("End graph constructor " << getId())
  }


  Graph::~Graph()
  {
  }


  void Graph::validate() const
  {
    //graph should be valid due to invariants during construction.
    //simply call validate on all children
    for (auto& pair: node_map_){
      const NodeWrapper & nw = pair.second;
      nw.node_->validate();
    }
  }


  int Graph::registerNode(std::unique_ptr<Node> && node)
  {
    if (node->getBlockSize() != getBlockSize()){
      //TODO: raise an error
    }
    if (abs(node->getSampleRate() - getSampleRate()) < 1){
      //TODO: raise an error
    }
    int id = id_counter_++;
    node_map_.emplace(id, NodeWrapper(std::move(node), &null_audio_frames_));
    recomputeNodeOrder();
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
    recomputeNodeOrder();
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
    recomputeNodeOrder();
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
    recomputeNodeOrder();
  }


  std::string Graph::toDescriptionString() const
  {
    std::stringstream ss;
    ss << Node::toDescriptionString();
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
      std::string id_str;
      if (id == INPUT_ID){
        id_str = "INPUT_ID";
      } else if (id == OUTPUT_ID){
        id_str = "OUTPUT_ID";
      } else {
        id_str = std::to_string(id);
      }
      ss << "  " << id_str << " -> " << nw.node_->toString() << "\n";
      ss << indentString(nw.node_->toDescriptionString(), 4);
    }
    return ss.str();
  }


  const ConstIframesVector & Graph::computeAudio(const ConstIframesVector & inputs)
  {
    //TODO: add runtime assertions in debug mode
    
    NodeWrapper & input_nw = node_map_.at(INPUT_ID);
    NodeWrapper & output_nw = node_map_.at(OUTPUT_ID);

    // Read in the input frames
    for (auto& pair: input_nw.output_audio_connections_){
      PortPair & pp = pair.second;
      int out_port = pair.first;
      node_map_.at(pp.node_id_).input_buffer_.at(pp.port_) = inputs.at(out_port);
    }

    // Propegate the frames through the graph
    for (int id: sorted_node_list_){
      if (id == INPUT_ID || id == OUTPUT_ID){
        continue;
      }
      NodeWrapper & nw = node_map_.at(id);
      const ConstIframesVector & output_buffer = nw.node_->computeAudio(nw.input_buffer_);
      for (auto& pair: nw.output_audio_connections_){
        PortPair & pp = pair.second;
        int out_port = pair.first;
        node_map_.at(pp.node_id_).input_buffer_.at(pp.port_) = output_buffer.at(out_port);
      }
    }

    // Our result is stored in the input_buffer_ of the output node
    return output_nw.input_buffer_;
  }

  void Graph::recomputeNodeOrder()
  {
    //TODO: topological sort -- break ties based on dist from output
    std::vector<int> tmp;
    for (auto& pair: node_map_){
      tmp.push_back(pair.first);
    }
    sorted_node_list_ = std::move(tmp);
  }

  void Graph::requireNode(int id)
  {
    if (node_map_.count(id) != 1){
      //TODO: raise an error
    }
  }

}
