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
 * @brief The implementation of DBCells
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/DBCell.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/DBCell.inl>
#endif /* GUM_NO_INLINE */


namespace gum {

  
  namespace learning {


    /// copy constructor
    DBCell::DBCell ( const DBCell& from ) :
      __type ( from.__type ) {
      // for debugging
      GUM_CONS_CPY ( DBCell );
      
      switch ( from.__type ) {
      case EltType::FLOAT:
        __float = from.__float;
        break;
        
      case EltType::STRING:
        new(&__string) std::string { from.__string };
        break;

      case EltType::MISSING:
        break;

      default:
        GUM_ERROR ( TypeError, "type not handled in DBCell copy constructor" );
      }
    }

    
    /// copy operator
    DBCell& DBCell::operator= ( const DBCell& from ) {
      if ( __type == EltType::STRING && from.__type == EltType::STRING ) {
        __string = from.__string;
        return *this;
      }

      // if we stored previously a string, remove it
      if ( __type == EltType::STRING ) __string.std::string::~string ();

      // assign the scalar types
      switch ( from.__type ) {
      case EltType::FLOAT:
        __float = from.__float;
        break;
          
      case EltType::MISSING:
        break;
        
      default:
        GUM_ERROR ( TypeError, "type not handled in DBCell operator=" );
      }

      __type = from.__type;
      
      return *this;
    }


    /// safely sets the content of the DBCell with the best possible type
    void DBCell::setAgainTypeSafe ( const std::string& elt ) {
      // try first to set the DBCell with the current type
      switch ( __type ) {
      case EltType::FLOAT:
        // try to convert the string into a float
        try { __setFloatFromStringSafe ( elt ); return; }
        catch ( std::invalid_argument& ) {}
        break;

      case EltType::STRING:
        // here, the best type is a string
        setStringSafe ( elt );
        return;

      case EltType::MISSING:
        break;
        
      default:
        GUM_ERROR ( TypeError, "type not handled in DBCell copy constructor" );
      }

      // here, set the content of the DBCell with the best type
      setBestTypeSafe ( elt );
    }

    
  } /* namespace learning */

  
} /* namespace gum */
