#include "audiolib/AudioPatch.h"
#include <string>
#include <stringstream>
#include <stdexcept>


namespace audiolib{

      void connectAudioInput(int n, AudioPatch & other, int m){
        if (n >= numInputs())
          throw_port_out_of_range("AudioInput",n);
        if (m >= other.numSources)
          other.throw_port_out_of_range("AudioSource",m);
        if (external_audio_sources_.at(n) != NULL)
          throw_audio_port_occupied("AudioInput",n);
        if (not other.lockAudioSource(m))
          other.throw_audio_port_occupied("AudioSource",m);
        if (not other.getSourceSettings(m) == getInputSettings(n))
          throw_audio_settings_dont_match("AudioInput", n, "AudioSource", other, m);
        AudioPortPair* p = new AudioPortPair(other, m);
        external_audio_sources_.at(n) = p;
      }

      void disconnectAudioInput(int n){
        if (n >= external_audio_sources_.size())
          throw_audio_port_out_of_range("AudioInput",n);
        if (external_audio_sources_.at(n) == NULL)
          throw_audio_port_empty("AudioInput",n);
        AudioPortPair& p = *external_audio_sources_[n];
        p.patch.unlockAudioSource(p.port);
      }


      void connectAudioOutput(int n, AudioPatch & other, int m){
        if (n >= external_audio_sinks_.size())
          throw_audio_port_out_of_range("AudioOutput",n);
        if (m >= other.numSinks)
          other.throw_port_out_of_range("AudioSink",m);
        if (external_audio_sinks_.at(n) != NULL)
          throw_audio_port_occupied("AudioOutput",n);
        if (not other.lockAudioSink(m))
          other.throw_audio_port_occupied("AudioSink",m);
        if (not other.getSinkSettings(m) == getOutputSettings(n))
          throw_audio_settings_dont_match("AudioOutput", n, "AudioSink", other, m);
        AudioPortPair* p = new AudioPortPair(other, m);
        external_audio_sinks_.at(n) = p;
      }

      void disconnectAudioOutput(int n){
        if (n >= external_audio_sinks_.size())
          throw_audio_port_out_of_range("AudioOutput",n);
        if (external_audio_sinks_.at(n) == NULL)
          throw_audio_port_empty("AudioOutput",n);
        AudioPortPair& p = *external_audio_sinks_[n];
        p.patch.unlockAudioSink(p.port);
      }


      void addInput(const AudioSettings & s){
        audio_input_settings_.push_back(s);
      }
      void addOutput(const AudioSettings & s){
        audio_output_settings_.push_back(s);
      }
      void addSource(const AudioSettings & s){
        audio_source_settings_.push_back(s);
        audio_source_locks_.push_back(false);
      }
      void addSink(const AudioSettings & s){
        audio_sink_settings_.push_back(s);
        audio_sink_locks_.push_back(false);
      }


      const Iframe & fetchAudioInput(int n){
        AudioPortPair& p = *external_audio_sources_[n];
        return p.patch.sourceAudioFrame(p.port);
      }
      void pushAudioOutput(int n, const Iframes & frames){
        AudioPortPair& p = *external_audio_sources_[n];
        p.patch.sinkAudioFrame(frames, p.port);
      }


      void throw_port_occupied(const string & s, int n){
        std::stringstream ss;
        ss << "Cannot make connection. " << s << " #" << n << " on AudioPatch " << name_;
        ss << " is already connected to something.";
        throw std::runtime_error(ss.str());
      }
      void throw_port_empty(const string & s, int n){
        std::stringstream ss;
        ss << "Cannot disconnect. " << s << " #" << n << " on AudioPatch " << name_;
        ss << " is already disconnected.";
        throw std::runtime_error(ss.str());
      }
      void throw_port_out_of_range(const string & s, int n){
        std::stringstream ss;
        ss << "Port index out of range. " << s << " #" << n << " on AudioPatch " << name_;
        throw std::runtime_error(ss.str());
      }
      void throw_settings_dont_match(const string & s1, int n, const string & s2,
          AudioPatch & other, int m){
        std::stringstream ss;
        ss << "Cannot make connection. Audio settings don't match. From ";
        ss << s1 << " #" << n << " on AudioPatch " << name_ << " to ";
        ss << s2 << " #" << m << " on AudioPatch " << other.getName();
        throw std::runtime_error(ss.str());
      }



      bool lockAudioSource(int n){
        if (audio_source_locks_[n] == false)
          return audio_source_locks[n] = true;
        return false
      }
      bool lockAudioSink(int n){
        if (audio_sink_locks_[n] == false)
          return audio_sink_locks[n] = true;
        return false
      }
      void unlockAudioSource(int n){
        audio_source_locks[n] = false;
      }
      void unlockAudioSink(int n){
        audio_sink_locks[n] = false;
      }



      const Iframes & sourceAudioFrame(int n){
        std::stringstream ss;
        ss << "NotImplemented: sourceAudioFrame(...) is not implemented for AudioPatch ";
        ss << name_;
        throw std::runtime_error(ss.str());
      }
      void sinkAudioFrame(int n, const Iframes & frames){
        std::stringstream ss;
        ss << "NotImplemented: sinkAudioFrame(...) is not implemented for AudioPatch ";
        ss << name_;
        throw std::runtime_error(ss.str());
      }

}
