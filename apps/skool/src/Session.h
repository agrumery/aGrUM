#ifndef _Session_H
#define	_Session_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <ctime>
#include <cstdio>
#include "TokenizableString.h"
#include "Request.h"
#include "Import.h"
#include "Action.h"
#include "Engine.h"
#include <agrum/prm/skool/SkoolReader.h>
#include <agrum/prm/PRMFactory.h>

#define Err_unknownAction "Le mot clé entré n'est pas correct."
using namespace std;

class Session {

private:
  string package;
  Engine engine;
  int package_line;
  vector < Import > imports;
  vector < Request > requests;
  bool inside_request;
  bool quit;

public:
  int line_error;
    Session ();
   ~Session ();
  int getImportsSize ();
  int getRequestsSize ();
  vector < string > convertStringList (string s);
  void insertPackageInformation (int line, const string str);
  void insertImport (Import & imp);
  void insertEngine(Engine& eng);
  void setExitStatus(bool status);
  bool getExitStatus(); 
  string getPackageName ();
  Engine &getEngine();
  vector < Import > &getImports ();
  vector < Request > &getRequests ();
  void enterRequest (Request q);
  Request *getCurrentRequest ();
  bool isInsideRequest ();
  void quitRequest ();
  void parser (char *fichier);
  string getPackageSystemName(const string systemNameInRequest);
  string getPackageSystemFilename(const string systemImport);
  void act (const string requestFile, ostream& o, bool option);
};


#endif
