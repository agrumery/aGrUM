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
#ifndef GUM_ERRORS_CONTAINERS_H
#define GUM_ERRORS_CONTAINERS_H

#include <iostream>
#include <fstream>
#include <vector>

#include <agrum/config.h>

#include <agrum/core/cocoR/common.h>

namespace gum {

  /**
   *
   */
  class ParseError {
    public:
    ParseError(bool is_error, const std::string &msg, int line);
    ParseError(bool is_error, const std::string &msg, const std::string &filename,
               int line, int col = 0);
    ParseError(bool is_error, const std::string &msg, const std::string &filename,
               const std::string &code, int line, int col = 0);

    bool is_error;
    int line;
    int colomn; // default 0
    std::string msg;
    std::string filename;     // default ""
    mutable std::string code; // default ""

    ///
    std::string toString() const;
    ///
    std::string toElegantString() const;
  };

  class ErrorsContainer {

    mutable std::vector<ParseError> errors;

    public:
    int error_count;   // number of errors detected
    int warning_count; // number of warnings detected

    ErrorsContainer();
    ErrorsContainer(const ErrorsContainer &cont);

    /// Add an error object to container.
    void add(ParseError error);

    /// Return the i-th error.
    /// May throw an exception if i >= count().
    ParseError error(int i) const;
    ParseError last() const;

    /// Add an error.
    void addError(const std::string &msg, const std::string &filename, int line,
                  int col);
    /// Add a warning.
    void addWarning(const std::string &msg, const std::string &filename, int line,
                    int col);
    /// Add an exception.
    void addException(const std::string &msg, const std::string &filename);

    /// Return the number of errors and warnings
    int count(void) const;

    /// Print errors on output stream.
    void syntheticResults(std::ostream &o) const;
    void simpleErrors(std::ostream &o) const;
    void simpleErrorsAndWarnings(std::ostream &o) const;
    void elegantErrors(std::ostream &o) const;
    void elegantErrorsAndWarnings(std::ostream &o) const;

    // Use by coco/R
    void Error(const std::wstring &filename, int line, int col, const wchar_t *msg);
    void Warning(const std::wstring &filename, int line, int col,
                 const wchar_t *msg);
    void Exception(const std::wstring &filename, const wchar_t *msg);

    // Deprecated.
    // Use error(i)->method().
    bool is_error(int i) const;
    int line(int i) const;
    int col(int i) const;
    std::wstring msg(int i) const;
    std::wstring filename(int i) const;

    ErrorsContainer operator+(const ErrorsContainer &cont) const;
    ErrorsContainer operator=(const ErrorsContainer &cont);
    ErrorsContainer operator+=(const ErrorsContainer &cont);

  }; // ErrorsContainer

} // namespace gum

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/core/errorsContainer.inl>
#endif /* GUM_NO_INLINE */

#endif // GUM_ERRORS_CONTAINERS_H
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
