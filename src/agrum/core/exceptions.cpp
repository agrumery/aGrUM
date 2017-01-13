/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string.h>

#include <agrum/config.h>
#include <agrum/core/exceptions.h>
#ifndef NDEBUG
#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif  // HAVE_EXECINFO_H
#endif  // NDEBUG


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  const std::string __createMsg( const std::string& filename,
                                 const std::string& function,
                                 const int          line,
                                 const std::string& msg ) {
    std::stringstream stream;
    stream << std::endl
           << "<" << filename << "> " << function << "() #" << std::setw( 6 )
           << std::dec << line << " :" << std::endl
           << "--------------" << std::endl
           << "! " << msg << std::endl
           << "--------------" << std::endl;
    return stream.str();
  }
  Exception::Exception( const Exception& e )
      : _msg( e._msg )
      , _type( e._type ) {}

  Exception::Exception( const std::string aMsg, const std::string aType )
      : _msg( aMsg )
      , _type( aType ) {
#ifndef NDEBUG
#ifdef HAVE_EXECINFO_H
#define callStackDepth 20
    void*  array[callStackDepth];
    size_t size;
    char** strings;
    size = backtrace( array, callStackDepth );
    strings = backtrace_symbols( array, size );

    std::stringstream stream;

    for ( size_t i = 1; i < size; ++i ) {
      stream << i << " :" << strings[i] << std::endl;
    }

    free( strings );
    _callstack = stream.str();
#else   // HAVE_EXECINFO_H
    _callstack = "Callstack only in linux debug mode when execinfo.h available";
#endif  // HAVE_EXECINFO_H
#else   // NDEBUG
    _callstack = "Callstack only in linux debug mod ewhen execinfo.h available";
#endif  // NDEBUG
  }

} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
