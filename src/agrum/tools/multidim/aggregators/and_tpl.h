/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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
 * @brief and aggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <sstream>

#include <agrum/tools/multidim/aggregators/and.h>

namespace gum {

  namespace aggregator {
    template < typename GUM_SCALAR >
    INLINE And< GUM_SCALAR >::And() : MultiDimAggregator< GUM_SCALAR >() {
      this->decomposable_ = true;
      GUM_CONSTRUCTOR(And);
    }

    template < typename GUM_SCALAR >
    INLINE And< GUM_SCALAR >::And(const And< GUM_SCALAR >& from) :
        MultiDimAggregator< GUM_SCALAR >(from) {
      GUM_CONS_CPY(And);
    }

    template < typename GUM_SCALAR >
    INLINE And< GUM_SCALAR >::~And() {
      GUM_DESTRUCTOR(And);
    }

    template < typename GUM_SCALAR >
    INLINE Idx And< GUM_SCALAR >::neutralElt_() const {
      return (Idx)1;
    }

    template < typename GUM_SCALAR >
    INLINE Idx And< GUM_SCALAR >::fold_(const DiscreteVariable& v,
                                        Idx                     i1,
                                        Idx                     i2,
                                        bool&                   stop_iteration) const {
      if (i1 == 1) {
        return (Idx)1;
      } else {
        stop_iteration = true;
        return (Idx)0;
      }
    }

    template < typename GUM_SCALAR >
    INLINE std::string And< GUM_SCALAR >::aggregatorName() const {
      return std::string("and");
    }

    template < typename GUM_SCALAR >
    INLINE MultiDimContainer< GUM_SCALAR >* And< GUM_SCALAR >::newFactory() const {
      return new And< GUM_SCALAR >();
    }

  }   // namespace aggregator
}   // namespace gum
