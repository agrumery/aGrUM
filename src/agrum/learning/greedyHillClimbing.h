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
 * @brief The greedy hill learning algorithm (for directed graphs)
 *
 * The GreedyHillClimbing class implements a greedy search in which the only the
 * graph changes that increase the global score are applied. Those that increase
 * it the more are applied first. The algorithm stops when no single change can
 * increase the score anymore.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_GREEDY_HILL_CLIMBING_H
#define GUM_LEARNING_GREEDY_HILL_CLIMBING_H

#include <vector>
#include <string>

#include <agrum/graphs/DAG.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/core/algorithms/approximationScheme/approximationScheme.h>

namespace gum {

  namespace learning {

    /** @class GreedyHillClimbing
     * @brief The greedy hill climbing learning algorithm (for directed graphs)
     *
     * The GreedyHillClimbing class implements a greedy search in which the only
     * the graph changes that increase the global score are applied. Those that
     * increase it the more are applied first. The algorithm stops when no single
     * change can increase the score anymore.
     * @ingroup learning_group
     */
    class GreedyHillClimbing : public ApproximationScheme {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      GreedyHillClimbing();

      /// copy constructor
      GreedyHillClimbing(const GreedyHillClimbing &from);

      /// move constructor
      GreedyHillClimbing(GreedyHillClimbing &&from);

      /// destructor
      ~GreedyHillClimbing();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      GreedyHillClimbing &operator=(const GreedyHillClimbing &from);

      /// move operator
      GreedyHillClimbing &operator=(GreedyHillClimbing &&from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the approximation policy of the learning algorithm
      ApproximationScheme &approximationScheme();

      /// learns the structure of a Bayes net
      /** @param A selector class that computes the best changes that can be
       * applied and that enables the user to get them very easily. Typically,
       * the selector is a GraphChangesSelector4DiGraph<SCORE, STRUCT_CONSTRAINT,
       * GRAPH_CHANGES_GENERATOR>.
       * @param modal the domain sizes of the random variables observed in the
       * database
       * @param N the max number of changes that decrease the score that we
       * allow to apply
       * @param initial_dag the DAG we start from for our learning */
      template <typename GRAPH_CHANGES_SELECTOR>
      DAG learnStructure(GRAPH_CHANGES_SELECTOR &selector,
                         const std::vector<unsigned int> &modal,
                         DAG initial_dag = DAG());

      /// learns the structure and the parameters of a BN
      template <typename GUM_SCALAR = float, typename GRAPH_CHANGES_SELECTOR,
                typename PARAM_ESTIMATOR, typename CELL_TRANSLATORS>
      BayesNet<GUM_SCALAR>
      learnBN(GRAPH_CHANGES_SELECTOR &selector, PARAM_ESTIMATOR &estimator,
              const std::vector<std::string> &names,
              const std::vector<unsigned int> &modal,
              const CELL_TRANSLATORS &translator, DAG initial_dag = DAG());

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/learning/greedyHillClimbing.tcc>

#endif /* GUM_LEARNING_GREEDY_HILL_CLIMBING_H */
