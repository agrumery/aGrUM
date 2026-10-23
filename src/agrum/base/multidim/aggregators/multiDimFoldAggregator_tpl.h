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
 * @brief MultiDimFoldAggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

// to ease parser in IDEs
#include <agrum/base/multidim/aggregators/multiDimFoldAggregator.h>

namespace gum {
  namespace aggregator {

    template < typename GUM_SCALAR >
    MultiDimFoldAggregator< GUM_SCALAR >::MultiDimFoldAggregator() :
        MultiDimAggregator< GUM_SCALAR >() {
      GUM_CONSTRUCTOR(MultiDimFoldAggregator);
    }

    template < typename GUM_SCALAR >
    MultiDimFoldAggregator< GUM_SCALAR >::MultiDimFoldAggregator(
        const MultiDimFoldAggregator< GUM_SCALAR >& from) : MultiDimAggregator< GUM_SCALAR >(from) {
      GUM_CONS_CPY(MultiDimFoldAggregator);
    }

    template < typename GUM_SCALAR >
    MultiDimFoldAggregator< GUM_SCALAR >::~MultiDimFoldAggregator() {
      GUM_DESTRUCTOR(MultiDimFoldAggregator);
    }

    template < typename GUM_SCALAR >
    Idx MultiDimFoldAggregator< GUM_SCALAR >::buildValue_(const Instantiation& i) const {
      if (this->nbrDim() == 1) return neutralElt_();

      // is i equal to f(f(f(f...(j_,neutral_elt))))
      Idx current = neutralElt_();

      bool stop_iteration = false;

      for (Idx j = 1; j < this->nbrDim(); j++) {
        current = fold_(this->variable(j), i.val(this->variable(j)), current, stop_iteration);

        if (stop_iteration) break;
      }

      return current;
    }

  } /* namespace aggregator */
} /* namespace gum */
