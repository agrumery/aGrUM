/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief the class for computing BDeu scores
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/learning/scores_and_tests/scoreBDeu.h>
#  include <sstream>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE ScoreBDeu::ScoreBDeu(const DBRowGeneratorParser&                                 parser,
                                const Apriori&                                              apriori,
                                const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                                const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Score(parser, apriori, ranges, nodeId2columns),
        _internal_apriori_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreBDeu);
    }


    /// default constructor
    INLINE ScoreBDeu::ScoreBDeu(const DBRowGeneratorParser&             parser,
                                const Apriori&                          apriori,
                                const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Score(parser, apriori, nodeId2columns),
        _internal_apriori_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreBDeu);
    }


    /// copy constructor
    INLINE ScoreBDeu::ScoreBDeu(const ScoreBDeu& from) :
        Score(from), _internal_apriori_(from._internal_apriori_), _gammalog2_(from._gammalog2_) {
      GUM_CONS_CPY(ScoreBDeu);
    }


    /// move constructor
    INLINE ScoreBDeu::ScoreBDeu(ScoreBDeu&& from) :
        Score(std::move(from)), _internal_apriori_(std::move(from._internal_apriori_)),
        _gammalog2_(std::move(from._gammalog2_)) {
      GUM_CONS_MOV(ScoreBDeu);
    }


    /// virtual copy constructor
    INLINE ScoreBDeu* ScoreBDeu::clone() const { return new ScoreBDeu(*this); }


    /// destructor
    INLINE ScoreBDeu::~ScoreBDeu() { GUM_DESTRUCTOR(ScoreBDeu); }


    /// indicates whether the apriori is compatible (meaningful) with the score
    INLINE std::string ScoreBDeu::isAprioriCompatible(const Apriori& apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    INLINE std::string ScoreBDeu::isAprioriCompatible() const {
      return isAprioriCompatible(*(this->apriori_));
    }


    /// returns the internal apriori of the score
    INLINE const Apriori& ScoreBDeu::internalApriori() const { return _internal_apriori_; }


    /// sets the effective sample size of the internal apriori
    INLINE void ScoreBDeu::setEffectiveSampleSize(double ess) {
      if (ess < 0) {
        GUM_ERROR(OutOfBounds,
                  "The effective sample size of the BDeu's "
                  "internal apriori must be positive");
      } else {
        _internal_apriori_.setEffectiveSampleSize(ess);
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */