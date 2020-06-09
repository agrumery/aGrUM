
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Errors container (at least) for parser
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/agrum.h>

#include <agrum/tools/core/errorsContainer.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/core/errorsContainer_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  ParseError::ParseError(bool is_error, const std::string& msg, Idx line) :
      is_error(is_error), line(line), column(0), msg(msg), filename(""), code("") {
  }

  ParseError::ParseError(bool               is_error,
                         const std::string& msg,
                         const std::string& filename,
                         Idx                line,
                         Idx                col) :
      is_error(is_error),
      line(line), column(col), msg(msg), filename(filename), code("") {}

  ParseError::ParseError(bool               is_error,
                         const std::string& msg,
                         const std::string& filename,
                         const std::string& code,
                         Idx                line,
                         Idx                col) :
      is_error(is_error),
      line(line), column(col), msg(msg), filename(filename), code(code) {}

  ParseError::ParseError(const ParseError& err) {
    is_error = err.is_error;
    line = err.line;
    column = err.column;   // default 0
    msg = err.msg;
    filename = err.filename;   // default ""
    code = err.code;           // default ""
  }

  ParseError ParseError::operator=(const ParseError& err) {
    if (this != &err) {
      is_error = err.is_error;
      line = err.line;
      column = err.column;   // default 0
      msg = err.msg;
      filename = err.filename;   // default ""
      code = err.code;           // default ""
    }

    return *this;
  }

  ///
  std::string ParseError::toString() const {
    std::ostringstream s;

    if (!filename.empty()) s << filename << ":";

    if (line > 0) s << line << ": ";

    if (column > 0) s << column << " : ";

    s << (is_error ? "error" : "warning") << " : " << msg;

    return s.str();
  }

  ///
  std::string ParseError::toElegantString() const {
    if (code.empty()) {
      std::ifstream ifs(filename.c_str());

      for (Idx i = 0; i < line; i++)
        std::getline(ifs, code);
    }

    std::ostringstream s;

    s << toString() << std::endl << code << std::endl;

    if (column > 0) s << std::string(column - 1, ' ') << "^";

    return s.str();
  }

  ParseError ErrorsContainer::error(Idx i) const {
    if (count() > i)
      return errors[i];   // May throw an error if i >= count().
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

  ErrorsContainer::ErrorsContainer() {
    error_count = 0;
    warning_count = 0;
  }

  ErrorsContainer::ErrorsContainer(const ErrorsContainer& cont) {
    error_count = cont.error_count;
    warning_count = cont.warning_count;
    errors.clear();
    errors = cont.errors;
  }

  ErrorsContainer ErrorsContainer::operator+(const ErrorsContainer& cont) const {
    ErrorsContainer newCont;

    newCont.error_count = this->error_count + cont.error_count;
    newCont.warning_count = this->warning_count + cont.warning_count;
    std::copy(this->errors.begin(), this->errors.end(), newCont.errors.begin());
    std::copy(cont.errors.begin(), cont.errors.end(), newCont.errors.end());

    return newCont;
  }

  ErrorsContainer ErrorsContainer::operator=(const ErrorsContainer& cont) {
    error_count = cont.error_count;
    warning_count = cont.warning_count;
    errors.clear();
    errors = cont.errors;

    return *this;
  }

  ErrorsContainer ErrorsContainer::operator+=(const ErrorsContainer& cont) {
    error_count += cont.error_count;
    warning_count += cont.warning_count;

    for (Idx i = 0; i < cont.count(); i++)
      errors.push_back(cont.error(i));

    return *this;
  }

  void ErrorsContainer::simpleErrors(std::ostream& o) const {
    if (count() == 0) return;

    for (Idx i = 0; i < count(); i++) {
      if (error(i).is_error) o << error(i).toString() << std::endl;
    }
  }

  void ErrorsContainer::simpleErrorsAndWarnings(std::ostream& o) const {
    if (count() == 0) return;

    for (Idx i = 0; i < count(); i++)
      o << error(i).toString() << std::endl;
  }

  void ErrorsContainer::elegantErrors(std::ostream& o) const {
    if (count() == 0) return;

    for (Idx i = 0; i < count(); i++) {
      if (error(i).is_error) {
        o << error(i).toElegantString();
        o << std::endl;
      }
    }
  }

  void ErrorsContainer::elegantErrorsAndWarnings(std::ostream& o) const {
    if (count() == 0) return;

    for (Idx i = 0; i < count(); i++) {
      o << error(i).toElegantString();
      o << std::endl;
    }
  }

}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
