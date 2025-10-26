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

    /// computes the parameters of a CPT
    INLINE std::vector< double >
           ParamEstimatorML::parameters(const NodeId                 target_node,
                                     const std::vector< NodeId >& conditioning_nodes) {
      return _parametersAndLogLikelihood_(target_node, conditioning_nodes, false).first;
    }

    /// computes the parameters of a CPT as well as its log-likelihood
    INLINE std::pair< std::vector< double >, double > ParamEstimatorML::parametersAndLogLikelihood(
        const NodeId                 target_node,
        const std::vector< NodeId >& conditioning_nodes) {
      return _parametersAndLogLikelihood_(target_node, conditioning_nodes, true);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
