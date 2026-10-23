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
 * @brief median aggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <vector>

// to ease parser in IDEs
#include <agrum/base/multidim/aggregators/median.h>
#include <agrum/base/multidim/aggregators/multiDimAggregator.h>

namespace gum {

  namespace aggregator {
    template < GUM_Numeric GUM_SCALAR >
    Median< GUM_SCALAR >::Median() : MultiDimValueAggregator< GUM_SCALAR >() {
      GUM_CONSTRUCTOR(Median);
    }

    template < GUM_Numeric GUM_SCALAR >
    Median< GUM_SCALAR >::Median(const Median< GUM_SCALAR >& from) :
        MultiDimValueAggregator< GUM_SCALAR >(from) {
      GUM_CONS_CPY(Median);
    }

    template < GUM_Numeric GUM_SCALAR >
    Median< GUM_SCALAR >::~Median() {
      GUM_DESTRUCTOR(Median);
    }

    template < GUM_Numeric GUM_SCALAR >
    std::string Median< GUM_SCALAR >::aggregatorName() const {
      return "median";
    }

    template < GUM_Numeric GUM_SCALAR >
    MultiDimContainer< GUM_SCALAR >* Median< GUM_SCALAR >::newFactory() const {
      return new Median< GUM_SCALAR >();
    }

    template < GUM_Numeric GUM_SCALAR >
    Idx Median< GUM_SCALAR >::buildValue_(const gum::Instantiation& i) const {
      if (this->nbrDim() < 2) return 0;

      // buffer reused between calls (get() is called for every configuration)
      thread_local std::vector< double > values;
      values.clear();
      for (Idx j = 1; j < this->nbrDim(); j++)
        values.push_back(this->parentValue_(i, j));
      std::sort(values.begin(), values.end());

      // for an even number of parents: the middle of the two medians (rounded half down)
      const auto n = values.size();
      return this->valueToIndex_((values[(n - 1) / 2] + values[n / 2]) / 2.0);
    }

    template < GUM_Numeric GUM_SCALAR >
    void Median< GUM_SCALAR >::checkVariable_(const DiscreteVariable& v) const {
      this->checkHomogeneousVariable_(v);
    }

  }   // namespace aggregator
}   // namespace gum
