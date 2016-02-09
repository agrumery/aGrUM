/**************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Implementation for the O3ClassFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/newo3prm/O3ClassFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>::O3ClassFactory() {
        GUM_CONSTRUCTOR( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>::O3ClassFactory(
          const O3ClassFactory<GUM_SCALAR>& src ) {
        GUM_CONS_CPY( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>::O3ClassFactory(
          O3ClassFactory<GUM_SCALAR>&& src ) {
        GUM_CONS_MOV( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>::~O3ClassFactory() {
        GUM_DESTRUCTOR( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>& O3ClassFactory<GUM_SCALAR>::
      operator=( const O3ClassFactory<GUM_SCALAR>& src ) {
        return *this;
      }

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>& O3ClassFactory<GUM_SCALAR>::
      operator=( O3ClassFactory<GUM_SCALAR>&& src ) {
        return *this;
      }

    } // o3prm
  } // prm
} // gum

