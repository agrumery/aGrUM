/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
 *   test $Id: $                                                           *
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
 * @brief implementation of the class generating the IDs of aGrUM's objects
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#include <sstream>
#include <string>
#include <agrum/gumInline.h>
#include <agrum/gumDebug.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS


// ================================================================================
/// transforms a gumId into a string
// ================================================================================
INLINE std::string gumId2string( const gumId& id ) {
  std::ostringstream str;
  str << id;
  return str.str();
}


// ================================================================================
/// returns a new "unique" ID
// ================================================================================
INLINE const gumId& gumIdFactory::getNewId()  {
  return ++max_ID;
}


// ================================================================================
/// returns the max of the IDs gneerated by the factory
// ================================================================================
INLINE const gumId& gumIdFactory::getMaxId() const  {
  return max_ID;
}


// ================================================================================
/// basic constructor
// ================================================================================
INLINE gumIdFactory::gumIdFactory( const gumId& id )  : max_ID( id ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumIdFactory );
}


// ================================================================================
/// copy constructor
// ================================================================================
INLINE gumIdFactory::gumIdFactory( const gumIdFactory& from )  :
    max_ID( from.max_ID ) {
  // for debugging purposes
  GUM_CONS_CPY( gumIdFactory );
}


// ================================================================================
/// copy operator
// ================================================================================
INLINE gumIdFactory& gumIdFactory::operator= ( const gumIdFactory& from ) {
  // for debugging purposes
  GUM_OP_CPY( gumIdFactory );

  if ( from.max_ID >= max_ID )
    max_ID = from.max_ID;
  else
    GUM_ERROR( gumIdError, "Factory's ID would decrease => potential problems" );

  return *this;
}


// ================================================================================
/// destructor
// ================================================================================
INLINE gumIdFactory::~gumIdFactory()  {
  // for debugging purposes
  GUM_DESTRUCTOR( gumIdFactory );
}

INLINE std::ostream& operator<< ( std::ostream& stream, std::pair<gumId,gumId>& val ) {
  stream << "(" << val.first << "," << val.second << ")";
  return stream;
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

