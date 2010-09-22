#ifndef _Engine_H
#define _Engine_H
#include <string>
#include <iostream>
#include "SessionException.cpp"
#define KW_sve "SVE"
#define KW_sved "SVED"
#define KW_grounded "Grounded"
#define ERR_TypeUnknown "Warning : Le type d'engin n'existe pas, le type d'engine considéré par défaut est SVE"

using namespace std;

typedef enum { tGrounded, tSve, tSved } TypeEngine;
class Engine {
private:
  TypeEngine type;
  int line;
public:
    Engine ();
    Engine (const TypeEngine type);
   ~Engine ();
  TypeEngine getType ();
  void insertLine (int line);
  int getLine ();
  void insertType (TypeEngine type);
  TypeEngine convertStringType (string str);
};
#endif
