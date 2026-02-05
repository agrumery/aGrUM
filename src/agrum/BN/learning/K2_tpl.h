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
 * @brief The K2 algorithm
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/BN/learning/structureUtils/graphChange.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4K2.h>

#include <type_traits>

namespace gum {

  namespace learning {

    /// learns the structure of a Bayes net
    template < typename GRAPH_CHANGES_SELECTOR >
    DAG K2::learnStructure(GRAPH_CHANGES_SELECTOR& selector, DAG initial_dag) {
      // check that we used a selector compatible with the K2 algorithm
      static_assert(std::is_base_of< _GraphChangesGenerator4K2_,
                                     typename GRAPH_CHANGES_SELECTOR::GeneratorType >::value,
                    "K2 must be called with a K2-compliant Graph Change Generator");

      // check that the order passed in argument concerns all the nodes
      // __checkOrder(modal);

      // get the generator and assign the order
      auto& generator = selector.graphChangeGenerator();
      generator.setOrder(_order_);

      // use the greedy hill climbing algorithm to perform the search
      return GreedyHillClimbing::learnStructure(selector, initial_dag);
    }

    /// learns the structure and the parameters of a BN
    template < typename GUM_SCALAR, typename GRAPH_CHANGES_SELECTOR, typename PARAM_ESTIMATOR >
    BayesNet< GUM_SCALAR >
        K2::learnBN(GRAPH_CHANGES_SELECTOR& selector, PARAM_ESTIMATOR& estimator, DAG initial_dag) {
      // check that we used a selector compatible with the K2 algorithm
      static_assert(std::is_base_of< _GraphChangesGenerator4K2_,
                                     typename GRAPH_CHANGES_SELECTOR::GeneratorType >::value,
                    "K2 must be called with a K2-compliant Graph Change Generator");

      // check that the order passed in argument concerns all the nodes
      // __checkOrder(modal);

      // get the generator and assign the order
      auto& generator = selector.graphChangeGenerator();
      generator.setOrder(_order_);

      // use the greedy hill climbing algorithm to perform the search
      return GreedyHillClimbing::learnBN< GUM_SCALAR >(selector, estimator, initial_dag);
    }

  } /* namespace learning */

} /* namespace gum */
