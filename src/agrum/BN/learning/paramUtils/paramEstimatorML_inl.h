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
 * @brief the class for estimating parameters of CPTs using Maximum Likelihood
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE ParamEstimatorML::ParamEstimatorML(
       const DBRowGeneratorParser&                                 parser,
       const Prior&                                                external_prior,
       const Prior&                                                score_internal_prior,
       const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
       const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        ParamEstimator(parser, external_prior, score_internal_prior, ranges, nodeId2columns) {
      GUM_CONSTRUCTOR(ParamEstimatorML);
    }


    /// default constructor
    INLINE
    ParamEstimatorML::ParamEstimatorML(const DBRowGeneratorParser&             parser,
                                       const Prior&                            external_prior,
                                       const Prior&                            score_internal_prior,
                                       const Bijection< NodeId, std::size_t >& nodeId2columns) :
        ParamEstimator(parser, external_prior, score_internal_prior, nodeId2columns) {
      GUM_CONSTRUCTOR(ParamEstimatorML);
    }


    /// copy constructor
    INLINE ParamEstimatorML::ParamEstimatorML(const ParamEstimatorML& from) : ParamEstimator(from) {
      GUM_CONS_CPY(ParamEstimatorML);
    }


    /// move constructor
    INLINE ParamEstimatorML::ParamEstimatorML(ParamEstimatorML&& from) :
        ParamEstimator(std::move(from)) {
      GUM_CONS_MOV(ParamEstimatorML);
    }


    /// virtual copy constructor
    INLINE ParamEstimatorML* ParamEstimatorML::clone() const { return new ParamEstimatorML(*this); }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
