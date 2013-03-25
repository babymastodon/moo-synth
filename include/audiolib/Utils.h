#ifndef AUDIOLIB_UTILS_H
#define AUDIOLIB_UTILS_H

#ifdef _AUDIOLIB_DEBUG_
#define DEBUG(x) std::cout << x << std::endl;
#else
#define DEBUG(x)
#endif 

#include <string>


namespace audiolib{

  void sleep(unsigned long milliseconds);

  std::string indentString(const std::string & input, int n, char ch = ' ');

}


#endif
