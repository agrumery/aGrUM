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
 * @brief Class of signalers.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

#include <agrum/core/signal/signaler0.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  namespace __sig__ {

    BasicSignaler0::BasicSignaler0() { GUM_CONSTRUCTOR( BasicSignaler0 ); }

    BasicSignaler0::BasicSignaler0( const BasicSignaler0& s )
        : ISignaler( s ) {
      GUM_CONS_CPY( BasicSignaler0 );

      for ( const auto el : _connectors ) {
        el->target()->attachSignal__( this );
        _connectors.push_back( el->clone() );
      }
    }

    BasicSignaler0::~BasicSignaler0() {
      GUM_DESTRUCTOR( BasicSignaler0 );

      for ( const auto el : _connectors ) {
        el->target()->detachSignal__( this );
        delete el;
      }

      _connectors.clear();
    }

  }  // namespace __sig__

}  // namespace gum

#endif  // DOXYGEN_SHOULD_SKIP_THIS

#ifdef GUM_NO_INLINE
#include <agrum/core/signal/signaler0_inl.h>
#endif  // GUM_NO_INLINE
