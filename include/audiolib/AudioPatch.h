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
   *  AudioSource
   *      Conceptually, it is an output buffer. An AudioInput
   *      can connect to it and pull a single batch of frame
   *      data. Internally, an AudioSource may or may not be
   *      a buffer -- it may compute the batch on demand (in
   *      the case of a synthesizer/instrument), or might pull
   *      it from an AudioSource higher (closer to the source)
   *      in the tree.
   *  AudioSink
   *      Conceptually, an input buffer. An AudioOutput can
   *      send it a single batch of frame data. The AudioSink
   *      might save the frame, or it might send it off to
   *      another AudioSink lower (closer to the output)
   *      in the tree.
   *  AudioInput
   *      Connects to the AudioSource of another node, and
   *      pulls new data when required. For example, a node
   *      might wrap around a DAC (sound card) thread. When
   *      the sound card's buffer needs more data, the node
   *      uses the "fetchAudioInput()" function to pull new
   *      frame data from the connected AudioSource.
   *  AudioOutput
   *      Connects to the AudioSink. Example: a ADC (microphone
   *      input thread) will trigger an event whenever it
   *      has a new, complete batch of audio data. A mic patch
   *      might wrap around this thread with its AudioOutput
   *      connected to an AudioSink of a buffer patch. Whenever
   *      the ADC triggers the new data event, the microphone
   *      patch will convert the data to an Iframe, and push
   *      it out of its AudioOutput into the AudioSink of the
   *      buffer patch.
   *
   * Basically, there are two types of connections:
   *  AudioSource -> AudioInput
   *  AudioOutput -> AudioSink
   *
   * In each case, there is a passive end (Source/Sink),
   *      and an active end (Input/Output). At a more fundemental
   *      level, the passive end can be thought of as a
   *      function object -- an AudioSource
   *      is a function that returns frame data, whereas
   *      an AudioSink is a function that takes frame
   *      data as an argument. The active end is a pointer
   *      to these functions. Suppose that an AudioInput I
   *      of a patch P is connected to the AudioSource S of
   *      another patch. In other words, I points to S. When
   *      the patch P needs more frame data, it uses I to
   *      find S, then directly calls the function S to
   *      get the new frame data.
   *
   * Invariants
   *  Both ends of a connection must be have the same
   *      AudioSettings.
   *  There is a one-to-one relationship between all
   *      connections
   *
   * Inputs/Outputs/Sources/Sinks are 0-indexed
   *
   * Subclasses should implement the following functions:
   *    Both constructors
   *    sourceAudioFrame (if applicable)
   *    sinkAudioFrame (if applicable)
   *    numAudioInputs (if applicable)
   *    numAudioOutputs (if applicable)
   *    numAudioSources (if applicable)
   *    numAudioSinks (if applicable)
   *
   * When two audio patches are connected, their sample rates
   *  are expected to match. Additionally, all inputs of a
   *  an AudioPatch are to be connected to a Source, and
   *  all outputs should be connected to a Sink. These invariants
   *  are checked by calling the validate function.
   *
   * */
  class AudioPatch : public Patch{
    public:

      /**
       * These constructors don't do much. Subclasses
       * are expected to implement their own constructors
       * that spec out the input/outut/source/sink
       * by using the "addXXX()" function.
       */
      AudioPatch(const char * s, int sample_rate = 44100) : Patch(s), sample_rate_(sample_rate){}
      AudioPatch(const std::string & s, int sample_rate = 44100) : Patch(s), sample_rate_(sample_rate){}
      /* Virtual destructor to stop memory leaks */
      virtual ~AudioPatch() {}

      /**
       * function connectAudioInput
       *
       * Connects the nth audio input of "this" to the mth audio
       * source of "other". The "AudioSettings" of the input and
       * the source must be equal. Additionally, n and m must be
       * valid (must refer to existing inputs/sources). Eg,
       * n < this.numAudioInputs() and m < other.numaAudioOutputs().
       * If an invarient is not met, then a runtime error is thrown.
       */
      void connectAudioInput(int n, AudioPatch & other, int m);
      void connectAudioOutput(int n, AudioPatch & other, int m);

      /**
       * Disconnects AudioInput/Output n from whatever its
       * connected to. If n is out of range, or if nothing
       * is connected to the port, then an exception is raised.
       */
      void disconnectAudioInput(int n);
      void disconnectAudioOutput(int n);

      virtual int numAudioInputs(){return 0;}
      virtual int numAudioOutputs(){return 0;}
      virtual int numAudioSources(){return 0;}
      virtual int numAudioSinks(){return 0;}

      /**
       * function validate()
       *
       * ensures that:
       *    all connected sources and sinks have
       *      the same sample rate
       *    all of the inputs on the current device
       *      are connected to a source
       *    all of the outputs on the current device
       *      are connected to a sink
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
       * set of frames from the AudioSource that is connected
       * to the nth AudioInput of this object. n is assumed
       * to be an index for an input. Otherwise the
       * behavior is undefined
       */
      const Iframes & fetchAudioInput(int n);

      /**
       * function pushAudioOutput(n)
       *
       * subclasses can use this function to send a
       * batch of frames to the AudioSink that is connected
       * to the nth AudioOutput of this object. n is assumed
       * to be an index for a non-null output. Otherwise the
       * behavior is undefined
       */
      void pushAudioOutput(int n, const Iframes & frames);
      

      /**
       * function fetchSampleRate(n)
       *
       * All connected AudioPatches are expected to have
       * this sample rate. Certain subclasses might have
       * secondary sample rates for internal use, but they
       * should use this one while communicating with other
       * patches.
       */
      int getSampleRate() {return sample_rate_;}
      void setSampleRate(int n) {sample_rate_ = n;}

    private:
      int sample_rate_;

      /**
       * keeps track of the mapping between the local
       * Input/Output ports and the external Sources
       * and Sinks belonging to other patches. The
       * nth index of the external_audio_sources_
       * vector corresponds to the AudioSource that
       * is connected to the nth AudioInput on this
       * object.
       *
       * if the pointer is null, then the port is not
       * connected
       */
      std::unordered_map<int,AudioPortPair> external_audio_sources_;
      std::unordered_map<int,AudioPortPair> external_audio_sinks_;

      /**
       * when something connects to a Source or a Sink,
       * it locks that resource, enforcing the one-to-
       * one relationship between ports
       *
       * if the pointer is null, then the port is not
       * connected
       */
      std::set<int> audio_source_locks_;
      std::set<int> audio_sink_locks_;

      /**
       * helper functions for printing error messages
       */
      void throw_audio_port_not_connected(const std::string & s, int n);
      void throw_audio_port_out_of_range(const std::string & s, int n);
      void throw_audio_port_occupied(const std::string & s, int n);


      /**
       * internal functions to lock and unlock the
       * Source and Sink ports
       *
       * lockXXX(N) tries to lock the specific port
       * belonging to this. (by flipping the corresponding
       * vector entry to true). If it fails, it returns false
       *
       * n is assumed to be a valid index
       */
      bool lockAudioSource(int n);
      bool lockAudioSink(int n);
      void unlockAudioSource(int n);
      void unlockAudioSink(int n);
  
      /**
       * To be implemented by sub-classes
       *
       * sourceAudioFrame(int n)
       * is supposed to return a Iframes object conforming
       * to the settings described inputSettings(n). "N" is
       * assumed to be an index of a valid source.
       * 
       * sinkAudioFrame(int n)
       * accepts an Iframes object containing frame data
       * directed towards port n. N is assumed to be
       * an index of a valid source
       *
       * default implementations raise runtime errors
       */
      const Iframes & sourceAudioFrames(int n);
      void sinkAudioFrames(int n, const Iframes & frames);
  };

}


#endif

