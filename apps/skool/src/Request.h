#ifndef _REQUEST_H
#define	_REQUEST_H

#include <string>
#include <vector>
#include "Action.h"
#include "Observation.h"
#include "Unobservation.h"
#include "Interrogation.h"

using namespace std;

class Request {
private:
  string name;
  int line;
    vector < Action > actions;
public:
    Request (int line, string name);
   ~Request ();
  void insertAction (Action& act);
    vector < Action > &getActions ();
  string getRequestName ();
  int getLine ();
  int getActionsSize ();
};
#endif
