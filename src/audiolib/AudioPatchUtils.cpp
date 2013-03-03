#include "audiolib/AudioPatchUtils.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include <cassert>


namespace audiolib{

  /**
   * AudioDACPatch
   */
  AudioDACPatch::AudioDACPatch(const std::string & name,
      unsigned int n_channels, unsigned int sample_rate, unsigned  int block_size)
    : AudioPatch(name, sample_rate, block_size), n_channels_(n_channels) {
    RtAudio::StreamParameters sp;
    sp.deviceId = rt_audio_.getDefaultOutputDevice();
    sp.nChannels = n_channels_;

    RtAudio::StreamOptions so;
    so.flags |= RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME | RTAUDIO_NONINTERLEAVED; 

    unsigned int n_frames = (unsigned int) getBlockSize();

    rt_audio_.openStream(&sp, NULL, RTAUDIO_FLOAT32, sample_rate, 
        &n_frames, AudioDACPatch::rtCallback, this, &so);

    /* if the sample rate of the stream does not match the output
     * sample rate of this patch, then throw an error (resampling
     * is not yet implemented)
     */
    if (getSampleRate() != rt_audio_.getStreamSampleRate()){
      std::stringstream ss;
      ss << "Sample rate of DAC is ";
      ss << rt_audio_.getStreamSampleRate();
      ss << " but is supposed to be ";
      ss << getSampleRate();
      throw std::runtime_error(ss.str());
    }

    /* if the block size of the stream does not match the output
     * block size of this frame, then throw an error (buffering is
     * not yet implemented)
     */
    if (getBlockSize() != n_frames){
      std::stringstream ss;
      ss << "Block size of DAC is ";
      ss << n_frames;
      ss << " but is supposed to be ";
      ss << getBlockSize();
      throw std::runtime_error(ss.str());
    }

  }

  int AudioDACPatch::rtCallback(void* outputBuffer, void* inputBuffer, 
      unsigned int nFrames, double streamTime,
      RtAudioStreamStatus status, void* userData)
  {
    AudioDACPatch & patch = *((AudioDACPatch*) userData);

    /* make sure the RTaudio didn't change the block size */
    assert (patch.getBlockSize() == nFrames);

    /* combine the stream data from the input channels into the single
     * output buffer (non-interleaved)
     */
    float* output_buffer = (float*)outputBuffer;
    int block_size = patch.getBlockSize();
    for (int i=0; i<patch.n_channels_; i++){
      const Iframes& frames = patch.fetchAudioInput(i);
      for (int j=0; j<block_size; j++){
        output_buffer[block_size*i+j] = frames[j];
      }
    }

    return 0;
  }

}
