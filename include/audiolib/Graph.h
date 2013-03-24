#ifndef AUDIOLIB_AUDIO_GRAPH_H
#define AUDIOLIB_AUDIO_GRAPH_H

#include "audiolib/Node.h"
#include "audiolib/Iframes.h"
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
  };

  typedef std::multimap<int, PortPair> MultiConnections;
  typedef std::map<int, PortPair> SingleConnections;

  struct NodeWrapper{
    std::unique_ptr<Node> node_;
    MultiConnections forward_audio_connections_;
    SingleConnections reverse_audio_connections_;
    IframesVector input_buffer_;

    NodeWrapper(std::unique_ptr<Node> && node);
  };

  class Graph : public Node {
    public:

      Graph(int sample_rate = 44100, int block_size = 64,
          int n_inputs=0, int n_outputs=0);

      ~Graph();

      static NodeSettings mkNodeSettings(int sample_rate, int block_size,
          int n_inputs, int n_outputs);

      void validate();

      int registerNode(std::unique_ptr<Node> && node);
      std::unique_ptr<Node> deregisterNode(int id);

      void connectAudio(int source_id, int source_port, int sink_id, int sink_port);
      void disconnectAudio(int source_id, int source_port, int sink_id, int sink_port);

      void connectMessage(int source_id, int source_port, int sink_id, int sink_port);
      void disconnectMessage(int source_id, int source_port, int sink_id, int sink_port);

      const Node & getNode(int node) const;
      std::string toString() const;

      virtual const ConstIframesVector & computeAudio(const ConstIframesVector & inputs);


    private:

      std::vector<int> sorted_node_list_;
      std::unordered_map<int, NodeWrapper> node_map_;

      ConstIframesVector external_output_buffer_;
      IframesVector internal_output_buffer_;

      Iframes null_frames_;


      void sortNodes();
  };

}


#endif

