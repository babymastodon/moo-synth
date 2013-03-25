#ifndef AUDIOLIB_IFRAME_H
#define AUDIOLIB_IFRAME_H

#include "stk/Stk.h"

namespace audiolib{
  typedef stk::StkFrames Iframes;
  typedef float AudioFloat;

  struct IframesVector : public std::vector<Iframes *>{
    IframesVector(int size, int block_size, int sample_rate);
    ~IframesVector();
  };

  struct ConstIframesVector : public std::vector<const Iframes *>{
    ConstIframesVector(const IframesVector &);
    ConstIframesVector(int size, const Iframes * default_value);
  };

}

#endif
