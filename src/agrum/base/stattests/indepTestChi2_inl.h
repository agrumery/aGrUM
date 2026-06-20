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


/** @file
 * @brief the class for computing Chi2 scores
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/stattests/indepTestChi2.h>   // to ease IDE parser
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/base/stattests/idCondSet.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE IndepTestChi2::IndepTestChi2(
        const DBRowGeneratorParser&                                 parser,
        const Prior&                                                prior,
        const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
        const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        IndependenceTest(parser, prior, ranges, nodeId2columns) {
      GUM_CONSTRUCTOR(IndepTestChi2);
    }

    /// default constructor
    INLINE IndepTestChi2::IndepTestChi2(const DBRowGeneratorParser&             parser,
                                        const Prior&                            prior,
                                        const Bijection< NodeId, std::size_t >& nodeId2columns) :
        IndependenceTest(parser, prior, nodeId2columns) {
      GUM_CONSTRUCTOR(IndepTestChi2);
    }

    /// copy constructor
    INLINE IndepTestChi2::IndepTestChi2(const IndepTestChi2& from) : IndependenceTest(from) {
      GUM_CONS_CPY(IndepTestChi2);
    }

    /// move constructor
    INLINE IndepTestChi2::IndepTestChi2(IndepTestChi2&& from) noexcept :
        IndependenceTest(std::move(from)) {
      GUM_CONS_MOV(IndepTestChi2);
    }

    /// virtual copy constructor
    INLINE IndepTestChi2* IndepTestChi2::clone() const { return new IndepTestChi2(*this); }

    /// destructor
    INLINE IndepTestChi2::~IndepTestChi2() { GUM_DESTRUCTOR(IndepTestChi2); }

    /// returns the pair <statistics,pvalue> corresponding to a given IdCondSet
    INLINE std::pair< double, double >
        IndepTestChi2::statistics(NodeId var1, NodeId var2, const std::vector< NodeId >& rhs_ids) {
      return statistics_(IdCondSet(var1, var2, rhs_ids, false));
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
