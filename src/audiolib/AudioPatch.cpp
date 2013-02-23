#include "audiolib/AudioPatch.h"
#include "audiolib/Iframes.h"
#include <string>
#include <sstream>
#include <stdexcept>


namespace audiolib{

  bool AudioSettings::operator==(const AudioSettings & other) const{
    return (
        n_frames == other.n_frames &&
        n_channels == other.n_channels &&
        sample_rate == other.sample_rate
        );
  }

  void AudioPatch::connectAudioInput(int n, AudioPatch & other, int m){
    if (n >= numAudioInputs())
      throw_audio_port_out_of_range("AudioInput",n);
    if (m >= other.numAudioSources())
      other.throw_audio_port_out_of_range("AudioSource",m);
    if (external_audio_sources_.at(n) != NULL)
      throw_audio_port_occupied("AudioInput",n);
    if (not other.lockAudioSource(m))
      other.throw_audio_port_occupied("AudioSource",m);
    if (not (other.getAudioSourceSettings(m) == getAudioInputSettings(n)))
      throw_audio_settings_dont_match("AudioInput", n, "AudioSource", other, m);
    AudioPortPair* p = new AudioPortPair(other, m);
    external_audio_sources_.at(n) = p;
  }

  void AudioPatch::disconnectAudioInput(int n){
    if (n >= (int)external_audio_sources_.size())
      throw_audio_port_out_of_range("AudioInput",n);
    if (external_audio_sources_.at(n) == NULL)
      throw_audio_port_empty("AudioInput",n);
    AudioPortPair& p = *external_audio_sources_[n];
    p.patch.unlockAudioSource(p.port);
  }


  void AudioPatch::connectAudioOutput(int n, AudioPatch & other, int m){
    if (n >= (int)external_audio_sinks_.size())
      throw_audio_port_out_of_range("AudioOutput",n);
    if (m >= other.numAudioSinks())
      other.throw_audio_port_out_of_range("AudioSink",m);
    if (external_audio_sinks_.at(n) != NULL)
      throw_audio_port_occupied("AudioOutput",n);
    if (not other.lockAudioSink(m))
      other.throw_audio_port_occupied("AudioSink",m);
    if (not (other.getAudioSinkSettings(m) == getAudioOutputSettings(n)))
      throw_audio_settings_dont_match("AudioOutput", n, "AudioSink", other, m);
    AudioPortPair* p = new AudioPortPair(other, m);
    external_audio_sinks_.at(n) = p;
  }

  void AudioPatch::disconnectAudioOutput(int n){
    if (n >= (int)external_audio_sinks_.size())
      throw_audio_port_out_of_range("AudioOutput",n);
    if (external_audio_sinks_.at(n) == NULL)
      throw_audio_port_empty("AudioOutput",n);
    AudioPortPair& p = *external_audio_sinks_[n];
    p.patch.unlockAudioSink(p.port);
  }


  void AudioPatch::addInput(const AudioSettings & s){
    audio_input_settings_.push_back(s);
  }
  void AudioPatch::addOutput(const AudioSettings & s){
    audio_output_settings_.push_back(s);
  }
  void AudioPatch::addSource(const AudioSettings & s){
    audio_source_settings_.push_back(s);
    audio_source_locks_.push_back(false);
  }
  void AudioPatch::addSink(const AudioSettings & s){
    audio_sink_settings_.push_back(s);
    audio_sink_locks_.push_back(false);
  }


  const Iframes & AudioPatch::fetchAudioInput(int n){
    AudioPortPair& p = *external_audio_sources_[n];
    return p.patch.sourceAudioFrames(p.port);
  }
  void AudioPatch::pushAudioOutput(int n, const Iframes & frames){
    AudioPortPair& p = *external_audio_sources_[n];
    p.patch.sinkAudioFrames(p.port, frames);
  }


  void AudioPatch::throw_audio_port_occupied(const std::string & s, int n){
    std::stringstream ss;
    ss << "Cannot make connection. " << s << " #" << n << " on AudioPatch " << this->getName();
    ss << " is already connected to something.";
    throw std::runtime_error(ss.str());
  }
  void AudioPatch::throw_audio_port_empty(const std::string & s, int n){
    std::stringstream ss;
    ss << "Cannot disconnect. " << s << " #" << n << " on AudioPatch " << this->getName();
    ss << " is already disconnected.";
    throw std::runtime_error(ss.str());
  }
  void AudioPatch::throw_audio_port_out_of_range(const std::string & s, int n){
    std::stringstream ss;
    ss << "Port index out of range. " << s << " #" << n << " on AudioPatch " << this->getName();
    throw std::runtime_error(ss.str());
  }
  void AudioPatch::throw_audio_settings_dont_match(const std::string & s1, int n, const std::string & s2,
      AudioPatch & other, int m){
    std::stringstream ss;
    ss << "Cannot make connection. Audio settings don't match. From ";
    ss << s1 << " #" << n << " on AudioPatch " << this->getName() << " to ";
    ss << s2 << " #" << m << " on AudioPatch " << other.getName();
    throw std::runtime_error(ss.str());
  }



  bool AudioPatch::lockAudioSource(int n){
    if (audio_source_locks_[n] == false)
      return audio_source_locks_[n] = true;
    return false;
  }
  bool AudioPatch::lockAudioSink(int n){
    if (audio_sink_locks_[n] == false)
      return audio_sink_locks_[n] = true;
    return false;
  }
  void AudioPatch::unlockAudioSource(int n){
    audio_source_locks_[n] = false;
  }
  void AudioPatch::unlockAudioSink(int n){
    audio_sink_locks_[n] = false;
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

}
