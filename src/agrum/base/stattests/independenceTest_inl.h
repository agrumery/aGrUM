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
 * @brief the base class for all the independence tests used for learning
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/base/stattests/independenceTest.h>   // to ease IDE parser
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE IndependenceTest::IndependenceTest(
        const DBRowGeneratorParser&                                 parser,
        const Prior&                                                prior,
        const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
        const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        CachedContingencyCounter(parser, prior, ranges, nodeId2columns),
        _domain_sizes_(parser.database().domainSizes()) {
      GUM_CONSTRUCTOR(IndependenceTest);
    }

    /// default constructor
    INLINE
    IndependenceTest::IndependenceTest(const DBRowGeneratorParser&             parser,
                                       const Prior&                            prior,
                                       const Bijection< NodeId, std::size_t >& nodeId2columns) :
        CachedContingencyCounter(parser, prior, nodeId2columns),
        _domain_sizes_(parser.database().domainSizes()) {
      GUM_CONSTRUCTOR(IndependenceTest);
    }

    /// copy constructor
    INLINE IndependenceTest::IndependenceTest(const IndependenceTest& from) :
        CachedContingencyCounter(from), _domain_sizes_(from._domain_sizes_) {
      GUM_CONS_CPY(IndependenceTest);
    }

    /// move constructor
    INLINE IndependenceTest::IndependenceTest(IndependenceTest&& from) :
        CachedContingencyCounter(std::move(from)), _domain_sizes_(std::move(from._domain_sizes_)) {
      GUM_CONS_MOV(IndependenceTest);
    }

    /// destructor
    INLINE IndependenceTest::~IndependenceTest() { GUM_DESTRUCTOR(IndependenceTest); }

    INLINE Size IndependenceTest::degreesOfFreedom_(std::size_t X_size,
                                                    std::size_t Y_size,
                                                    std::size_t Z_size,
                                                    std::size_t n_skipped) {
      const Size theoretical = Z_size * (X_size - 1) * (Y_size - 1);
      return (n_skipped < theoretical) ? theoretical - Size(n_skipped) : Size(1);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
