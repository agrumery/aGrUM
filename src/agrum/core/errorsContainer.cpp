/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Errors container (at least) for parser
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <agrum/config.h>

#include <agrum/core/errorsContainer.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/core/errorsContainer.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  ParseError::ParseError(bool is_error, const std::string &msg, int line)
      : is_error(is_error), line(line), colomn(-1), msg(msg), filename(""),
        code("") {}

  ParseError::ParseError(bool is_error, const std::string &msg,
                         const std::string &filename, int line, int col)
      : is_error(is_error), line(line), colomn(col), msg(msg), filename(filename),
        code("") {}

  ParseError::ParseError(bool is_error, const std::string &msg,
                         const std::string &filename, const std::string &code,
                         int line, int col)
      : is_error(is_error), line(line), colomn(col), msg(msg), filename(filename),
        code(code) {}

  ///
  std::string ParseError::toString() const {
    std::ostringstream s;

    if (!filename.empty())
      s << filename << ":" << line << ": ";
    else
      s << line << " : ";

    s << (is_error ? "error" : "warning") << " : " << msg;

    return s.str();
  }

  ///
  std::string ParseError::toElegantString() const {
    if (filename.empty())
      return toString();

    if (code.empty()) {
      std::ifstream ifs(filename.c_str());

      for (int i = 0; i < line; i++)
        std::getline(ifs, code);
    }

    std::ostringstream s;

    s << filename << ":" << line << ": " << (is_error ? "error" : "warning") << "\n";
    s << code << "\n";

    if (colomn > 0)
      s << std::string(colomn - 1, ' ') << "^";

    s << msg << "\n";

    return s.str();
  }

  /// Return the i-th error.
  /// May throw an exception if i >= count().
  ParseError ErrorsContainer::error(int i) const {
    if (count() > i)
      return errors[i]; // May throw an error if i >= count().
    else {
      GUM_ERROR(OutOfBounds, "Index out of bound.");
    }
  }

  ParseError ErrorsContainer::last() const {
    if (count() > 0)
      return errors[count() - 1];
    else {
      GUM_ERROR(OutOfBounds, "Index out of bound.");
    }
  }

  /* ************************************************************************** */

  ErrorsContainer::ErrorsContainer() {
    error_count = 0;
    warning_count = 0;
  }

  ErrorsContainer::ErrorsContainer(const ErrorsContainer &cont) {

    error_count = cont.error_count;
    warning_count = cont.warning_count;
    errors.clear();
    errors = cont.errors;
  }

  ErrorsContainer ErrorsContainer::operator+(const ErrorsContainer &cont) const {
    ErrorsContainer newCont;

    newCont.error_count = this->error_count + cont.error_count;
    newCont.warning_count = this->warning_count + cont.warning_count;
    std::copy(this->errors.begin(), this->errors.end(), newCont.errors.begin());
    std::copy(cont.errors.begin(), cont.errors.end(), newCont.errors.end());

    return newCont;
  }

  ErrorsContainer ErrorsContainer::operator=(const ErrorsContainer &cont) {
    error_count = cont.error_count;
    warning_count = cont.warning_count;
    errors.clear();
    errors = cont.errors;

    return *this;
  }

  ErrorsContainer ErrorsContainer::operator+=(const ErrorsContainer &cont) {
    error_count += cont.error_count;
    warning_count += cont.warning_count;

    for (int i = 0; i < cont.count(); i++)
      errors.push_back(cont.error(i));

    return *this;
  }

  void ErrorsContainer::simpleErrors(std::ostream &o) const {
    if (count() == 0)
      return;

    for (int i = 0; i < count(); i++) {
      if (error(i).is_error)
        o << error(i).toString() << std::endl;
    }
  }

  void ErrorsContainer::simpleErrorsAndWarnings(std::ostream &o) const {
    if (count() == 0)
      return;

    for (int i = 0; i < count(); i++)
      o << error(i).toString() << std::endl;
  }

  void ErrorsContainer::elegantErrors(std::ostream &o) const {
    if (count() == 0)
      return;

    for (int i = 0; i < count(); i++) {
      if (error(i).is_error) {
        o << error(i).toElegantString();
        o << std::endl;
      }
    }
  }

  void ErrorsContainer::elegantErrorsAndWarnings(std::ostream &o) const {
    if (count() == 0)
      return;

    for (int i = 0; i < count(); i++) {
      o << error(i).toElegantString();
      o << std::endl;
    }
  }

} // namespace gum

#endif // DOXYGEN_SHOULD_SKIP_THIS
