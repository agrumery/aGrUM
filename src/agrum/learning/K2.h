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
#ifndef GUM_LEARNING_K2_H
#define GUM_LEARNING_K2_H

#include <vector>
#include <string>

#include <agrum/core/sequence.h>
#include <agrum/graphs/DAG.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/learning/greedyHillClimbing.h>

namespace gum {

  namespace learning {

    /** @class K2
     * @brief The K2 algorithm
     * @ingroup learning_group
     */
    class K2 : private GreedyHillClimbing {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      K2();

      /// copy constructor
      K2(const K2 &from);

      /// move constructor
      K2(K2 &&from);

      /// destructor
      ~K2();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      K2 &operator=(const K2 &from);

      /// move operator
      K2 &operator=(K2 &&from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the approximation policy of the learning algorithm
      ApproximationScheme &approximationScheme();

      /// sets the order on the variables
      void setOrder(const Sequence<NodeId> &order);

      /// sets the order on the variables
      void setOrder(const std::vector<NodeId> &order);

      /// returns the current order
      const Sequence<NodeId> &order() const noexcept;

      /// learns the structure of a Bayes net
      /** @param A selector class that computes the best changes that can be
       * applied and that enables the user to get them very easily. Typically,
       * the selector is a GraphChangesSelector4DiGraph<SCORE, STRUCT_CONSTRAINT,
       * GRAPH_CHANGES_GENERATOR>.
       * @param modal the domain sizes of the random variables observed in the
       * database
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

      private:
      /// the order on the variable used for learning
      Sequence<NodeId> __order;

      /** @brief checks that the order passed to K2 is coherent with the variables
       * as specified by their modalities */
      void __checkOrder(const std::vector<unsigned int> &modal);
      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/K2.inl>
#endif /* GUM_NO_INLINE */

/// always include templated methods
#include <agrum/learning/K2.tcc>

#endif /* GUM_LEARNING_K2_H */
