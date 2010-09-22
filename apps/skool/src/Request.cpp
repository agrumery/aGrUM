#include "Request.h"

/*******************************************************/
/****Constructeur et Destructeur************************/
/*******************************************************/
Request::Request (int line, string name) {
  this->line = line;
  this->name = name;
}

Request::~Request () {
  actions.erase (actions.begin (), actions.end ());
}

/*******************************************************/
/****Insertions des indices des elts dans leur tableau**/
/*******************************************************/
void
Request::insertAction (Action & act) {
  actions.push_back (act);
}

vector < Action > &Request::getActions () {
  return actions;
}

string Request::getRequestName () {
  return name;
}

int
Request::getLine () {
  return line;
}

int
Request::getActionsSize () {
  return actions.size ();
}

