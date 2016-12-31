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
 * @brief Inline implementation of type.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */
#include <agrum/PRM/elements/PRMType.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    PRMType<GUM_SCALAR>::PRMType( const DiscreteVariable& var )
        : PRMObject( var.name() )
        , __var( var.clone() )
        , __superType( 0 )
        , __label_map( 0 ) {
      GUM_CONSTRUCTOR( PRMType );
    }

    template <typename GUM_SCALAR>
    PRMType<GUM_SCALAR>::PRMType( PRMType<GUM_SCALAR>&    super_type,
                                  const std::vector<Idx>& label_map,
                                  const DiscreteVariable& var )
        : PRMObject( var.name() )
        , __var( var.clone() )
        , __superType( &super_type )
        , __label_map( new std::vector<Idx>( label_map ) ) {
      GUM_CONSTRUCTOR( PRMType );

      if ( !__isValid() ) {
        delete __label_map;
        __label_map = 0;
        GUM_ERROR( OperationNotAllowed, "Invalid label map." );
      }
    }

    template <typename GUM_SCALAR>
    PRMType<GUM_SCALAR>::PRMType( const PRMType<GUM_SCALAR>& from )
        : PRMObject( from )
        , __var( from.__var->clone() )
        , __superType( from.__superType )
        , __label_map( 0 ) {
      GUM_CONS_CPY( PRMType );

      if ( __superType ) {
        __label_map = new std::vector<Idx>( from.label_map() );
      }
    }

    template <typename GUM_SCALAR>
    PRMType<GUM_SCALAR>::~PRMType() {
      GUM_DESTRUCTOR( PRMType );
      delete __var;
      if ( __label_map ) {
        delete __label_map;
      }
    }

    template <typename GUM_SCALAR>
    bool
    PRMType<GUM_SCALAR>::isSubTypeOf( const PRMType<GUM_SCALAR>& super ) const {
      if ( ( *this ) == super ) {
        return true;
      } else if ( __superType ) {
        return __superType->isSubTypeOf( super );
      } else {
        return false;
      }
    }

    template <typename GUM_SCALAR>
    bool PRMType<GUM_SCALAR>::__isValid() const {
      if ( !__superType ) {
        return __var->domainSize() > 1;
      }

      if ( __label_map->size() == __var->domainSize() ) {
        for ( size_t i = 0; i < __label_map->size(); ++i ) {
          if ( __label_map->at( i ) >= ( **__superType ).domainSize() ) {
            return false;
          }
        }

        return true;
      }

      return false;
    }

    template <typename GUM_SCALAR>
    INLINE PRMType<GUM_SCALAR>& PRMType<GUM_SCALAR>::superType() {
      if ( __superType ) {
        return *__superType;
      } else {
        GUM_ERROR( NotFound, "No super type for this type." );
      }
    }

    template <typename GUM_SCALAR>
    INLINE DiscreteVariable& PRMType<GUM_SCALAR>::variable() {
      return *__var;
    }

    template <typename GUM_SCALAR>
    INLINE const DiscreteVariable& PRMType<GUM_SCALAR>::variable() const {
      return *__var;
    }

    template <typename GUM_SCALAR>
    INLINE DiscreteVariable& PRMType<GUM_SCALAR>::operator*() {
      return *__var;
    }

    template <typename GUM_SCALAR>
    INLINE const DiscreteVariable& PRMType<GUM_SCALAR>::operator*() const {
      return *__var;
    }

    template <typename GUM_SCALAR>
    INLINE DiscreteVariable* PRMType<GUM_SCALAR>::operator->() {
      return __var;
    }

    template <typename GUM_SCALAR>
    INLINE DiscreteVariable const* PRMType<GUM_SCALAR>::operator->() const {
      return __var;
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject::prm_type PRMType<GUM_SCALAR>::obj_type() const {
      return PRMObject::prm_type::TYPE;
    }

    template <typename GUM_SCALAR>
    INLINE const std::string& PRMType<GUM_SCALAR>::name() const {
      return PRMObject::name();
    }

    template <typename GUM_SCALAR>
    INLINE const PRMType<GUM_SCALAR>& PRMType<GUM_SCALAR>::superType() const {
      if ( __superType ) {
        return *__superType;
      } else {
        GUM_ERROR( NotFound, "No super type for this type." );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMType<GUM_SCALAR>::setSuper( PRMType<GUM_SCALAR>& t ) {
      try {
        if ( t != superType() ) {
          GUM_ERROR( WrongType,
                     "The given type is not a valid super type for this Type" );
        }
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed, "This Type has no super Type" );
      }

      __superType = &t;
    }

    template <typename GUM_SCALAR>
    INLINE const std::vector<Idx>& PRMType<GUM_SCALAR>::label_map() const {
      if ( __label_map ) {
        return *__label_map;
      } else {
        GUM_ERROR( NotFound, "No super type for this type." );
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMType<GUM_SCALAR>::isSubType() const {
      return __superType != nullptr;
    }

    template <typename GUM_SCALAR>
    INLINE bool
    PRMType<GUM_SCALAR>::isSuperTypeOf( const PRMType<GUM_SCALAR>& t ) const {
      return t.isSubTypeOf( *this );
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMType<GUM_SCALAR>::operator==( const PRMObject& obj ) const {
      return name() == obj.name();
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMType<GUM_SCALAR>::operator!=( const PRMObject& obj ) const {
      return name() != obj.name();
    }

  } /* namespace prm */
} /* namespace gum */
