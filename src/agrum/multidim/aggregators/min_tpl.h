
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief min aggregator
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

// to ease parser in IDEs
#include <agrum/multidim/aggregators/min.h>
#include <agrum/multidim/aggregators/multiDimAggregator.h>

namespace gum {

  namespace aggregator {

    // Default constructor
    template < typename GUM_SCALAR >
    INLINE Min< GUM_SCALAR >::Min() : MultiDimAggregator< GUM_SCALAR >() {
      GUM_CONSTRUCTOR(Min);
    }

    // Default constructor
    template < typename GUM_SCALAR >
    INLINE Min< GUM_SCALAR >::Min(const Min< GUM_SCALAR >& from) :
        MultiDimAggregator< GUM_SCALAR >(from) {
      GUM_CONS_CPY(Min);
    }

    // destructor
    template < typename GUM_SCALAR >
    INLINE Min< GUM_SCALAR >::~Min() {
      GUM_DESTRUCTOR(Min);
    }

    template < typename GUM_SCALAR >
    INLINE Idx Min< GUM_SCALAR >::_neutralElt() const {
      return std::numeric_limits< Idx >::max();
    }

    template < typename GUM_SCALAR >
    INLINE Idx Min< GUM_SCALAR >::_fold(const DiscreteVariable& v,
                                        Idx                     i1,
                                        Idx                     i2,
                                        bool& stop_iteration) const {
      return (i1 < i2) ? i1 : i2;
    }

    template < typename GUM_SCALAR >
    INLINE std::string Min< GUM_SCALAR >::aggregatorName() const {
      return "min";
    }

    template < typename GUM_SCALAR >
    INLINE MultiDimContainer< GUM_SCALAR >* Min< GUM_SCALAR >::newFactory() const {
      return new Min< GUM_SCALAR >;
    }

  }   // namespace aggregator
}   // namespace gum
