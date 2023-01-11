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
 * @brief median aggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <sstream>

#include <vector>

// to ease parser in IDEs
#include <agrum/tools/multidim/aggregators/median.h>
#include <agrum/tools/multidim/aggregators/multiDimAggregator.h>

namespace gum {

  namespace aggregator {
    template < typename GUM_SCALAR >
    INLINE Median< GUM_SCALAR >::Median() : MultiDimAggregator< GUM_SCALAR >() {
      GUM_CONSTRUCTOR(Median);
    }

    template < typename GUM_SCALAR >
    INLINE Median< GUM_SCALAR >::Median(const Median< GUM_SCALAR >& from) :
        MultiDimAggregator< GUM_SCALAR >(from) {
      GUM_CONS_CPY(Median);
    }

    template < typename GUM_SCALAR >
    INLINE Median< GUM_SCALAR >::~Median() {
      GUM_DESTRUCTOR(Median);
    }

    template < typename GUM_SCALAR >
    INLINE std::string Median< GUM_SCALAR >::aggregatorName() const {
      std::stringstream ss;
      ss << "median";
      return ss.str();
    }

    template < typename GUM_SCALAR >
    INLINE MultiDimContainer< GUM_SCALAR >* Median< GUM_SCALAR >::newFactory() const {
      return new Median< GUM_SCALAR >();
    }

    template < typename GUM_SCALAR >
    Idx Median< GUM_SCALAR >::buildValue_(const gum::Instantiation& i) const {
      if (i.nbrDim() < 2) return i.nbrDim() / 2;   // arbitrary. Guess = (max-min)/2 .

      // we assume that every (parent) variable has the same domainSize
      Idx maxVal = i.variable(1).domainSize();

      std::vector< Idx > cum(maxVal, 0);

      for (Idx j = 1; j < this->nbrDim(); j++)
        cum[i.val(j)]++;

      Idx half = (this->nbrDim() + 1) / 2;   // 50% of the population

      Idx max = maxVal;
      for (Idx j = 0, sumcum = 0; j < maxVal; j++)
        if ((sumcum += cum[j]) >= half) {
          max = j;
          break;
        }
      Idx maxR = maxVal;
      for (Idx j = maxVal - 1, jj = 0, sumcum = 0; jj < maxVal; jj++, j--)
        if ((sumcum += cum[j]) >= half) {
          maxR = j;
          break;
        }

      // multiDimAggregator::get will truncate if needed.
      return (maxR + max) / 2;
    }

    template < typename GUM_SCALAR >
    INLINE Idx Median< GUM_SCALAR >::fold_(const DiscreteVariable& v,
                                           Idx                     i1,
                                           Idx                     i2,
                                           bool&                   stop_iteration) const {
      return 0;
    }

  }   // namespace aggregator
}   // namespace gum
