#include "Engine.h"

Engine::Engine () {
  type = tSve;
}

Engine::Engine (TypeEngine t) {
  type = t;
}

Engine::~Engine () {
}

TypeEngine Engine::getType () {
  return type;
}

void
Engine::insertLine (int line) {
  line = line;
}

int
Engine::getLine () {
  return line;
}

void
Engine::insertType (TypeEngine type) {
  this->type = type;
}

TypeEngine Engine::convertStringType (string str) {
  if (!str.compare (KW_sve))
    return tSve;
  else if (!str.compare (KW_sved))
    return tSved;
  else if (!str.compare (KW_grounded))
    return tGrounded;
  else
    throw SessionException (ERR_TypeUnknown,str);
}
