#ifndef AUDIOLIB_UTILS_H
#define AUDIOLIB_UTILS_H

#include <string>


namespace audiolib{

  void sleep(unsigned long milliseconds);

  std::string indentString(const std::string & input, int n, char ch = ' ');

}


#endif
