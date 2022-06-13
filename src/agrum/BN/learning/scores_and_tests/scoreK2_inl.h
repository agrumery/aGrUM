/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief the class for computing K2 scores
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/learning/scores_and_tests/scoreK2.h>
#  include <sstream>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE ScoreK2::ScoreK2(const DBRowGeneratorParser&                                 parser,
                            const Prior&                                                apriori,
                            const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                            const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Score(parser, apriori, ranges, nodeId2columns),
        _internal_apriori_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreK2);
    }


    /// default constructor
    INLINE ScoreK2::ScoreK2(const DBRowGeneratorParser&             parser,
                            const Prior&                            apriori,
                            const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Score(parser, apriori, nodeId2columns),
        _internal_apriori_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreK2);
    }


    /// copy constructor
    INLINE ScoreK2::ScoreK2(const ScoreK2& from) :
        Score(from), _internal_apriori_(from._internal_apriori_), _gammalog2_(from._gammalog2_) {
      GUM_CONS_CPY(ScoreK2);
    }


    /// move constructor
    INLINE ScoreK2::ScoreK2(ScoreK2&& from) :
        Score(std::move(from)), _internal_apriori_(std::move(from._internal_apriori_)),
        _gammalog2_(std::move(from._gammalog2_)) {
      GUM_CONS_MOV(ScoreK2);
    }


    /// virtual copy constructor
    INLINE ScoreK2* ScoreK2::clone() const { return new ScoreK2(*this); }


    /// destructor
    INLINE ScoreK2::~ScoreK2() { GUM_DESTRUCTOR(ScoreK2); }


    /// indicates whether the apriori is compatible (meaningful) with the score
    INLINE std::string ScoreK2::isPriorCompatible(const Prior& apriori) {
      return isPriorCompatible(apriori.getType(), apriori.weight());
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    INLINE std::string ScoreK2::isPriorCompatible() const {
      return isPriorCompatible(*(this->apriori_));
    }


    /// returns the internal apriori of the score
    INLINE const Prior& ScoreK2::internalApriori() const { return _internal_apriori_; }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
