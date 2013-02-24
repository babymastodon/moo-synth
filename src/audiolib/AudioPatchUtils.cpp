#include "audiolib/AudioPatchUtils.h"
#include <string>


namespace audiolib{

  /**
   * AudioDACPatch
   */
  AudioDACPatch::AudioDACPatch(const char * name, AudioSettings & s) : AudioDACPatch(std::string(name), s) {}

  AudioDACPatch::AudioDACPatch(const std::string & name, AudioSettings & s) : AudioPatch(name){
    RtAudio::StreamParameters sp;
    sp.deviceId = rt_audio_.getDefaultOutputDevice();
    sp.nChannels = s.n_channels;

    RtAudio::StreamOptions so;
    so.flags |= RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;

    unsigned int bufferFrames = 0;

    rt_audio_.openStream(&sp, NULL, RTAUDIO_FLOAT32, s.sample_rate, 
        &bufferFrames, AudioDACPatch::rt_callback, this, &so);

  }

  int AudioDACPatch::rt_callback(void* outputBuffer, void* inputBuffer, 
      unsigned int nFrames, double streamTime,
      RtAudioStreamStatus status, void* userData)
  {
    return 0;
  }

}
