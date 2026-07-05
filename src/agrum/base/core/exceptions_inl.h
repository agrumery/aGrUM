/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once

#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/inline.h>
#include <agrum/base/core/types.h>

namespace gum {


  GUM_NODISCARD INLINE const char* Exception::what() const noexcept { return what_.c_str(); }

  GUM_NODISCARD INLINE std::string Exception::errorContent() const { return msg_; }

  GUM_NODISCARD INLINE std::string Exception::errorType() const { return type_; }

  GUM_NODISCARD INLINE std::string Exception::errorCallStack() const { return callstack_; }

  INLINE
  SyntaxError::SyntaxError(const std::string& aMsg,
                           std::string        aFilename,
                           Size               nol,
                           Size               noc,
                           const std::string& aType) :
      IOError(aMsg, aType), noLine_(nol), noCol_(noc), filename_(std::move(aFilename)) {
#ifdef GUM_FOR_SWIG
    what_ = "[pyAgrum] " + msg_;
#else    // GUM_FOR_SWIG
    std::ostringstream error_stream;
    error_stream << type_ << ":" << std::endl;
    error_stream << filename() << ":" << line() << "," << col() << " : " << msg_;
    what_ = error_stream.str();
#endif   // GUM_FOR_SWIG
  }

  GUM_NODISCARD INLINE Size SyntaxError::col() const { return noCol_; }

  GUM_NODISCARD INLINE Size SyntaxError::line() const { return noLine_; }

  GUM_NODISCARD INLINE std::string SyntaxError::filename() const { return filename_; }

}   // namespace gum
