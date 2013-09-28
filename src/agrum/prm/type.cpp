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
 * @brief Source implementation of type.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */

#include <agrum/prm/type.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/type.inl>
#endif //GUM_NO_INLINE

namespace gum {
  namespace prm {

    Type::Type( const DiscreteVariable& var ):
      PRMObject( var.name() ), __var( var.clone() ),
      __super( 0 ), __label_map( 0 ) {
      GUM_CONSTRUCTOR( Type );
    }

    Type::Type( Type& super_type, const std::vector<Idx>& label_map,
                const DiscreteVariable& var ):
      PRMObject( var.name() ), __var( var.clone() ),
      __super( &super_type ), __label_map( new std::vector<Idx>( label_map ) ) {
      GUM_CONSTRUCTOR( Type );

      if ( __isValid() ) {
        delete __label_map;
        GUM_ERROR( OperationNotAllowed, "Invalid label map." );
      }
    }

    Type::Type( const Type& from ):
      PRMObject( from ), __var( from.__var->clone() ),
      __super( from.__super ), __label_map( 0 ) {
      GUM_CONS_CPY( Type );

      if ( __super ) {
        __label_map = new std::vector<Idx>( from.label_map() );
      }
    }

    Type::~Type() {
      GUM_DESTRUCTOR( Type );
      delete __var;
      delete __label_map;
    }

    bool
    Type::isSubTypeOf( const Type& super ) const {
      if ( ( *this ) == super ) {
        return true;
      } else if ( __super ) {
        return __super->isSubTypeOf( super );
      } else {
        return false;
      }
    }

    bool
    Type::__isValid() const {
      if ( not __super ) {
        return __var->domainSize() > 1;
      }

      if ( __label_map->size() == __var->domainSize() ) {
        for ( size_t i = 0; i < __label_map->size(); ++i ) {
          if ( __label_map->at( i ) >= ( **__super ).domainSize() ) {
            return false;
          }
        }

        return true;
      }

      return false;
    }

  } /* namespace prm */
} /* namespace gum */

