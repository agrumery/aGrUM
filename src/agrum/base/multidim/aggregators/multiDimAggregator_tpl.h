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
 * @brief MultiDimAggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

// to ease parser in IDEs
#include <agrum/base/multidim/aggregators/multiDimAggregator.h>

namespace gum {
  namespace aggregator {

    // Default constructor
    template < typename GUM_SCALAR >
    INLINE MultiDimAggregator< GUM_SCALAR >::MultiDimAggregator() :
        MultiDimReadOnly< GUM_SCALAR >(), decomposable_(false) {
      GUM_CONSTRUCTOR(MultiDimAggregator);
    }

    // Default constructor
    template < typename GUM_SCALAR >
    INLINE MultiDimAggregator< GUM_SCALAR >::MultiDimAggregator(
        const MultiDimAggregator< GUM_SCALAR >& from) :
        MultiDimReadOnly< GUM_SCALAR >(from), decomposable_(from.decomposable_) {
      GUM_CONS_CPY(MultiDimAggregator);
    }

    // destructor
    template < typename GUM_SCALAR >
    INLINE MultiDimAggregator< GUM_SCALAR >::~MultiDimAggregator() {
      GUM_DESTRUCTOR(MultiDimAggregator);
    }

    template < typename GUM_SCALAR >
    Idx MultiDimAggregator< GUM_SCALAR >::buildValue_(const Instantiation& i) const {
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

    template < typename GUM_SCALAR >
    GUM_SCALAR MultiDimAggregator< GUM_SCALAR >::get(const Instantiation& i) const {
      if (this->nbrDim() < 1) {
        GUM_ERROR(OperationNotAllowed, "Not enough variable for an aggregator : " << *this)
      }

      const DiscreteVariable& agg     = this->variable((Idx)0);
      auto                    current = buildValue_(i);

      // truncate to fit in aggreegator domain size
      if (current >= agg.domainSize()) current = agg.domainSize() - 1;

      return (i.val(agg) == current) ? (GUM_SCALAR)1.0 : (GUM_SCALAR)0.0;
    }

    template < typename GUM_SCALAR >
    std::string MultiDimAggregator< GUM_SCALAR >::toString() const {
      std::stringstream s;
      s << this->variable(0) << "=" << aggregatorName() << "(";

      for (Idx i = 1; i < this->nbrDim(); i++) {
        if (i > 1) s << ",";

        s << this->variable(i);
      }

      s << ")";

      return s.str();
    }

    template < typename GUM_SCALAR >
    void MultiDimAggregator< GUM_SCALAR >::copyFrom(
        const MultiDimContainer< GUM_SCALAR >& src) const {
      if (auto p = dynamic_cast< const MultiDimAggregator< GUM_SCALAR >* >(&src)) {
        if (p->name() != this->name()) {
          GUM_ERROR(OperationNotAllowed,
                    "Can not copy from a " << p->name() << " to a " << this->name());
        }
      } else {
        MultiDimReadOnly< GUM_SCALAR >::copyFrom(src);
      }
      // it the types aree consistant, nothing to do...
    }

    template < typename GUM_SCALAR >
    INLINE bool MultiDimAggregator< GUM_SCALAR >::isDecomposable() const {
      return decomposable_;
    }

    // returns the name of the implementation
    template < typename GUM_SCALAR >
    const std::string& MultiDimAggregator< GUM_SCALAR >::name() const {
      static const std::string str = "MultiDimAggregator";
      return str;
    }

    // For friendly displaying the content of the variable.
    template < typename GUM_SCALAR >
    INLINE std::ostream& operator<<(std::ostream& s, const MultiDimAggregator< GUM_SCALAR >& ag) {
      return s << ag.toString();
    }
  } /* namespace aggregator */
} /* namespace gum */
