
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief The K2 algorithm
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <type_traits>

#include <agrum/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4K2.h>

namespace gum {

  namespace learning {

    /// learns the structure of a Bayes net
    template < typename GRAPH_CHANGES_SELECTOR >
    DAG K2::learnStructure(GRAPH_CHANGES_SELECTOR& selector, DAG initial_dag) {
      // check that we used a selector compatible with the K2 algorithm
      static_assert(
         std::is_base_of< __GraphChangesGenerator4K2,
                          typename GRAPH_CHANGES_SELECTOR::GeneratorType >::value,
         "K2 must be called with a K2-compliant Graph Change Generator");

      // check that the order passed in argument concerns all the nodes
      //__checkOrder(modal);

      // get the generator and assign the order
      auto& generator = selector.graphChangeGenerator();
      generator.setOrder(__order);

      // use the greedy hill climbing algorithm to perform the search
      return GreedyHillClimbing::learnStructure(selector, initial_dag);
    }

    /// learns the structure and the parameters of a BN
    template < typename GUM_SCALAR,
               typename GRAPH_CHANGES_SELECTOR,
               typename PARAM_ESTIMATOR >
    BayesNet< GUM_SCALAR > K2::learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                       PARAM_ESTIMATOR&        estimator,
                                       DAG                     initial_dag) {
      // check that we used a selector compatible with the K2 algorithm
      static_assert(
         std::is_base_of< __GraphChangesGenerator4K2,
                          typename GRAPH_CHANGES_SELECTOR::GeneratorType >::value,
         "K2 must be called with a K2-compliant Graph Change Generator");

      // check that the order passed in argument concerns all the nodes
      //__checkOrder(modal);

      // get the generator and assign the order
      auto& generator = selector.graphChangeGenerator();
      generator.setOrder(__order);

      // use the greedy hill climbing algorithm to perform the search
      return GreedyHillClimbing::learnBN< GUM_SCALAR >(
         selector, estimator, initial_dag);
    }

  } /* namespace learning */

} /* namespace gum */
