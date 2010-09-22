#ifndef _Import_H
#define _Import_H
#include <string>
using namespace std;

class Import {
private:
  string package;
  int line;
public:
    Import (int line, string package);
   ~Import ();
  int getLine ();
  string getSystemName ();
  string getImportPackage();
};
#endif
