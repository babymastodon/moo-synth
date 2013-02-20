#include "audiolib/AudioPatch.h"
#include <string>
#include <stringstream>


namespace audiolib{

      void connectAudioInput(int n, AudioPatch & other, int m){
        if (n >= external_audio_sources_.size())
          print_port_out_of_range("AudioInput",n);
        if (external_audio_sources_.at(n) != NULL)
          print_port_occupied("AudioInput",n);
        if (not other.lockAudioSource(m))
          other.print_port_occupied("AudioSource",m)
        AudioPortPair* p = new AudioPortPair(other, m);
        external_audio_sources_.at(n) = p;
      }

      void disconnectAudioInput(int n){
        if (n >= external_audio_sources_.size())
          print_port_out_of_range("AudioInput",n);
        if (external_audio_sources_.at(n) == NULL)
          print_port_empty("AudioInput",n);
        AudioPortPair& p = *external_audio_sources_[n];
        p.patch.unlockAudioSource(n);
      }


      void connectAudioOutput(int n, AudioPatch & other, int m){
        if (n >= external_audio_sinks_.size())
          print_port_out_of_range("AudioOutput",n);
        if (external_audio_sinks_.at(n) != NULL)
          print_port_occupied("AudioOutput",n);
        if (not other.lockAudioSink(m))
          other.print_port_occupied("AudioSink",m)
        AudioPortPair* p = new AudioPortPair(other, m);
        external_audio_sinks_.at(n) = p;
      }

      void disconnectAudioOutput(int n){
        if (n >= external_audio_sources_.size())
          print_port_out_of_range("AudioInput",n);
        if (external_audio_sources_.at(n) == NULL)
          print_port_empty("AudioInput",n);
        AudioPortPair& p = *external_audio_sources_[n];
        p.patch.unlockAudioSource(n);
      }

}
