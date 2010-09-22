#ifndef _Observation_H
#define _Observation_H
#include <string>
#include <vector>
#include "Action.h"
using namespace std;

class Observation:public Action {
public:
  Observation (string node_path);
  Observation ();
  ~Observation ();
  virtual TypeAction getType () {
    return (type);
  };

};

#endif
