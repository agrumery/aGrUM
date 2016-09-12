/**************************************************************************
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
 * @brief Inline implementation of gum::PRMClassElement
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/elements/PRMClassElement.h>
#include <agrum/PRM/elements/PRMType.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    PRMClassElement<GUM_SCALAR>::PRMClassElement( const std::string& name )
        : PRMObject( name ) {
      GUM_CONSTRUCTOR( PRMClassElement );
    }

    template <typename GUM_SCALAR>
    PRMClassElement<GUM_SCALAR>::PRMClassElement( const PRMClassElement& source )
        : PRMObject( source.name() )
        , __id( source.id() ) {
      GUM_CONS_CPY( PRMClassElement );
    }

    template <typename GUM_SCALAR>
    PRMClassElement<GUM_SCALAR>::~PRMClassElement() {
      GUM_DESTRUCTOR( PRMClassElement );
    }

    template <typename GUM_SCALAR>
    INLINE NodeId PRMClassElement<GUM_SCALAR>::id() const {
      return __id;
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject::prm_type PRMClassElement<GUM_SCALAR>::obj_type() const {
      return prm_type::CLASS_ELT;
    }

    template <typename GUM_SCALAR>
    INLINE void PRMClassElement<GUM_SCALAR>::setId( NodeId id ) {
      __id = id;
    }

    template <typename GUM_SCALAR>
    INLINE const std::string& PRMClassElement<GUM_SCALAR>::safeName() const {
      return _safeName;
    }

    template <typename GUM_SCALAR>
    INLINE std::string
    PRMClassElement<GUM_SCALAR>::cast( const PRMType<GUM_SCALAR>& t ) const {
      if ( type().isSubTypeOf( t ) ) {
        return PRMObject::LEFT_CAST() + t.name() + PRMObject::RIGHT_CAST() +
               name();
      } else {
        GUM_ERROR( OperationNotAllowed, "illegal cast" );
      }
    }

  } /* namespace prm */
} /* namespace gum */
