/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/**
 * @file
 * @brief MultiDimValueAggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <algorithm>
#include <cmath>

// to ease parser in IDEs
#include <agrum/base/multidim/aggregators/multiDimValueAggregator.h>

namespace gum {
  namespace aggregator {

    template < typename GUM_SCALAR >
    MultiDimValueAggregator< GUM_SCALAR >::MultiDimValueAggregator() :
        MultiDimAggregator< GUM_SCALAR >() {
      GUM_CONSTRUCTOR(MultiDimValueAggregator);
    }

    template < typename GUM_SCALAR >
    MultiDimValueAggregator< GUM_SCALAR >::MultiDimValueAggregator(
        const MultiDimValueAggregator< GUM_SCALAR >& from) :
        MultiDimAggregator< GUM_SCALAR >(from) {
      GUM_CONS_CPY(MultiDimValueAggregator);
    }

    template < typename GUM_SCALAR >
    MultiDimValueAggregator< GUM_SCALAR >::~MultiDimValueAggregator() {
      GUM_DESTRUCTOR(MultiDimValueAggregator);
    }

    template < typename GUM_SCALAR >
    void MultiDimValueAggregator< GUM_SCALAR >::checkNumericalVariable_(
        const DiscreteVariable& v) const {
      if (!v.isNumerical()) {
        GUM_ERROR(TypeError,
                  "The aggregator " << this->aggregatorName() << " needs numerical variables, "
                                    << v.name() << " is labelized")
      }
    }

    template < typename GUM_SCALAR >
    void MultiDimValueAggregator< GUM_SCALAR >::checkHomogeneousVariable_(
        const DiscreteVariable& v) const {
      if (this->nbrDim() > 0 && v.isNumerical() != this->variable(0).isNumerical()) {
        GUM_ERROR(TypeError,
                  "The aggregator " << this->aggregatorName()
                                    << " cannot mix labelized and numerical variables ("
                                    << this->variable(0).name() << " and " << v.name() << ")")
      }
    }

    template < typename GUM_SCALAR >
    double MultiDimValueAggregator< GUM_SCALAR >::parentValue_(const Instantiation& i,
                                                               Idx                  j) const {
      const auto& v = this->variable(j);
      // fast path (no lookup of v in i) when i has the variables of the aggregator in its order
      if (j < i.nbrDim() && &i.variable(j) == &v) return v.numerical(i.val(j));
      return v.numerical(i.val(v));
    }

    template < typename GUM_SCALAR >
    Idx MultiDimValueAggregator< GUM_SCALAR >::valueToIndex_(double value) const {
      const auto& agg = this->variable((Idx)0);
      if (agg.isNumerical()) return agg.closestIndex(value);

      // rounded half down (as RangeVariable::closestIndex()), saturated at both bounds
      const auto last = static_cast< double >(agg.domainSize() - 1);
      return static_cast< Idx >(std::ceil(std::clamp(value, 0.0, last) - 0.5));
    }

  } /* namespace aggregator */
} /* namespace gum */
