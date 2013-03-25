#include "audiolib/Utils.h"
#include "stk/Stk.h"
#include <sstream>


namespace audiolib{

  void sleep(unsigned long milliseconds) {
    stk::Stk::sleep(milliseconds);
  }

  std::string indentString(const std::string & input, int n, char ch){
    std::stringstream ss;
    std::istringstream iss(input);
    std::string line;
    std::string indent(n, ch);
    while (std::getline(iss, line)){
      ss << indent << line << "\n";
    }
    return ss.str();
  }

}
