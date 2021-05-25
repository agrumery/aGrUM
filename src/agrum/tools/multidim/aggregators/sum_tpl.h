/**
 *
 *   Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
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
 * @brief sum aggregator
 *
 * @author Gaspard DUCAMP
 */

#include <agrum/tools/multidim/aggregators/sum.h>

namespace gum {

  namespace aggregator {
    template < typename GUM_SCALAR >
    INLINE Sum< GUM_SCALAR >::Sum() : MultiDimAggregator< GUM_SCALAR >() {
      this->decomposable_ = true;
      GUM_CONSTRUCTOR(Sum);
    }

    template < typename GUM_SCALAR >
    INLINE Sum< GUM_SCALAR >::Sum(const Sum< GUM_SCALAR >& from) :
        MultiDimAggregator< GUM_SCALAR >(from) {
      GUM_CONS_CPY(Sum);
    }

    template < typename GUM_SCALAR >
    INLINE Sum< GUM_SCALAR >::~Sum() {
      GUM_DESTRUCTOR(Sum);
    }

    template < typename GUM_SCALAR >
    INLINE Idx Sum< GUM_SCALAR >::neutralElt_() const {
      return (Idx)0;
    }

    template < typename GUM_SCALAR >
    INLINE Idx Sum< GUM_SCALAR >::fold_(const DiscreteVariable& v,
                                        Idx                     i1,
                                        Idx                     i2,
                                        bool&                   stop_iteration) const {
      GUM_SCALAR max_val
         = this->variable((Idx)0).numerical(this->variable((Idx)0).domainSize() - 1);
      if ((i1 + i2) > max_val) {
        stop_iteration = true;
        return (Idx)max_val;
      }
      return i1 + i2;
    }

    template < typename GUM_SCALAR >
    INLINE std::string Sum< GUM_SCALAR >::aggregatorName() const {
      return "sum";
    }

    template < typename GUM_SCALAR >
    INLINE MultiDimContainer< GUM_SCALAR >* Sum< GUM_SCALAR >::newFactory() const {
      return new Sum< GUM_SCALAR >;
    }

  }   // namespace aggregator
}   // namespace gum