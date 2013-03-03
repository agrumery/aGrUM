#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <string>

namespace gum {

  class Errors : public std::exception {

    private:
      int err_id;
      std::string err_txt;
      int err_lvl;

    protected:

    public:
      Errors ( const int err_id, const std::string &err_txt, const int err_lvl ) throw();

      virtual const char *what() const throw();

      int getLvl() const throw();
      int getId() const throw();

      ~Errors() throw();

  }; // end of : Errors class definition

} // end of : namespace cn

//#include "Errors.cpp"

#endif // end of : ifndef ERRORS_H
