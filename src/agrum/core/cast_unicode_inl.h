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

/**
 * @file
 * @brief Unicode helper functions.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef CAST_UNICODE_H
#define CAST_UNICODE_H

#include <iostream>
#include <locale>
#include <sstream>
#include <string>

#include <agrum/core/cast_unicode.h>

INLINE std::wstring widen( const std::string& str ) {
  std::wostringstream        wstm;
  const std::ctype<wchar_t>& ctfacet =
      std::use_facet<std::ctype<wchar_t>>( wstm.getloc() );

  for ( size_t i = 0; i < str.size(); ++i ) {
    wstm << ctfacet.widen( str[i] );
  }

  return wstm.str();
}

INLINE std::string narrow( const std::wstring& str ) {
  std::ostringstream      stm;
  const std::ctype<char>& ctfacet =
      std::use_facet<std::ctype<char>>( stm.getloc() );

  for ( std::size_t i = 0; i < str.size(); ++i ) {
    stm << ctfacet.narrow( str[i], 0 );
  }

  return stm.str();
}

#endif  // CAST_UNICODE_H
