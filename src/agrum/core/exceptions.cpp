
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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


#include <cstdio>
#include <iostream>
#include <sstream>
#include <string.h>

#include <agrum/agrum.h>
#include <agrum/core/exceptions.h>
#ifdef GUM_DEBUG_MODE
#  ifdef HAVE_EXECINFO_H
#    include <execinfo.h>
#  endif   // HAVE_EXECINFO_H
#endif     // GUM_DEBUG_MODE


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  const std::string __createMsg(const std::string& filename,
                                const std::string& function,
                                const int          line,
                                const std::string& msg) {
    std::stringstream stream;
#  ifdef SWIG
    stream << std::endl << msg << std::endl;
#  else
    stream << std::endl
           << "<" << filename << "> " << function << "() #" << std::setw(6)
           << std::dec << line << " :" << std::endl
           << "--------------" << std::endl
           << "! " << msg << std::endl
           << "--------------" << std::endl;
#  endif   // SWIG
    return stream.str();
  }
  Exception::Exception(const Exception& e) : _msg(e._msg), _type(e._type) {}

  Exception::Exception(const std::string aMsg, const std::string aType) :
      _msg(aMsg), _type(aType) {
#  ifdef GUM_DEBUG_MODE
#    ifdef HAVE_EXECINFO_H
#      define callStackDepth 20
    void*  array[callStackDepth];
    size_t size;
    char** strings;
    size = backtrace(array, callStackDepth);
    strings = backtrace_symbols(array, size);

    std::stringstream stream;

    for (size_t i = 1; i < size; ++i) {
      stream << i << " :" << strings[i] << std::endl;
    }

    free(strings);
    _callstack = stream.str();
#    else    // HAVE_EXECINFO_H
    _callstack = "Callstack only in linux debug mode when execinfo.h available";
#    endif   // HAVE_EXECINFO_H
#  else      // GUM_DEBUG_MODE
    _callstack = "Callstack only in linux debug mod ewhen execinfo.h available";
#  endif     // GUM_DEBUG_MODE
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
