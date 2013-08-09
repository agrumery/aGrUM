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
 * @author Lionel TORTI
 *
 */

namespace gum {
  namespace prm {

    INLINE
    Type&
    Type::super() { return *__super; }

    INLINE
    DiscreteVariable&
    Type::variable() { return *__var; }

    INLINE
    const DiscreteVariable&
    Type::variable() const { return *__var; }

    INLINE
    DiscreteVariable&
    Type::operator*() { return *__var; }

    INLINE
    const DiscreteVariable&
    Type::operator*() const { return *__var; }

    INLINE
    DiscreteVariable*
    Type::operator->() const { return __var; }

    INLINE
    PRMObject::PRMType
    Type::obj_type() const { return PRMObject::PRMType::TYPE; }

    INLINE
    const std::string&
    Type::name() const { return PRMObject::name(); }

    INLINE
    const Type&
    Type::super() const {
      if ( __super ) {
        return *__super;
      } else {
        GUM_ERROR( NotFound, "No super type for this type." );
      }
    }

    INLINE
    void
    Type::setSuper( Type& t ) {
      try {
        if ( t != super() ) {
          GUM_ERROR( WrongType, "the given Type is not equal to this Type super." );
        }
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed, "this Type has no super Type" );
      }

      __super = &t;
    }

    INLINE
    const std::vector<Idx>&
    Type::label_map() const {
      if ( __label_map ) {
        return *__label_map;
      } else {
        GUM_ERROR( NotFound, "No super type for this type." );
      }
    }

    INLINE
    bool
    Type::isSubType() const { return __super; }

    INLINE
    bool
    Type::isSuperTypeOf( const Type& t ) const {
      return t.isSubTypeOf( *this );
    }

    INLINE
    bool
    Type::operator==( const PRMObject& obj ) const {
      return name() == obj.name();
    }

    INLINE
    bool
    Type::operator!=( const PRMObject& obj ) const {
      return name() != obj.name();
    }

  } /* namespace prm */
} /* namespace gum */

