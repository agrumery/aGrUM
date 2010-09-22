#ifndef _SessionException_H
#define _SessionException_H
#include <string>
#include <exception>
using namespace std;

class SessionException:public exception {
private:
  string description;
  string reason;

public:
SessionException (const string & sentence = "", const string& line="") throw ():description
    (sentence), reason(line){ }

  virtual const char *what () const throw () {
    string res = description + ": " + reason;
    return res.c_str ();
  }

  virtual ~ SessionException ()throw () {
  }
};
#endif
