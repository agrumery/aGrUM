/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief exists aggregator
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 */

#include <sstream>
// to ease parser in IDEs
#include <agrum/tools/multidim/aggregators/exists.h>

namespace gum {

  namespace aggregator {
    template < typename GUM_SCALAR >
    INLINE Exists< GUM_SCALAR >::Exists(Idx value) :
        MultiDimAggregator< GUM_SCALAR >(), value__(value) {
      GUM_CONSTRUCTOR(Exists);
    }

    template < typename GUM_SCALAR >
    INLINE Exists< GUM_SCALAR >::Exists(const Exists< GUM_SCALAR >& from) :
        MultiDimAggregator< GUM_SCALAR >(from) {
      value__ = from.value__;
      GUM_CONS_CPY(Exists);
    }

    template < typename GUM_SCALAR >
    INLINE Exists< GUM_SCALAR >::~Exists() {
      GUM_DESTRUCTOR(Exists);
    }

    template < typename GUM_SCALAR >
    INLINE Idx Exists< GUM_SCALAR >::neutralElt_() const {
      return (Idx)0;
    }

    template < typename GUM_SCALAR >
    INLINE Idx Exists< GUM_SCALAR >::fold_(const DiscreteVariable& v,
                                           Idx                     i1,
                                           Idx                     i2,
                                           bool& stop_iteration) const {
      if (i1 != value__) {
        return (Idx)0;
      } else {
        stop_iteration = true;
        return (Idx)1;
      }
    }

    template < typename GUM_SCALAR >
    INLINE

       std::string
       Exists< GUM_SCALAR >::aggregatorName() const {
      std::stringstream ss;
      ss << "exists[" << value__ << "]";
      return ss.str();
    }

    template < typename GUM_SCALAR >
    INLINE MultiDimContainer< GUM_SCALAR >*
           Exists< GUM_SCALAR >::newFactory() const {
      return new Exists< GUM_SCALAR >(value__);
    }

  }   // namespace aggregator
}   // namespace gum
