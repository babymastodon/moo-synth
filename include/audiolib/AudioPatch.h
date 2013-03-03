#ifndef AUDIOLIB_AUDIO_PATCH_H
#define AUDIOLIB_AUDIO_PATCH_H

#include "audiolib/Patch.h"
#include "audiolib/Iframes.h"
#include <functional>
#include <vector>
#include <string>


namespace audiolib{

  class AudioPatch;

  struct AudioPortPair {
    AudioPatch & patch;
    int port;
    AudioPortPair(AudioPatch & a, int b) : patch(a), port(b) {}
  };


  /**
   * AudioPatch
   *
   * Base class for all audio-processing classes. (buffers,
   * mic inputs, outputs, instruments, filters, resamplers,
   * etc.)
   *
   * An audio patch can have 4 types of IO ports:
   *  AudioOutput
   *      Conceptually, it is an output buffer. An AudioInput
   *      can connect to it and pull a single batch of frame
   *      data.
   *  AudioInput
   *      Connects to the AudioOutput of another node, and
   *      pulls new data when required. For example, a node
   *      might wrap around a DAC (sound card) thread. When
   *      the sound card's buffer needs more data, the node
   *      uses the "fetchAudioInput()" function to pull new
   *      frame data from the connected AudioOutput.
   *
   * Basically, there are two types of connections:
   *  AudioOutput -> AudioInput
   *
   * Invariants
   *  Both ends of a connection must be have the same
   *      AudioSettings.
   *
   * Inputs/Outputs are 0-indexed
   *
   * Subclasses should implement the following functions:
   *    constructor
   *    numAudioInputs (if applicable)
   *    numAudioOutputs (if applicable)
   *
   * When two audio patches are connected, their sample rates
   *  and block size are expected to match. Additionally, all
   *  inputs of an AudioPatch shold be connected to an Output.
   *  These invariants are checked by calling the validate function.
   *
   * */
  class AudioPatch : public Patch{
    public:

      AudioPatch(const std::string & s, unsigned int sample_rate = 44100, unsigned int block_size = 64)
        : Patch(s), sample_rate_(sample_rate), block_size_(block_size), last_compute_cycle_(0){}
      /* Virtual destructor to stop memory leaks */
      virtual ~AudioPatch() {}

      /**
       * function connectAudioInput
       *
       * Connects the nth audio input of "this" to the mth audio
       * output of "other". n and m must be
       * valid (must refer to existing inputs/outputs). Eg,
       * n < this.numAudioInputs() and m < other.numaAudioOutputs().
       * If an invarient is not met, then a runtime error is thrown.
       */
      void connectAudioInput(int n, AudioPatch & other, int m);

      /**
       * Disconnects AudioInput/Output n from whatever its
       * connected to. If n is out of range, or if nothing
       * is connected to the port, then an exception is raised.
       */
      void disconnectAudioInput(int n);

      /**
       * function addAudioDependency
       *
       * Build a dependency graph between nodes. When
       * the compute() function is called on a particular
       * node, all of its parents are executed, and the
       * results are stored in the output buffers.
       */

      virtual int numAudioInputs(){return 0;}
      virtual int numAudioOutputs(){return 0;}

      /**
       * function validate()
       *
       * ensures that:
       *    all connected outputs have the same sample rate
       *    all of the inputs on the current device
       *      are connected to an output
       *
       * raises a RuntimeError with an error message
       *    if a check fails.
       */
      void validate();

    protected:
      /**
       * function fetchAudioInput(n)
       *
       * subclasses can use this function to read a new
       * set of frames from the AudioOutput that is connected
       * to the nth AudioInput of this object. n is assumed
       * to be an index for an input. Otherwise the
       * behavior is undefined
       */
      const Iframes & fetchAudioInput(int n);

      /**
       * function setAudioOutput(n, frames)
       *
       * subclasses can use this function to set
       * the nth audio output buffer equal to the given
       * frames. The frames are captured by pointer,
       * so don't delete/modify the iframes until the
       * next cycle.
       */
      void setAudioOutput(int n, const Iframes * frames);

      /**
       * function fetchSampleRate(n)
       *
       * All connected AudioPatches are expected to have
       * this sample rate. Certain subclasses might have
       * secondary sample rates for internal use, but they
       * should use this one while communicating with other
       * patches.
       */
      unsigned int getSampleRate() {return sample_rate_;}
      void setSampleRate(unsigned int n) {sample_rate_ = n;}
      unsigned int getBlockSize() {return block_size_;}
      void setBlockSize(unsigned int n) {block_size_ = n;}

      /**
       * recomputes all of the parents of this node (starts
       * a new cycle). Should only be used on the base
       * of the dependency graph.
       */
      void recomputeParents();

    private:
      int sample_rate_;
      int block_size_;

      int last_compute_cycle_;

      /**
       * keeps track of the mapping between the local
       * Input input ports and the output ports
       * belonging to other patches. The
       * nth index of the external_audio_outputs_
       * vector corresponds to the AudioOutput that
       * is connected to the nth AudioInput on this
       * object.
       *
       * if the pointer is null, then the port is not
       * connected
       */
      std::unordered_map<int,AudioPortPair> external_audio_outputs_;

      /**
       * helper functions for printing error messages
       */
      void throw_audio_port_not_connected(const std::string & s, int n);
      void throw_audio_port_out_of_range(const std::string & s, int n);
      void throw_audio_port_occupied(const std::string & s, int n);


      /**
       * function compute(cycle)
       *
       * If this node has not yet been computed in the
       * current cycle, then call compute(cycle) on all
       * of the parent dependencies, and call internalCompute()
       */
      void compute(int id);

      /**
       * to be implemented by subclass
       *
       * This gets executed after all of the current patch's
       * dependencies are executed. The subclass is expected
       * to use the "fetchAudioInput(n)" and "setAudioOutput(n, frames)"
       * to fetch and save the frame data.
       */
      void internalCompute();

  };

}


#endif

