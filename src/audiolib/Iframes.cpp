#include "audiolib/Iframes.h"

namespace audiolib{


  /**
   * IframesVector
   */

  IframesVector::IframesVector(int size, int block_size, int sample_rate) :
    std::vector<Iframes *>(size, NULL)
  {
    for (int i=0; i<size; i++){
      push_back(new Iframes(0.0, block_size, 1));
    }
  }

  IframesVector::~IframesVector(){
    for (auto pointer: *this){
      delete pointer;
    }
  }

}
