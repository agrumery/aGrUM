/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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


#include <cstdio>
#include <iostream>
#include <sstream>
#include <utility>

#include <agrum/agrum.h>

#include <agrum/base/core/exceptions.h>
#ifdef GUM_DEBUG_MODE
#  ifdef HAVE_EXECINFO_H
#    include <execinfo.h>
#  endif   // HAVE_EXECINFO_H
#endif     // GUM_DEBUG_MODE


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  std::string _createMsg_(const std::string& filename,
                          const std::string& function,
                          const int          line,
                          const std::string& msg) {
    std::stringstream stream;
#  ifdef GUM_FOR_SWIG
    stream << std::endl << msg << std::endl;
#  else    // GUM_FOR_SWIG
    stream << std::endl
           << filename << ":" << line << " in " << function << "()" << std::endl
           << "--------------" << std::endl
           << "! " << msg << std::endl
           << "--------------" << std::endl;
#  endif   // GUM_FOR_SWIG
    return stream.str();
  }

  Exception::Exception(const Exception& e) :
      std::exception(e), msg_(e.msg_), type_(e.type_), what_(e.what_) {}

  Exception::Exception(std::string aMsg, std::string aType) :
      std::exception(), msg_(std::move(aMsg)), type_(std::move(aType)) {
#  ifdef GUM_FOR_SWIG
    what_ = "[pyAgrum] " + type_ + ": " + msg_;
#  else    // GUM_FOR_SWIG
    what_ = type_ + ": " + msg_;
#  endif   // GUM_FOR_SWIG

#  ifdef GUM_DEBUG_MODE
#    ifdef HAVE_EXECINFO_H
#      define callStackDepth 20
    void*  array[callStackDepth];
    size_t size;
    char** strings;
    size    = backtrace(array, callStackDepth);
    strings = backtrace_symbols(array, size);

    std::stringstream stream;

    for (size_t i = 1; i < size; ++i) {
      stream << i << " :" << strings[i] << std::endl;
    }

    free(strings);
    callstack_ = stream.str();
#    else    // HAVE_EXECINFO_H
    callstack_ = "Callstack only in linux debug mode when execinfo.h available";
#    endif   // HAVE_EXECINFO_H
#  else      // GUM_DEBUG_MODE
    callstack_ = "Callstack only in linux debug mod ewhen execinfo.h available";
#  endif     // GUM_DEBUG_MODE
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
