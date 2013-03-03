#include "Errors.h"

namespace gum {

  Errors::Errors ( const int err_id, const std::string &err_txt, const int err_lvl ) throw() {
    this->err_id = err_id;
    this->err_txt = err_txt;
    this->err_lvl = err_lvl;
  }

  Errors::~Errors() throw() {
  }

  const char *Errors::what() const throw() {
    return err_txt.c_str();
  }

  int Errors::getLvl() const throw () {
    return err_lvl;
  }

  int Errors::getId() const throw() {
    return err_id;
  }

} // end of : namespace cn
