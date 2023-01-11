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


/** @file
 * @brief the class for computing BD scores
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/learning/scores_and_tests/scoreBD.h>
#  include <sstream>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE ScoreBD::ScoreBD(const DBRowGeneratorParser&                                 parser,
                            const Prior&                                                prior,
                            const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                            const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Score(parser, prior, ranges, nodeId2columns),
        _internal_prior_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreBD);
    }


    /// default constructor
    INLINE ScoreBD::ScoreBD(const DBRowGeneratorParser&             parser,
                            const Prior&                            prior,
                            const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Score(parser, prior, nodeId2columns),
        _internal_prior_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreBD);
    }


    /// copy constructor
    INLINE ScoreBD::ScoreBD(const ScoreBD& from) :
        Score(from), _internal_prior_(from._internal_prior_), _gammalog2_(from._gammalog2_) {
      GUM_CONS_CPY(ScoreBD);
    }


    /// move constructor
    INLINE ScoreBD::ScoreBD(ScoreBD&& from) :
        Score(std::move(from)), _internal_prior_(std::move(from._internal_prior_)),
        _gammalog2_(std::move(from._gammalog2_)) {
      GUM_CONS_MOV(ScoreBD);
    }


    /// virtual copy constructor
    INLINE ScoreBD* ScoreBD::clone() const { return new ScoreBD(*this); }


    /// destructor
    INLINE ScoreBD::~ScoreBD() { GUM_DESTRUCTOR(ScoreBD); }


    /// indicates whether the prior is compatible (meaningful) with the score
    INLINE std::string ScoreBD::isPriorCompatible(const Prior& prior) {
      return isPriorCompatible(prior.getType(), prior.weight());
    }


    /// indicates whether the prior is compatible (meaningful) with the score
    INLINE std::string ScoreBD::isPriorCompatible() const {
      return isPriorCompatible(*(this->prior_));
    }


    /// returns the internal prior of the score
    INLINE const Prior& ScoreBD::internalPrior() const { return _internal_prior_; }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
