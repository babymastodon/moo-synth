#ifndef AUDIOLIB_AUDIO_NODE_H
#define AUDIOLIB_AUDIO_NODE_H

#include "audiolib/Node.h"
#include "audiolib/Iframes.h"
#include "audiolib/Utils.h"
#include <functional>
#include <vector>
#include <string>


namespace audiolib{

  struct NodeSettings{
      float sample_rate_;
      int block_size_;
      int num_audio_inputs_;
      int num_audio_outputs_;
      int num_message_inputs_;
      int num_message_outputs_;

      NodeSettings(): sample_rate_(1), block_size_(0), num_audio_inputs_(0),
        num_audio_outputs_(0), num_message_inputs_(0), num_message_outputs_(0){}
  };

  class Node{
    public:

      /* Subclasses are expected to initialize the internal NodeSettings object
       * upon construction by calling this superclass constructor.
       */
      Node(const NodeSettings & ps);

      /* Virtual destructor to stop memory leaks */
      virtual ~Node() {
        DEBUG("Node destructor " << getId())
      }

      /* disable moving and copying by default */
      Node(const Node & n) = delete;
      Node& operator=(const Node & n) = delete;
      Node(Node && n) = delete;
      Node& operator=(Node && n) = delete;

      const NodeSettings& getSettings() const {return settings_;}
      int getId() const {return id_;}

      int getNumAudioInputs() const {return settings_.num_audio_inputs_;}
      int getNumAudioOutputs() const {return settings_.num_audio_outputs_;}
      int getNumMessageInputs() const {return settings_.num_message_inputs_;}
      int getNumMessageOutputs() const {return settings_.num_message_outputs_;}
      int getSampleRate() const {return settings_.sample_rate_;}
      int getBlockSize() const {return settings_.block_size_;}

      std::string toString() const {return className() + " #" + std::to_string(getId());}
      std::string toDebugString() const {
        return toString() + "\n" + indentString(toDescriptionString(), 2);
      }
      virtual std::string toDescriptionString() const;

      /**
       * function validate()
       *
       * raises a RuntimeError with an error message
       *    if a check fails.
       */
      virtual void validate() const {return;}

      /**
       * function compute(inputs, outputs)
       *
       * Computes a vector of output Iframes based on a list
       * of input Iframes. The length of the input vector is guarenteed
       * to be equal to the value of getNumAudioInputs(). The output vector
       * should have length equal to numAudioOutputs(), and should
       * not contain NULL pointers. (output invariants only checked in debug mode)
       *
       * Each of the input and output Iframes should have 1 channel consisting
       * of getBlockSize() samples to be interpreted at a sample rate of
       * getSampleRate().
       *
       * The implementation of this function must guarentee that the return
       * vector and its contents exist and remain unchanged as long as the
       * input pointers remain valid. The input pointers are guarenteed to be valid for an
       * undefined amount of time between the return of computeAudio() and
       * the next invocation of computeAudio()
       *
       * Therefore, if the output depends entirely on the input (the input pointers
       * were coppied into the output vector), then the implementation should just
       * ensure that the return vector (which contains the pointers) exists and remains
       * unchanged until the next invocation of computeAudio()
       *
       * If the output consists of new Iframes pointers that were not in the input vector
       * (eg, they were computed/generated during the computeAudio() function), then
       * the implementation needs to guanentee that both the output vector of Iframe
       * pointers and the new Iframes exist and remain unchanged until the next
       * invocation of computeAudio()
       *
       * Here is a timing diagram:
       *
       *  computeAudio() invocation & input pointers valid
       *  computeAudio() return & output valid
       *  anything except for computeAudio(), setSampleRate(), and setBlockSize()
       *  output not valid
       *  input pointers not valid
       *  computeAudio()
       */
      virtual const ConstIframesVector & computeAudio(const ConstIframesVector & inputs) = 0;


    protected:

    private:
      const int id_;
      const NodeSettings settings_;

      static int id_counter_;

      virtual std::string className() const = 0;
  };



  class DummyNode : public Node{
    public:
      DummyNode(const NodeSettings & ps);

      const ConstIframesVector & computeAudio(const ConstIframesVector & inputs);

    private:
      const Iframes null_audio_frames_;
      const ConstIframesVector output_buffer_;

      std::string className() const {return "DummyNode";}
  };



  class AudioAdder: public Node{
    public:
      AudioAdder(const NodeSettings & ps);

      const ConstIframesVector & computeAudio(const ConstIframesVector & inputs);

    private:
      const IframesVector internal_output_buffer_;
      const ConstIframesVector external_output_buffer_;

      static NodeSettings filterNodeSettings(const NodeSettings & ps);
      std::string className() const {return "AudioAdder";}
  };
}


#endif

