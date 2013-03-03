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
      /**
       * The AudioSettings passed in the constructor are just
       * "suggestions". The values will be modified in place
       * if the audio card wants to do something different.
       */
      AudioDACPatch(const char * name, int sample_rate, int n_channels);
      AudioDACPatch(const std::string & name, int sample_rate, int n_channels);

    private:
      RtAudio rt_audio_;
      int n_channels_;
      unsigned int buffer_frames_;

      static int rt_callback(void* outputBuffer, void* inputBuffer, 
          unsigned int nFrames, double streamTime,
          RtAudioStreamStatus status, void* userData);
  };

}

#endif
