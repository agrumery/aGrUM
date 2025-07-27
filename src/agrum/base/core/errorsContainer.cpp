/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Errors container (at least) for parser
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/agrum.h>

#include <agrum/base/core/errorsContainer.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/core/errorsContainer_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  ParseError::ParseError(bool is_error, const std::string& msg, Idx line) :
      is_error(is_error), line(line), column(0), msg(msg), filename(""), code("") {}

  ParseError::ParseError(bool               is_error,
                         const std::string& msg,
                         const std::string& filename,
                         Idx                line,
                         Idx                col) :
      is_error(is_error), line(line), column(col), msg(msg), filename(filename), code("") {}

  ParseError::ParseError(bool               is_error,
                         const std::string& msg,
                         const std::string& filename,
                         const std::string& code,
                         Idx                line,
                         Idx                col) :
      is_error(is_error), line(line), column(col), msg(msg), filename(filename), code(code) {}

  ParseError::ParseError(const ParseError& err) {
    is_error = err.is_error;
    line     = err.line;
    column   = err.column;     // default 0
    msg      = err.msg;
    filename = err.filename;   // default ""
    code     = err.code;       // default ""
  }

  ParseError ParseError::operator=(const ParseError& err) {
    if (this != &err) {
      is_error = err.is_error;
      line     = err.line;
      column   = err.column;     // default 0
      msg      = err.msg;
      filename = err.filename;   // default ""
      code     = err.code;       // default ""
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
    if (count() > i) return errors[i];   // May throw an error if i >= count().
    else { GUM_ERROR(OutOfBounds, "Index out of bound.") }
  }

  ParseError ErrorsContainer::last() const {
    if (count() > 0) return errors[count() - 1];
    else { GUM_ERROR(OutOfBounds, "Index out of bound.") }
  }

  ErrorsContainer::ErrorsContainer() {
    error_count   = 0;
    warning_count = 0;
  }

  ErrorsContainer::ErrorsContainer(const ErrorsContainer& cont) {
    error_count   = cont.error_count;
    warning_count = cont.warning_count;
    errors.clear();
    errors = cont.errors;
  }

  ErrorsContainer ErrorsContainer::operator+(const ErrorsContainer& cont) const {
    ErrorsContainer newCont;

    newCont.error_count   = this->error_count + cont.error_count;
    newCont.warning_count = this->warning_count + cont.warning_count;
    std::copy(this->errors.begin(), this->errors.end(), newCont.errors.begin());
    std::copy(cont.errors.begin(), cont.errors.end(), newCont.errors.end());

    return newCont;
  }

  ErrorsContainer ErrorsContainer::operator=(const ErrorsContainer& cont) {
    error_count   = cont.error_count;
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
