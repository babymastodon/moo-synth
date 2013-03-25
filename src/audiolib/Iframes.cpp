#include "audiolib/Iframes.h"

namespace audiolib{


  /**
   * IframesVector
   */

  IframesVector::IframesVector(int size, int block_size, float sample_rate) :
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

  /**
   * ConstIframesVector 
   */

  ConstIframesVector::ConstIframesVector(const IframesVector & other):
    std::vector<const Iframes *>(other.begin(), other.end()){}
  ConstIframesVector::ConstIframesVector(int size, const Iframes * default_value):
    std::vector<const Iframes *>(size, default_value){}

}
