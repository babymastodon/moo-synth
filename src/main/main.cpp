#include <iostream>
#include <signal.h>

#include "stk/WvOut.h"
#include "stk/Instrmnt.h"
#include "stk/JCRev.h"
#include "stk/Voicer.h"
#include "stk/Skini.h"
#include "stk/RtAudio.h"

#include "stk/Mutex.h"

// STK Instrument Classes
#include "stk/Clarinet.h"
#include "stk/BlowHole.h"
#include "stk/Saxofony.h"
#include "stk/Flute.h"
#include "stk/Brass.h"
#include "stk/BlowBotl.h"
#include "stk/Bowed.h"
#include "stk/Plucked.h"
#include "stk/StifKarp.h"
#include "stk/Sitar.h"
#include "stk/Mandolin.h"
#include "stk/Rhodey.h"
#include "stk/Wurley.h"
#include "stk/TubeBell.h"
#include "stk/HevyMetl.h"
#include "stk/PercFlut.h"
#include "stk/BeeThree.h"
#include "stk/FMVoices.h"
#include "stk/VoicForm.h"
#include "stk/Moog.h"
#include "stk/Simple.h"
#include "stk/Drummer.h"
#include "stk/BandedWG.h"
#include "stk/Shakers.h"
#include "stk/ModalBar.h"
#include "stk/Mesh2D.h"
#include "stk/Resonate.h"
#include "stk/Whistle.h"

#include "audiolib/Controller.h"

using namespace stk;

bool done;
static void finish(int ignore){done = true;}

#define DELTA_CONTROL_TICKS 64 // default sample frames between control input checks

// The TickData structure holds all the class instances and data that
// are shared by the various processing functions.
struct TickData {
  WvOut **wvout;
  Instrmnt **instrument;
  Voicer *voicer;
  JCRev reverb;
  Skini::Message message;
  StkFloat volume;
  StkFloat t60;
  unsigned int nWvOuts;
  int nVoices;
  int currentVoice;
  int channels;
  int counter;
  bool realtime;
  bool settling;
  bool haveMessage;
  int frequency;

  // Default constructor.
  TickData()
    : wvout(0), instrument(0), voicer(0), volume(1.0), t60(0.75),
      nWvOuts(0), nVoices(1), currentVoice(0), channels(2), counter(0),
      realtime( false ), settling( false ), haveMessage( false ) {}
};


// The tick() function handles sample computation and scheduling of
// control updates.  If doing realtime audio output, it will be called
// automatically when the system needs a new buffer of audio samples.
int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
          double streamTime, RtAudioStreamStatus status, void *dataPointer )
{
  TickData *data = (TickData *) dataPointer;
  register StkFloat sample, *samples = (StkFloat *) outputBuffer;
  int counter = (int) nBufferFrames;
  printf("MOO\n");

  for ( int i=0; i<counter; i++ ) {
    sample = data->volume * data->reverb.tick( data->voicer->tick() );
    for ( int k=0; k<data->channels; k++ ) *samples++ = sample;
  }

  return 0;
}




int main( int argc, char *argv[])
{
  done = false;

  //Install callback for closing
  (void) signal(SIGINT, finish);

  TickData data;
  int i;

  RtAudio dac(RtAudio::LINUX_PULSE);
  //RtAudio dac(RtAudio::LINUX_ALSA);

  // If you want to change the default sample rate (set in Stk.h), do
  // it before instantiating any objects!  If the sample rate is
  // specified in the command line, it will override this setting.
  Stk::setSampleRate( 44100.0 );

  // Depending on how you compile STK, you may need to explicitly set
  // the path to the rawwave directory.
  // Stk::setRawwavePath( "../../rawwaves/" );
  Stk::setRawwavePath("../resources/rawwaves");

  // By default, warning messages are not printed.  If we want to see
  // them, we need to specify that here.
  Stk::showWarnings( true );

  // Check the command-line arguments for errors and to determine
  // the number of WvOut objects to be instantiated (in utilities.cpp).

  // Instantiate the instrument(s) type from the command-line argument
  // (in utilities.cpp).
  data.nVoices = 1;
  data.instrument = (Instrmnt **) calloc( data.nVoices, sizeof(Instrmnt *) );
  data.instrument[0] = new Clarinet(10.0);

  data.voicer = (Voicer *) new Voicer( 0.0 );
  data.voicer->addInstrument( data.instrument[0] );


  RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  RtAudio::StreamParameters parameters;
  parameters.deviceId = dac.getDefaultOutputDevice();
  parameters.nChannels = data.channels;
  unsigned int bufferFrames = RT_BUFFER_SIZE;
  try {
    dac.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, (void *)&data );
  }
  catch ( RtError& error ) {
    error.printMessage();
    goto cleanup;
  }

  // Set the reverb parameters.
  data.reverb.setT60( data.t60 );
  data.reverb.setEffectMix(0.2);

  // set our callback function and start the dac.
  try {
    dac.startStream();
  }
  catch ( RtError &error ) {
    error.printMessage();
    goto cleanup;
  }

  i = 0;

  // Setup finished.
  while ( !done ) {
    // Periodically check "done" status.
    data.voicer->noteOn((float)i+20, 100.0);
    i=(i+1)%100;
    Stk::sleep( 100 );
  }

  // Shut down the output stream.
  try {
    dac.closeStream();
  }
  catch ( RtError& error ) {
    error.printMessage();
  }

cleanup:

  delete data.voicer;

  for ( i=0; i<data.nVoices; i++ ) delete data.instrument[i];
  free( data.instrument );

  std::cout << "\nStk demo finished ... goodbye.\n\n";
  return 0;

}
