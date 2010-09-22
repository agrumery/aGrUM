#ifndef _Unobservation_H
#define _Unobservation_H
#include <string>
#include <vector>
#include "Action.h"
using namespace std;

class Unobservation:public Action {
private:
  string node_path;
public:
  Unobservation (string node_path);
  ~Unobservation ();
  virtual TypeAction getType () {
    return type;
  };

};
#endif
