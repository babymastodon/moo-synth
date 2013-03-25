#ifndef AUDIOLIB_AUDIO_GRAPH_H
#define AUDIOLIB_AUDIO_GRAPH_H

#include "audiolib/Node.h"
#include "audiolib/Iframes.h"
#include "audiolib/Utils.h"
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>


namespace audiolib{

  struct PortPair{
    int node_id_;
    int port_;
    PortPair(int node_id, int port): node_id_(node_id), port_(port) {}
    bool operator==(const PortPair & other);
  };

  struct PortConnections : public std::multimap<int, PortPair>{
    bool removeConnectionToNode(int node_id);
    bool removeConnection(int local_port, const PortPair & pair);
    bool isConnected(int local_port, const PortPair & pair);
    bool isConnected(int local_port);
    void connect(int local_port, const PortPair & pair);
  };

  struct NodeWrapper{
    std::unique_ptr<Node> node_;
    PortConnections output_audio_connections_;
    PortConnections input_audio_connections_;
    ConstIframesVector input_buffer_;

    NodeWrapper(std::unique_ptr<Node> && node, const Iframes * default_input_frames);
    NodeWrapper(NodeWrapper && other) = default;
    NodeWrapper& operator=(NodeWrapper && other) = default;
    NodeWrapper(const NodeWrapper & other) = delete;
    NodeWrapper& operator=(const NodeWrapper & other) = delete;
  };

  class Graph : public Node {
    public:

      Graph(const NodeSettings & ps);

      ~Graph();

      void validate() const;

      int registerNode(Node * node);
      int registerNode(std::unique_ptr<Node> && node);
      std::unique_ptr<Node> deregisterNode(int id);

      void connectAudio(const PortPair & source, const PortPair & sink);
      void connectAudio(int source_id, int source_port, int sink_id, int sink_port);
      void disconnectAudio(const PortPair & source, const PortPair & sink);
      void disconnectAudio(int source_id, int source_port, int sink_id, int sink_port);

      std::string toDescriptionString() const;

      virtual const ConstIframesVector & computeAudio(const ConstIframesVector & inputs);

      static const int INPUT_ID;
      static const int OUTPUT_ID;
      static const int FIRST_EXTERNAL_NODE_ID;

    private:

      int id_counter_;
      std::vector<int> sorted_node_list_;
      std::unordered_map<int, NodeWrapper> node_map_;
      const Iframes null_audio_frames_;



      void recomputeNodeOrder();
      void requireNode(int id);


      static NodeSettings filterNodeSettings(const NodeSettings & ps);
      std::string className() const {return "GraphNode";}

  };


  class GraphInputNode : public Node {
    friend class Graph;

    protected:
      GraphInputNode(const NodeSettings & node_settings);

      ~GraphInputNode();
  };

}


#endif

