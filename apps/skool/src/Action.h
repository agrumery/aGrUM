#ifndef _Action_H
#define _Action_H
#include <string>
#include <fstream>
#include <agrum/prm/PRMInference.h>
#include <agrum/prm/skool/SkoolReader.h>
#include "TokenizableString.h"
#define Err_unknownAction "Le mot clé entré n'est pas correct."

using namespace std;
using namespace gum::prm; 

enum TypeAction { tUnknown, tObservation, tUnobservation, tInterrogation };

class Action {
private:
  int line;
  string node_path;
  string value;
protected:
  enum TypeAction type;

public:
    virtual TypeAction getType () {
    return type;
  };
  Action ();
  Action (string node_path);
  ~Action ();
  int getLine ();
  void insertLine (const int line);
  string getValue ();
  void setValue (const string value);
  void write(PRM *prm,  PRMInference* inf, const string systemName, const string requestFile,ostream& os, bool option);
  string getNode_path ();

};
#endif
