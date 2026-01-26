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
 * @brief count aggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <sstream>

#include <agrum/base/multidim/aggregators/count.h>

namespace gum {

  namespace aggregator {
    template < typename GUM_SCALAR >
    INLINE Count< GUM_SCALAR >::Count(Idx value) :
        MultiDimAggregator< GUM_SCALAR >(), _value_(value) {
      GUM_CONSTRUCTOR(Count);
    }

    template < typename GUM_SCALAR >
    INLINE Count< GUM_SCALAR >::Count(const Count< GUM_SCALAR >& from) :
        MultiDimAggregator< GUM_SCALAR >(from) {
      _value_ = from._value_;
      GUM_CONS_CPY(Count);
    }

    template < typename GUM_SCALAR >
    INLINE Count< GUM_SCALAR >::~Count() {
      GUM_DESTRUCTOR(Count);
    }

    template < typename GUM_SCALAR >
    INLINE Idx Count< GUM_SCALAR >::neutralElt_() const {
      return (Idx)0;
    }

    template < typename GUM_SCALAR >
    INLINE Idx Count< GUM_SCALAR >::fold_(const DiscreteVariable& v,
                                          Idx                     i1,
                                          Idx                     i2,
                                          bool&                   stop_iteration) const {
      return (i1 == _value_) ? 1 + i2 : i2;
    }

    template < typename GUM_SCALAR >
    INLINE std::string Count< GUM_SCALAR >::aggregatorName() const {
      std::stringstream ss;
      ss << "count[" << _value_ << "]";
      return ss.str();
    }

    template < typename GUM_SCALAR >
    INLINE MultiDimContainer< GUM_SCALAR >* Count< GUM_SCALAR >::newFactory() const {
      return new Count< GUM_SCALAR >(_value_);
    }

  }   // namespace aggregator
}   // namespace gum
