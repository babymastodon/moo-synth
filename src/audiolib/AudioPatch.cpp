#include "audiolib/AudioPatch.h"
#include <string>


namespace audiolib{

  AudioPatch::AudioPatch(const char * s) : Patch(s){}
  AudioPatch::AudioPatch(const string & s) : Patch(s){}

}
