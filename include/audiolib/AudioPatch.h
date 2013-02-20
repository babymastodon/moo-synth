#ifndef AUDIOLIB_AUDIO_PATCH_H
#define AUDIOLIB_AUDIO_PATCH_H

#include "audiolib/Patch.h"
#include <functional>
#include <vector>
#include <string>


namespace audiolib{

  struct AudioSettings {
    int n_frames;
    int n_channels;
    int sample_rate;
  }

  struct AudioPortPair {
    AudioPatch & patch;
    int port;
    AudioPortPair(Patch & a, int b){patch=a, port=b}
  }


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
   *
   * */
  class AudioPatch : Patch{
    public:

      /**
       * These constructors don't do much. Subclasses
       * are expected to implement their own constructors
       * that spec out the input/outut/source/sink
       * by using the "addXXX()" function.
       */
      AudioPatch(const char * s) : Patch(s){}
      AudioPatch(const string & s) : Patch(s){}
      ~AudioPatch()

      int numAudioInputs() {return audio_input_settings_.size();}
      int numAudioOutputs() {return audio_output_settings_.size();}
      int numAudioSources() {return audio_source_settings_.size();}
      int numAudioSinks() {return audio_sink_settings_.size();}

      /**
       * Get the AudioSettings associated with the
       * nth port. If the port does not exist (it
       * wasn't initialized), then an exception
       * is raised.
       */
      const AudioSettings & getInputSettings(int n) {return audio_input_settings_[n];}
      const AudioSettings & getOutputSettings(int n) {return audio_output_settings[n];}
      const AudioSettings & getSourceSettings(int n) {return audio_source_settings[n];}
      const AudioSettings & getSinkSettings(int n) {return audio_sink_settings[n];}

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
      void connectAudioInput(int n, AudioPatch & other, int m)
      void connectAudioOutput(int n, AudioPatch & other, int m)

      /**
       * Disconnects AudioInput/Output n from whatever its
       * connected to. If n is out of range, or if nothing
       * is connected to the port, then an exception is raised.
       */
      void disconnectAudioInput(int n)
      void disconnectAudioOutput(int n)

      const string & getName() { return name_;}

    protected:
      /**
       * To be used by subclass constructors to specify
       * the number and format of the Audio connections
       * belonging to this patch. For example, if this
       * patch is supposed to have 2 AudioInputs and
       * 1 AudioOutput, then the subclass should call
       * addInput() twice and addOutput() once.
       *
       * Ports are 0-indexed
       */
      void addInput(const AudioSettings & s)
      void addOutput(const AudioSettings & s)
      void addSource(const AudioSettings & s)
      void addSink(const AudioSettings & s)

      /**
       * function fetchAudioInput(n)
       *
       * subclasses can use this function to read a new
       * set of frames from the AudioSource that is connected
       * to the nth AudioInput of this object. n is assumed
       * to be an index for a non-null input. Otherwise the
       * behavior is undefined
       */
      const Iframe & fetchAudioInput(int n)

      /**
       * function pushAudioOutput(n)
       *
       * subclasses can use this function to send a
       * batch of frames to the AudioSink that is connected
       * to the nth AudioOutput of this object. n is assumed
       * to be an index for a non-null output. Otherwise the
       * behavior is undefined
       */
      void pushAudioOutput(int n, const Iframes & frames)

    private:
      vector<AudioSettings> audio_input_settings_;
      vector<AudioSettings> audio_output_settings_;
      vector<AudioSettings> audio_source_settings_;
      vector<AudioSettings> audio_sink_settings_;

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
      vector<AudioPortPair*> external_audio_sources_;
      vector<AudioPortPair*> external_audio_sinks_;

      /**
       * when something connects to a Source or a Sink,
       * it locks that resource, enforcing the one-to-
       * one relationship between ports
       *
       * if the pointer is null, then the port is not
       * connected
       */
      vector<bool> audio_source_locks_;
      vector<bool> audio_sink_locks_;

      /**
       * helper functions for printing error messages
       */
      void throw_port_occupied(const string & s, int n)
      void throw_port_empty(const string & s, int n)
      void throw_port_out_of_range(const string & s, int n)
      void throw_settings_dont_match(const string & s1, int n, const string & s2,
          AudioPatch & other, int m)


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
      bool lockAudioSource(int n)
      bool lockAudioSink(int n)
      void unlockAudioSource(int n)
      void unlockAudioSink(int n)
  
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
      const Iframes & sourceAudioFrame(int n)
      void sinkAudioFrame(int n, const Iframes & frames)
  }

}


#endif

