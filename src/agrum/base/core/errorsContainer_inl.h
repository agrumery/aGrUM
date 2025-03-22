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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Inlined implementation of the basic methods of ErrorsContainer
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to ease automatic parsers
#include <agrum/base/core/errorsContainer.h>

namespace gum {

  INLINE
  void ErrorsContainer::Error(const std::wstring& filename, Idx line, Idx col, const wchar_t* msg) {
    add(ParseError(true, narrow(std::wstring(msg)), narrow(filename), line, col));
  }

  INLINE
  void ErrorsContainer::Warning(const std::wstring& filename,
                                Idx                 line,
                                Idx                 col,
                                const wchar_t*      msg) {
    add(ParseError(false, narrow(std::wstring(msg)), narrow(filename), line, col));
  }

  INLINE
  void ErrorsContainer::Exception(const std::wstring& filename, const wchar_t* msg) {
    add(ParseError(true, "Exception : " + narrow(std::wstring(msg)), narrow(filename), 0, 0));
  }

  INLINE
  void ErrorsContainer::add(ParseError error) {
    errors.push_back(error);

    if (error.is_error) error_count++;
    else warning_count++;
  }

  INLINE
  void ErrorsContainer::addError(const std::string& msg,
                                 const std::string& filename,
                                 Idx                line,
                                 Idx                col) {
    add(ParseError(true, msg, filename, line, col));
  }

  INLINE
  void ErrorsContainer::addWarning(const std::string& msg,
                                   const std::string& filename,
                                   Idx                line,
                                   Idx                col) {
    add(ParseError(false, msg, filename, line, col));
  }

  INLINE
  void ErrorsContainer::addException(const std::string& msg, const std::string& filename) {
    add(ParseError(true, msg, filename, 0, 0));
  }

  INLINE
  Size ErrorsContainer::count() const { return error_count + warning_count; }

  INLINE
  void ErrorsContainer::syntheticResults(std::ostream& o) const {
    o << "Errors : " << error_count << std::endl;
    o << "Warnings : " << warning_count << std::endl;
  }

}   // namespace gum
