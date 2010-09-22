#ifndef _Interrogation_H
#define _Interrogation_H
#include <string>
#include "Action.h"
// #include <vector>
using namespace std;

class Interrogation:public Action {
private:
  //  int line;
  string node_path;
public:
  Interrogation (string node_path);
  ~Interrogation ();
  virtual TypeAction getType () {
    return type;
  };

};
#endif
