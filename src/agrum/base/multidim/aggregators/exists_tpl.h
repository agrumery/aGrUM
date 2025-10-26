/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief exists aggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <sstream>
// to ease parser in IDEs
#include <agrum/base/multidim/aggregators/exists.h>

namespace gum {

  namespace aggregator {
    template < typename GUM_SCALAR >
    INLINE Exists< GUM_SCALAR >::Exists(Idx value) :
        MultiDimAggregator< GUM_SCALAR >(), _value_(value) {
      GUM_CONSTRUCTOR(Exists);
    }

    template < typename GUM_SCALAR >
    INLINE Exists< GUM_SCALAR >::Exists(const Exists< GUM_SCALAR >& from) :
        MultiDimAggregator< GUM_SCALAR >(from) {
      _value_ = from._value_;
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
                                           bool&                   stop_iteration) const {
      if (i1 != _value_) {
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
      ss << "exists[" << _value_ << "]";
      return ss.str();
    }

    template < typename GUM_SCALAR >
    INLINE MultiDimContainer< GUM_SCALAR >* Exists< GUM_SCALAR >::newFactory() const {
      return new Exists< GUM_SCALAR >(_value_);
    }

  }   // namespace aggregator
}   // namespace gum
