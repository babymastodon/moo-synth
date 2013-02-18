#include "audiolib/AudioPatch.h"
#include <string>


namespace audiolib{


  /**
   * FunctionalAudioSource
   */
  FunctionalAudioSource::setCallback(AudioSourceFunction & f){
    callback_ = &f;
  }

  const Iframes & FunctionalAudioSource::getFrames(int iteration){
    if (callback != NULL){
      return (*callback_)(iteration);
    }
  }



  /**
   * FunctionalAudioSink
   */
  FunctionalAudioSink::setCallback(AudioSinkFunction & f){
    callback_ = &f;
  }

  void FunctionalAudioSink::sendFrames(const Iframes & frames){
    if (callback != NULL){
      return (*callback_)(frames);
    }
  }

  /**
   * NullAudioSource
   */
  NullAudioSource::NullAudioSource(const AudioSettings & s)
    : AudioSource(s), frames_(s.n_frames, s.n_channels)
  {
    // The constructor must initialize the empty buffer with zeros
    int size = s.n_frames * s.n_channels;
    for (int i=0; i<size; i++){
      frames_[i] = 0.0;
    }
  }

  /**
   * NullAudioSink
   */
  NullAudioSink::sendFrames(const AudioSettings & s){
    // do nothing
  }


  AudioPatch::AudioPatch(const char * s) : Patch(s){}
  AudioPatch::AudioPatch(const string & s) : Patch(s){}

}
