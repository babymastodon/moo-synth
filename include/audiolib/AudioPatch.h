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

  typedef std::functional<const Iframes &(int)> AudioSourceFunction;
  typedef std::functional<void(const Iframes &)> AudioSinkFunction;

  class AudioSource{
    public:
      AudioSource(const AudioSettings & s) : settings_(s){}
      virtual const Iframes & getFrames(int iteration)=0
      const AudioSettings & getAudioSettings(){return settings_};
    private:
      const AudioSettings & settings_; 
  }

  class AudioSink{
    public:
      AudioSink(const AudioSettings & s) : settings_(s){}
      virtual void sendFrames(const Iframes &)=0
      const AudioSettings & getAudioSettings(){return settings_};
    private:
      const AudioSettings & settings_; 
  }


  /**
   * Functional-versions of AudioSource and AudioSink
   * to make it easier for subclasses of AudioPatch to
   * customize them. (The alternate approach is quite messy)
   */
  class FunctionalAudioSource : AudioSource{
    public:
      FunctionalAudioSource(const AudioSettings & s) : AudioSource(s), callback_(NULL) {}
      setCallback(AudioSourceFunction & f)
      virtual const Iframes & getFrames(int iteration)
    private:
      AudioSourceFunction * callback_;
  }

  class FunctionalAudioSink : AudioSink{
    public:
      FunctionalAudioSink(const AudioSettings & s) : AudioSink(s), callback_(NULL) {}
      setCallback(AudioSinkFunction & f)
      virtual void sendFrames(const Iframes &)
    private:
      AudioSinkFunction * callback_;
  }

  /**
   * NullAudioSource always returns a frame of zeros.
   * NullAudioSink does absolutely nothing
   */
  class NullAudioSource : AudioSource{
    public:
      NullAudioSource(const AudioSettings &)
      const Iframes & getFrames(int iteration)
    private:
      Iframes frames_;
  }

  class NullAudioSink : AudioSink{
    public:
      void sendFrames(const Iframes &)
  }


  /**
   * AudioPatch
   *
   * Base class for all audio-processing classes. (buffers,
   * mic inputs, outputs, instruments, filters, resamplers,
   * etc.)
   */
  class AudioPatch : Patch{
    public:

      AudioPatch(const char *)
      AudioPatch(const string &) 
      ~AudioPatch()

      // can connect an input/output to only one source/sink,
      // but a source/sink can be connected to multiple input/outputs
      virtual void connectAudioInput(int n, AudioSource source)=0
      virtual void connectAudioOutput(int n, AudioSink sink)=0
      virtual void disconnectAudioInput(int n)=0
      virtual void disconnectAudioOutput(int n)=0

      int numAudioInputs()
      int numAudioOutputs()

      const AudioSource & getAudioSource(int n)
      const AudioSink & getAudioSink(int n)
      int numAudioSources()
      int numAudioSinks()
  
    protected:
      set 

    private:
      // Use pointers to simplify garbage collection
      vector<AudioSource *> external_audio_sources_;
      vector<AudioSink *> external_audio_sinks_;
      vector<AudioSource *> internal_audio_sources_;
      vector<AudioSink *> internal_audio_sinks_;

      // the external_audio_sources_[n] should have
      // the same AudioSettings as external_source_type_[n]
      vector<AudioSettings> external_source_types_;
      vector<AudioSettings> external_sink_types_;

  }

}


#endif

