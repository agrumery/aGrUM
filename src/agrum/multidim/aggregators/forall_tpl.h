
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
 * @brief forall aggregator
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

#include <sstream>
// to ease parser in IDEs
#include <agrum/multidim/aggregators/forall.h>

namespace gum {

  namespace aggregator {
    template < typename GUM_SCALAR >
    INLINE Forall< GUM_SCALAR >::Forall(Idx value) :
        MultiDimAggregator< GUM_SCALAR >(), __value(value) {
      GUM_CONSTRUCTOR(Forall);
    }

    template < typename GUM_SCALAR >
    INLINE Forall< GUM_SCALAR >::Forall(const Forall< GUM_SCALAR >& from) :
        MultiDimAggregator< GUM_SCALAR >(from) {
      __value = from.__value;
      GUM_CONS_CPY(Forall);
    }

    template < typename GUM_SCALAR >
    INLINE Forall< GUM_SCALAR >::~Forall() {
      GUM_DESTRUCTOR(Forall);
    }

    template < typename GUM_SCALAR >
    INLINE Idx Forall< GUM_SCALAR >::_neutralElt() const {
      return (Idx)1;
    }

    template < typename GUM_SCALAR >
    INLINE Idx Forall< GUM_SCALAR >::_fold(const DiscreteVariable& v,
                                           Idx                     i1,
                                           Idx                     i2,
                                           bool& stop_iteration) const {
      if (i1 == __value) {
        return (Idx)1;
      } else {
        stop_iteration = true;
        return (Idx)0;
      }
    }

    template < typename GUM_SCALAR >
    INLINE

       std::string
       Forall< GUM_SCALAR >::aggregatorName() const {
      std::stringstream ss;
      ss << "forall[" << __value << "]";
      return ss.str();
    }

    template < typename GUM_SCALAR >
    INLINE MultiDimContainer< GUM_SCALAR >*
           Forall< GUM_SCALAR >::newFactory() const {
      return new Forall< GUM_SCALAR >(__value);
    }

  }   // namespace aggregator
}   // namespace gum
