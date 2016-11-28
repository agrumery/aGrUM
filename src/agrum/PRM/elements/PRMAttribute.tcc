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
 * @brief Inline implementation of gum::PRMAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>::PRMAttribute( const std::string& name )
        : PRMClassElement<GUM_SCALAR>( name ) {
      GUM_CONSTRUCTOR( PRMAttribute );
    }

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>::PRMAttribute( const PRMAttribute& source )
        : PRMClassElement<GUM_SCALAR>( source ) {
      GUM_CONS_CPY( PRMAttribute );
      GUM_ERROR( OperationNotAllowed, "Cannot copy Attributes" );
    }

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>::~PRMAttribute() {
      GUM_DESTRUCTOR( PRMAttribute );
    }

    template <typename GUM_SCALAR>
    void PRMAttribute<GUM_SCALAR>::overload( PRMAttribute<GUM_SCALAR>* source ) {
      auto old_type = this->_type();
      this->_type( source->_type() );
      source->_type( old_type );
    }

  } /* namespace prm */
} /* namespace gum */
