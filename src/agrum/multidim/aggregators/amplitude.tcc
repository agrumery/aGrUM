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
 * @brief median aggregator
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

#include <sstream>
#include <vector>

// to ease parser in IDEs
#include <agrum/multidim/aggregators/amplitude.h>
#include <agrum/multidim/aggregators/multiDimAggregator.h>

namespace gum {

  namespace aggregator {
    template <typename GUM_SCALAR>
    INLINE Amplitude<GUM_SCALAR>::Amplitude()
        : MultiDimAggregator<GUM_SCALAR>() {
      GUM_CONSTRUCTOR( Amplitude )
    }

    template <typename GUM_SCALAR>
    INLINE Amplitude<GUM_SCALAR>::Amplitude( const Amplitude<GUM_SCALAR>& from )
        : MultiDimAggregator<GUM_SCALAR>( from ) {
      GUM_CONS_CPY( Amplitude );
    }

    template <typename GUM_SCALAR>
    INLINE Amplitude<GUM_SCALAR>::~Amplitude() {
      GUM_DESTRUCTOR( Amplitude );
    }

    template <typename GUM_SCALAR>
    INLINE std::string Amplitude<GUM_SCALAR>::aggregatorName( void ) const {
      std::stringstream ss;
      ss << "amplitude";
      return ss.str();
    }

    template <typename GUM_SCALAR>
    INLINE MultiDimContainer<GUM_SCALAR>*
           Amplitude<GUM_SCALAR>::newFactory() const {
      return new Amplitude<GUM_SCALAR>();
    }

    template <typename GUM_SCALAR>
    Idx Amplitude<GUM_SCALAR>::_buildValue( const gum::Instantiation& i ) const {
      // we assume that every (parent) variable has the same domainSize
      if ( i.nbrDim() < 2 ) return 0;

      Idx min = i.val( 1 );
      Idx max = i.val( 1 );

      for ( Idx j = 2; j < this->nbrDim(); j++ ) {
        Idx m = i.val( j );
        if ( m < min ) min = m;
        if ( m > max ) max = m;
      }
      return max - min;  // multiDimAggregator::get will truncate if needed.
    }

    template <typename GUM_SCALAR>
    INLINE Idx Amplitude<GUM_SCALAR>::_fold( const DiscreteVariable& v,
                                             Idx                     i1,
                                             Idx                     i2,
                                             bool& stop_iteration ) const {
      return 0;
    }

  }  // namespace aggregator
}  // namespace gum
