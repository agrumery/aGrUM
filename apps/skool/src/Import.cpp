#include "Import.h"
#include "TokenizableString.h"
#include <vector>
#include <iostream>
Import::Import (int line, string package) {
  this->line = line;
  this->package = package;
}

Import::~Import () {
}

int
Import::getLine () {
  return line;
}

string Import::getSystemName () {
  TokenizableString str(package);
  vector<string> elts = str.strtoken(".");
  return elts[elts.size()-1];
}

string Import::getImportPackage(){
  return package; 
}