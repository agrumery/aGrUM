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


/** @file
 * @brief the class for computing AIC scores
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <sstream>

#  include <agrum/BN/learning/scores_and_tests/scoreAIC.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE ScoreAIC::ScoreAIC(const DBRowGeneratorParser&                                 parser,
                              const Prior&                                                prior,
                              const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                              const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Score(parser, prior, ranges, nodeId2columns),
        _internal_prior_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreAIC);
    }

    /// default constructor
    INLINE ScoreAIC::ScoreAIC(const DBRowGeneratorParser&             parser,
                              const Prior&                            prior,
                              const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Score(parser, prior, nodeId2columns), _internal_prior_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreAIC);
    }

    /// copy constructor
    INLINE ScoreAIC::ScoreAIC(const ScoreAIC& from) :
        Score(from), _internal_prior_(from._internal_prior_) {
      GUM_CONS_CPY(ScoreAIC);
    }

    /// move constructor
    INLINE ScoreAIC::ScoreAIC(ScoreAIC&& from) :
        Score(std::move(from)), _internal_prior_(std::move(from._internal_prior_)) {
      GUM_CONS_MOV(ScoreAIC);
    }

    /// virtual copy constructor
    INLINE ScoreAIC* ScoreAIC::clone() const { return new ScoreAIC(*this); }

    /// destructor
    INLINE ScoreAIC::~ScoreAIC() { GUM_DESTRUCTOR(ScoreAIC); }

    /// indicates whether the prior is compatible (meaningful) with the score
    INLINE std::string ScoreAIC::isPriorCompatible(const Prior& prior) {
      return isPriorCompatible(prior.getType(), prior.weight());
    }

    /// indicates whether the prior is compatible (meaningful) with the score
    INLINE std::string ScoreAIC::isPriorCompatible() const {
      return isPriorCompatible(*(this->prior_));
    }

    /// returns the internal prior of the score
    INLINE const Prior& ScoreAIC::internalPrior() const { return _internal_prior_; }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
