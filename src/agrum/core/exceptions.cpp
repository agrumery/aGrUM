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
#include <iostream>
#include <sstream>

#ifndef NDEBUG
#include <execinfo.h>
#endif //NDEBUG

#include <agrum/core/exceptions.h>
#include <agrum/core/debug.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {
  const std::string __createMsg( const std::string filename,
                                 const std::string function,
                                 const int line,const std::string msg ) {
    char m[20];
    sprintf( m,"%6.6d",line );
    std::string res( "" );
    return "\n  <"+filename+"> "+function+"() #"+m+
      " :\n--------------\n! "+msg+"\n--------------\n";
  }

  
  Exception::Exception( const std::string aMsg,const std::string aType ) :
    _msg( aMsg ), _type( aType ) {
#ifndef NDEBUG
#define callStackDepth 20
      void *array[callStackDepth];
      size_t size;
      char **strings;
      size = backtrace( array, callStackDepth );
      strings = backtrace_symbols( array, size );

      std::stringstream stream;
      for ( size_t i = 1; i < size; ++i ) {
        stream<< i<<" :" <<strings[i]<<std::endl;
      }

      free( strings );
      _callstack=stream.str();
#else
      _callstack="Callstack only in debug mode";
#endif
  }

  
} /* namespace gum */


#endif  // DOXYGEN_SHOULD_SKIP_THIS
