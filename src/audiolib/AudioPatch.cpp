#include "audiolib/AudioPatch.h"
#include "audiolib/Iframes.h"
#include <string>
#include <sstream>
#include <stdexcept>


namespace audiolib{

  void AudioPatch::connectAudioInput(int n, AudioPatch & other, int m){
    if (n >= numAudioInputs())
      throw_audio_port_out_of_range("AudioInput",n);
    if (m >= other.numAudioOutputs())
      other.throw_audio_port_out_of_range("AudioOutput",m);
    if (external_audio_outputs_.count(n) != 0)
      throw_audio_port_occupied("AudioInput",n);
    if (not other.lockAudioOutput(m))
      other.throw_audio_port_occupied("AudioOutput",m);
    AudioPortPair pair(other, m);
    external_audio_outputs_.emplace(n,pair);
  }

  void AudioPatch::disconnectAudioInput(int n){
    if (external_audio_outputs_.count(n) == 1){
      AudioPortPair& p = external_audio_outputs_.at(n);
      p.patch.unlockAudioOutput(p.port);
      external_audio_outputs_.erase(n);
    }
  }


  const Iframes & AudioPatch::fetchAudioInput(int n){
    AudioPortPair& p = external_audio_outputs_.at(n);
    return p.patch.outputAudioFrames(p.port);
  }


  void AudioPatch::throw_audio_port_occupied(const std::string & s, int n){
    std::stringstream ss;
    ss << "Cannot make connection. " << s << " #" << n << " on AudioPatch " << this->getName();
    ss << " is already connected to something.";
    throw std::runtime_error(ss.str());
  }
  void AudioPatch::throw_audio_port_out_of_range(const std::string & s, int n){
    std::stringstream ss;
    ss << "Port index out of range. " << s << " #" << n << " on AudioPatch " << this->getName();
    throw std::runtime_error(ss.str());
  }



  const Iframes & AudioPatch::outputAudioFrames(int n){
    std::stringstream ss;
    ss << "NotImplemented: outputAudioFrame(...) is not implemented for AudioPatch ";
    ss << this->getName();
    throw std::runtime_error(ss.str());
  }


  void AudioPatch::validate(){
    /* ensure that all AudioInputs are connected to something */
    for (int i=0; i<numAudioInputs(); i++){
      if(external_audio_outputs_.count(i) != 1){
        std::stringstream ss;
        ss << "AudioInput " << i << " on AudioPatch " << getName();
        ss << " needs to be connected to an AudioOutput";
        throw std::runtime_error(ss.str());
      }
    }
    /* ensure that all connected AudioOutputs have a matching
     * sample rate and block size */
    for (auto pair: external_audio_outputs_){
      if (pair.second.patch.getSampleRate() != getSampleRate()){
        std::stringstream ss;
        ss << "Sample Rate mismatch between two connected AudioPatches: ";
        ss << getName() << " and " << pair.second.patch.getName();
        throw std::runtime_error(ss.str());
      }
      if (pair.second.patch.getBlockSize() != getBlockSize()){
        std::stringstream ss;
        ss << "Block Size mismatch between two connected AudioPatches: ";
        ss << getName() << " and " << pair.second.patch.getName();
        throw std::runtime_error(ss.str());
      }
    }
  }

}
