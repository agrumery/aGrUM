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
 * @brief Inline implementation of gum::ClassElement
 *
 * @author Lionel TORTI
 */

namespace gum {
  namespace prm {

    INLINE
    NodeId
    ClassElement::id() const { return __id; }

    INLINE
    PRMObject::PRMType
    ClassElement::obj_type() const { return PRMType::CLASS_ELT; }

    INLINE
    void
    ClassElement::setId( NodeId id ) { __id = id; }

    INLINE
    const std::string&
    ClassElement::safeName() const {
      return _safeName;
    }

    INLINE
    std::string
    ClassElement::cast( const Type& t ) const {
      if ( type().isSubTypeOf( t ) ) {
        return ClassElement::LEFT_CAST() + t.name() + ClassElement::RIGHT_CAST() + name();
      } else {
        GUM_ERROR( OperationNotAllowed, "illegal cast" );
      }
    }


  } /* namespace prm */
} /* namespace gum */

