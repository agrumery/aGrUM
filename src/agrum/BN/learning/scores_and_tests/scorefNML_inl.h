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
 * @brief the class for computing fNML scores
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/learning/scores_and_tests/scorefNML.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE ScorefNML::ScorefNML(const DBRowGeneratorParser&                                 parser,
                                const Apriori&                                              apriori,
                                const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                                const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Score(parser, apriori, ranges, nodeId2columns),
        _internal_apriori_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScorefNML);
    }


    /// default constructor
    INLINE ScorefNML::ScorefNML(const DBRowGeneratorParser&             parser,
                                const Apriori&                          apriori,
                                const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Score(parser, apriori, nodeId2columns),
        _internal_apriori_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScorefNML);
    }


    /// copy constructor
    INLINE ScorefNML::ScorefNML(const ScorefNML& from) :
        Score(from), _internal_apriori_(from._internal_apriori_) {
      GUM_CONS_CPY(ScorefNML);
    }


    /// move constructor
    INLINE ScorefNML::ScorefNML(ScorefNML&& from) :
        Score(std::move(from)), _internal_apriori_(std::move(from._internal_apriori_)) {
      GUM_CONS_MOV(ScorefNML);
    }


    /// virtual copy constructor
    INLINE ScorefNML* ScorefNML::clone() const { return ScorefNML(*this); }


    /// destructor
    INLINE ScorefNML::~ScorefNML() { GUM_DESTRUCTOR(ScorefNML); }


    /// indicates whether the apriori is compatible (meaningful) with the score
    INLINE std::string ScorefNML::isAprioriCompatible(const Apriori& apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    INLINE std::string ScorefNML::isAprioriCompatible() const {
      return isAprioriCompatible(*(this->apriori_));
    }


    /// returns the internal apriori of the score
    INLINE const Apriori& ScorefNML::internalApriori() const { return _internal_apriori_; }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
