#include "audiolib/AudioPatch.h"
#include "audiolib/Iframes.h"
#include <string>
#include <sstream>
#include <stdexcept>


namespace audiolib{

  void AudioPatch::connectAudioInput(int n, AudioPatch & other, int m){
    if (n >= numAudioInputs())
      throw_audio_port_out_of_range("AudioInput",n);
    if (m >= other.numAudioSources())
      other.throw_audio_port_out_of_range("AudioSource",m);
    if (external_audio_sources_.count(n) != 0)
      throw_audio_port_occupied("AudioInput",n);
    if (not other.lockAudioSource(m))
      other.throw_audio_port_occupied("AudioSource",m);
    AudioPortPair pair(other, m);
    external_audio_sources_.emplace(n,pair);
  }

  void AudioPatch::disconnectAudioInput(int n){
    if (external_audio_sources_.count(n) == 1){
      AudioPortPair& p = external_audio_sources_.at(n);
      p.patch.unlockAudioSource(p.port);
      external_audio_sources_.erase(n);
    }
  }


  void AudioPatch::connectAudioOutput(int n, AudioPatch & other, int m){
    if (n >= numAudioOutputs())
      throw_audio_port_out_of_range("AudioOutput",n);
    if (m >= other.numAudioSinks())
      other.throw_audio_port_out_of_range("AudioSink",m);
    if (external_audio_sinks_.count(n) != 0)
      throw_audio_port_occupied("AudioOutput",n);
    if (not other.lockAudioSink(m))
      other.throw_audio_port_occupied("AudioSink",m);
    AudioPortPair pair(other, m);
    external_audio_sinks_.emplace(n,pair);
  }

  void AudioPatch::disconnectAudioOutput(int n){
    if (external_audio_sinks_.count(n) == 1){
      AudioPortPair& p = external_audio_sinks_.at(n);
      p.patch.unlockAudioSink(p.port);
      external_audio_sources_.erase(n);
    }
  }


  const Iframes & AudioPatch::fetchAudioInput(int n){
    AudioPortPair& p = external_audio_sources_.at(n);
    return p.patch.sourceAudioFrames(p.port);
  }
  void AudioPatch::pushAudioOutput(int n, const Iframes & frames){
    AudioPortPair& p = external_audio_sources_.at(n);
    p.patch.sinkAudioFrames(p.port, frames);
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



  bool AudioPatch::lockAudioSource(int n){
    if (audio_source_locks_.count(n) == 0){
      audio_source_locks_.insert(n);
      return true;
    }
    return false;
  }
  bool AudioPatch::lockAudioSink(int n){
    if (audio_sink_locks_.count(n) == 0)
      audio_sink_locks_.insert(n);
      return true;
    return false;
  }
  void AudioPatch::unlockAudioSource(int n){
    if (audio_source_locks_.count(n) != 0)
      audio_source_locks_.erase(n);
  }
  void AudioPatch::unlockAudioSink(int n){
    if (audio_sink_locks_.count(n) != 0)
      audio_sink_locks_.erase(n);
  }



  const Iframes & AudioPatch::sourceAudioFrames(int n){
    std::stringstream ss;
    ss << "NotImplemented: sourceAudioFrame(...) is not implemented for AudioPatch ";
    ss << this->getName();
    throw std::runtime_error(ss.str());
  }
  void AudioPatch::sinkAudioFrames(int n, const Iframes & frames){
    std::stringstream ss;
    ss << "NotImplemented: sinkAudioFrame(...) is not implemented for AudioPatch ";
    ss << this->getName();
    throw std::runtime_error(ss.str());
  }


  void AudioPatch::validate(){
    /* ensure that all AudioInputs are connected to something */
    for (int i=0; i<numAudioInputs(); i++){
      if(external_audio_sources_.count(i) != 1){
        std::stringstream ss;
        ss << "AudioInput " << i << " on AudioPatch " << getName();
        ss << " needs to be connected to an AudioSource";
        throw std::runtime_error(ss.str());
      }
    }
    /* ensure that all AudioOutputs are connected to something */
    for (int i=0; i<numAudioOutputs(); i++){
      if(external_audio_sinks_.count(i) != 1){
        std::stringstream ss;
        ss << "AudioOutput " << i << " on AudioPatch " << getName();
        ss << " needs to be connected to an AudioSink";
        throw std::runtime_error(ss.str());
      }
    }
    /* ensure that all connected AudioSources have a matching
     * sample rate and block size */
    for (auto pair: external_audio_sources_){
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
    /* ensure that all connected AudioSinks have a matching
     * sample rate and block size */
    for (auto pair: external_audio_sinks_){
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
