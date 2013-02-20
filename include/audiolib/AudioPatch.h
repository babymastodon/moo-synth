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
   * */
  class AudioPatch : Patch{
    public:

      /**
       * These constructors don't do much. Subclasses
       * are expected to implement their own constructors
       * that spec out the input/outut/source/sink
       * by using the "addXXX()" function.
       */
      AudioPatch(const char *)
      AudioPatch(const string &) 
      ~AudioPatch()

      int numAudioInputs()
      int numAudioOutputs()
      int numAudioSources()
      int numAudioSinks()

      /**
       * function connectAudioInput
       *
       * Connects the nth audio input of "this" to the mth audio
       * source of "other". The "AudioSettings" of the input and
       * the source must be equal. Additionally, n and m must be
       * valid (must refer to existing inputs/sources). Eg,
       * n < this.numAudioInputs() and m < other.numaAudioOutputs().
       * If an invarient is not met, then a runtime error is raised.
       */
      void connectAudioInput(int n, AudioPatch & other, int m)
      void disconnectAudioInput(int n)
      void connectAudioOutput(int n, AudioPatch & other, int m)
      void disconnectAudioOutput(int n)
  
      const Iframe & sourceAudioFrame(int n)
      void sinkAudioFrame(int n, const Iframes & frames)

    protected:
      void addInput(AudioSettings & s)
      void addOutput(AudioSettings & s)
      void addSource(AudioSettings & s)
      void addSink(AudioSettings & s)

      /**
       * function fetchAudioInput(n)
       *
       * subclasses can use this function to read a new
       * set of frames from the AudioSource that is connected
       * to the nth AudioInput of this object
       */
      const Iframe & fetchAudioInput(int n)

      /**
       * function fetchAudioInput(n)
       *
       * subclasses can use this function to send a
       * batch of frames to the AudioSink that is connected
       * to the nth AudioOutput of this object
       */
      void pushAudioOutput(int n, const Iframes & frames)

    private:
      vector<AudioSettings> audio_input_settings_;
      vector<AudioSettings> audio_output_settings_;
      vector<AudioSettings> audio_source_settings_;
      vector<AudioSettings> audio_sink_settings_;

      vector<AudioPatch*> external_audio_sources_;
      vector<AudioPatch*> external_audio_sinks_;

      const Iframes & internalSourceAudioFrame()

  }

}


#endif

