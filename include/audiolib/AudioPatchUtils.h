#ifndef AUDIOLIB_AUDIO_PATCH_UTILS_H
#define AUDIOLIB_AUDIO_PATCH_UTILS_H

#include "audiolib/AudioPatch.h"
#include "stk/RtAudio.h"

namespace audiolib{

  /**
   * AudioDACPatch
   *
   * 1 input port (port 1).
   *
   * Upon construction, it opens up a stream with the
   * audio card. This class has an internal thread
   * (managed by RtAudio) which will periodically
   * query the input port for new frames.
   */
  class AudioDACPatch : public AudioPatch {
    public:
      AudioDACPatch(const std::string & name, unsigned int n_channels=1, unsigned int sample_rate=44100, unsigned int block_size=64);

      int numAudioInputs(){return n_channels_;}

    private:
      RtAudio rt_audio_;
      int n_channels_;

      static int rtCallback(void* outputBuffer, void* inputBuffer, 
          unsigned int nFrames, double streamTime,
          RtAudioStreamStatus status, void* userData);
  };

}

#endif
