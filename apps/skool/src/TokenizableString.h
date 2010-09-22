
#ifndef __TOKENIZABLE_STRING__
#define __TOKENIZABLE_STRING__ 1

#include <string>
#include <vector>

using namespace std;

class TokenizableString:public
  std::string {

public:
  TokenizableString (string s);
  TokenizableString ();
  ~
  TokenizableString ();
  vector <
    string >
  strtoken (string tok);

};

#endif
