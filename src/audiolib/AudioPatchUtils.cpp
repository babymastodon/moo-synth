#include "audiolib/AudioPatchUtils.h"
#include <string>


namespace audiolib{

  /**
   * AudioDACPatch
   */
  AudioDACPatch::AudioDACPatch(const char * name, int sample_rate, int n_channels) : AudioDACPatch(std::string(name), sample_rate, n_channels) {}

  AudioDACPatch::AudioDACPatch(const std::string & name, int sample_rate, int n_channels) : AudioPatch(name, sample_rate), n_channels_(n_channels) {
    RtAudio::StreamParameters sp;
    sp.deviceId = rt_audio_.getDefaultOutputDevice();
    sp.nChannels = n_channels_;

    RtAudio::StreamOptions so;
    so.flags |= RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;

    buffer_frames_ = 0;

    rt_audio_.openStream(&sp, NULL, RTAUDIO_FLOAT32, sample_rate, 
        &buffer_frames_, AudioDACPatch::rt_callback, this, &so);

  }

  int AudioDACPatch::rt_callback(void* outputBuffer, void* inputBuffer, 
      unsigned int nFrames, double streamTime,
      RtAudioStreamStatus status, void* userData)
  {
    return 0;
  }

}
