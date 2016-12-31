/***************************************************************************
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
 * @brief Outlined implementatioh of gum::ApproximationSettings.
 *
 * ApproximationSettings provides as well 2 signals :
 *   - onProgress(int pourcent,double error)
 *   - onStop(std::string message)
 * @see gum::ApproximationListener for dedicated listener.
 *
 * @author Pierre-Henri WUILLEMIN
 */

#include <agrum/core/approximations/approximationScheme.h>

namespace gum {

  ApproximationScheme::ApproximationScheme( bool v )
      : _current_state( ApproximationSchemeSTATE::Undefined )
      , _eps( 5e-2 )
      , _enabled_eps( true )
      , _min_rate_eps( 1e-2 )
      , _enabled_min_rate_eps( true )
      , _max_time( 1. )
      , _enabled_max_time( false )
      , _max_iter( (Size)10000 )
      , _enabled_max_iter( true )
      , _burn_in( (Size)0 )
      , _period_size( (Size)1 )
      , _verbosity( v ) {
    GUM_CONSTRUCTOR( ApproximationScheme );
  }

  ApproximationScheme::~ApproximationScheme() {
    GUM_DESTRUCTOR( ApproximationScheme );
  }

}  // namespace gum

#ifdef GUM_NO_INLINE
#include <agrum/core/approximations/approximationScheme_inl.h>
#endif
