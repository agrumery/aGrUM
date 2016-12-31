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
 * @brief Inline implementation of gum::PRMFuncAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/elements/PRMType.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    INLINE PRMFuncAttribute<GUM_SCALAR>::PRMFuncAttribute(
        const std::string&                  name,
        const PRMType<GUM_SCALAR>&          type,
        MultiDimImplementation<GUM_SCALAR>* impl )
        : PRMScalarAttribute<GUM_SCALAR>( name, type, impl ) {
      std::stringstream str;
      str << "In FuncAttribute(" << name << ", " << type.name() << ", " << impl
          << ")";
      GUM_CONSTRUCTOR( PRMFuncAttribute );
    }

    template <typename GUM_SCALAR>
    INLINE PRMFuncAttribute<GUM_SCALAR>::~PRMFuncAttribute() {
      GUM_DESTRUCTOR( PRMFuncAttribute );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFuncAttribute<GUM_SCALAR>::addParent(
        const PRMClassElement<GUM_SCALAR>& elt ) {}

    template <typename GUM_SCALAR>
    INLINE void PRMFuncAttribute<GUM_SCALAR>::addChild(
        const PRMClassElement<GUM_SCALAR>& elt ) {}

    template <typename GUM_SCALAR>
    INLINE PRMFuncAttribute<GUM_SCALAR>::PRMFuncAttribute(
        const PRMFuncAttribute<GUM_SCALAR>& source )
        : PRMScalarAttribute<GUM_SCALAR>( source ) {
      GUM_CONS_CPY( PRMFuncAttribute );
      GUM_ERROR( FatalError, "illegal call to copy constructor" );
    }

    template <typename GUM_SCALAR>
    INLINE PRMFuncAttribute<GUM_SCALAR>& PRMFuncAttribute<GUM_SCALAR>::
    operator=( const PRMFuncAttribute<GUM_SCALAR>& from ) {
      GUM_ERROR( FatalError, "illegal call to copy operator" );
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFuncAttribute<GUM_SCALAR>::_setCpf( Potential<GUM_SCALAR>* cpf ) {
      GUM_ERROR( OperationNotAllowed,
                 "Cannot change CPF of a functional attribute" );
    }

  } /* namespace prm */
} /* namespace gum */
