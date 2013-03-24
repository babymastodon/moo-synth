#ifndef AUDIOLIB_IFRAME_H
#define AUDIOLIB_IFRAME_H

#include "stk/Stk.h"

namespace audiolib{
  typedef stk::StkFrames Iframes;
  typedef float AudioFloat;

  typedef std::vector<const Iframes *> ConstIframesVector;

  struct IframesVector : public std::vector<Iframes *>{
    IframesVector(int size, int block_size, int sample_rate);
    ~IframesVector();
  };
}

#endif
