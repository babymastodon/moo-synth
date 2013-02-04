#include <iostream>
#include <signal.h>

#include "audiolib/Controller.h"

bool done;
static void finish(int ignore){done = true;}

int main( int argc, char *argv[])
{
  done = false;

  //Install callback for closing
  (void) signal(SIGINT, finish);

  while (!done){
    std::cout << "MOO" << std::endl;
  }
  return 0;
}
