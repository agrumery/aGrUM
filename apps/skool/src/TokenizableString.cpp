
#include "TokenizableString.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>

/*******************************************************/
/****Constructeur et Destructeur************************/
/*******************************************************/
TokenizableString::TokenizableString (string s):std::string (s) {
}

TokenizableString::TokenizableString ():std::string () {
}

TokenizableString::~TokenizableString () {
}

/*******************************************************/
/****Convertir un string en un vecteur de string********/
/*******************************************************/


vector < string > TokenizableString::strtoken (string tok) {
  vector < string > vect;
  const char *
  c_string_const, *
    c_tok;
  char *
  result, *
    c_string;

  void *
    c_string_p;

  c_tok = tok.c_str ();
  c_string_const = c_str ();

  //c_string = (char *) malloc (size () * sizeof (char));
  //c_string = strcpy (c_string, c_string_const);
  c_string = new char[this->size () + 1];
  strcpy (c_string, this->c_str ());

  c_string_p = (void *) c_string;

  result = strtok (c_string, c_tok);
  while (result != NULL) {
    vect.push_back (string (result));

    result = strtok (NULL, c_tok);

  }

  free (c_string);

  return vect;

}

