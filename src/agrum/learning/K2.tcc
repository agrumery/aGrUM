/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief The K2 algorithm
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <type_traits>

#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4K2.h>
#include <agrum/learning/paramUtils/DAG2BNLearner.h>

namespace gum {

  namespace learning {

    /// learns the structure of a Bayes net
    template <typename GRAPH_CHANGES_SELECTOR>
    DAG K2::learnStructure(GRAPH_CHANGES_SELECTOR &selector,
                           const std::vector<unsigned int> &modal, DAG initial_dag) {
      // check that we used a selector compatible with the K2 algorithm
      static_assert(
          std::is_base_of<__GraphChangesGenerator4K2,
                          typename GRAPH_CHANGES_SELECTOR::GeneratorType>::value,
          "K2 must be called with a K2-compliant Graph Change Generator");

      // check that the order passed in argument concerns all the nodes
      __checkOrder(modal);

      // get the generator and assign the order
      auto &generator = selector.graphChangeGenerator();
      generator.setOrder(__order);

      // use the greedy hill climbing algorithm to perform the search
      return GreedyHillClimbing::learnStructure(selector, modal, initial_dag);
    }

    /// learns the structure and the parameters of a BN
    template <typename GUM_SCALAR, typename GRAPH_CHANGES_SELECTOR,
              typename PARAM_ESTIMATOR, typename CELL_TRANSLATORS>
    BayesNet<GUM_SCALAR>
    K2::learnBN(GRAPH_CHANGES_SELECTOR &selector, PARAM_ESTIMATOR &estimator,
                const std::vector<std::string> &names,
                const std::vector<unsigned int> &modal,
                const CELL_TRANSLATORS &translator, DAG initial_dag) {
      // check that we used a selector compatible with the K2 algorithm
      static_assert(
          std::is_base_of<__GraphChangesGenerator4K2,
                          typename GRAPH_CHANGES_SELECTOR::GeneratorType>::value,
          "K2 must be called with a K2-compliant Graph Change Generator");

      // check that the order passed in argument concerns all the nodes
      __checkOrder(modal);

      // get the generator and assign the order
      auto &generator = selector.graphChangeGenerator();
      generator.setOrder(__order);

      // use the greedy hill climbing algorithm to perform the search
      return GreedyHillClimbing::learnBN<GUM_SCALAR>(selector, estimator, names,
                                                     modal, translator, initial_dag);
    }

  } /* namespace learning */

} /* namespace gum */
