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
 * @brief The mecanism to compute the next available graph changes for directed
 * structure learning search algorithms.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_GRAPH_CHANGES_SELECTOR_4_DIGRAPH_H
#define GUM_LEARNING_GRAPH_CHANGES_SELECTOR_4_DIGRAPH_H

#include <vector>

#include <agrum/core/set.h>
#include <agrum/core/sequence.h>
#include <agrum/core/priorityQueue.h>
#include <agrum/graphs/diGraph.h>
#include <agrum/learning/structureUtils/graphChange.h>

namespace gum {

  namespace learning {

    /** @class GraphChangesSelector4DiGraph
     * @brief The mecanism to compute the next available graph changes for
     * directed structure learning search algorithms.
     * @ingroup learning_group
     */
    template <typename SCORE, typename STRUCTURAL_CONSTRAINT,
              typename GRAPH_CHANGES_GENERATOR>
    class GraphChangesSelector4DiGraph {
      public:
      /// the type of the generator
      using GeneratorType = GRAPH_CHANGES_GENERATOR;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      GraphChangesSelector4DiGraph(SCORE &score, STRUCTURAL_CONSTRAINT &constraint,
                                   GRAPH_CHANGES_GENERATOR &changes_generator);

      /// copy constructor
      GraphChangesSelector4DiGraph(const GraphChangesSelector4DiGraph<
          SCORE, STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR> &from);

      /// move constructor
      GraphChangesSelector4DiGraph(GraphChangesSelector4DiGraph<
          SCORE, STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR> &&from);

      /// destructor
      ~GraphChangesSelector4DiGraph();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      GraphChangesSelector4DiGraph<SCORE, STRUCTURAL_CONSTRAINT,
                                   GRAPH_CHANGES_GENERATOR> &
      operator=(const GraphChangesSelector4DiGraph<SCORE, STRUCTURAL_CONSTRAINT,
                                                   GRAPH_CHANGES_GENERATOR> &from);

      /// move operator
      GraphChangesSelector4DiGraph<SCORE, STRUCTURAL_CONSTRAINT,
                                   GRAPH_CHANGES_GENERATOR> &
      operator=(GraphChangesSelector4DiGraph<SCORE, STRUCTURAL_CONSTRAINT,
                                             GRAPH_CHANGES_GENERATOR> &&from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the generator used by the selector
      GeneratorType &graphChangeGenerator() const noexcept;

      /// indicates whether the selector still contains graph changes
      bool empty();

      /** @brief indicates whether the selector contains graph changes related to
       * the ith node */
      bool empty(unsigned int i);

      /// returns the best graph change to examine
      /** @throws NotFound exception is thrown if the selector is empty */
      const GraphChange &bestChange();

      /// returns the best graph change to examine related to the ith node
      /** The selector computes not only the best change possible but also the
       * best changes impacting the parents' set of each node. This method
       * allows to get the change that is considered the best for modifying the
       * parents' set of the ith node.
       * @throws NotFound exception is thrown if the selector is empty */
      const GraphChange &bestChange(unsigned int i);

      /// return the score of the best graph change
      /** @throws NotFound exception is thrown if the selector is empty */
      float bestScore();

      /// return the score of the best graph change related to the ith node
      /** The selector computes not only the best change possible but also the
       * best changes impacting the parents' set of each node. This method
       * allows to get the score of the change that is considered the best for
       * modifying the parents' set of the ith node.
       * @throws NotFound exception is thrown if the selector is empty */
      float bestScore(unsigned int i);

      /// indicate to the selector that a change has been applied
      void applyChange(const GraphChange &change);

      /// indicate to the selector that one of serveral changes has been applied
      /** This function is to be used rather than applyChange when we wish to
       * apply several changes at a time. It is faster than applyChange because
       * it does not recomputes the scores. Then, after applying all changes,
       * we shall compute the scores with function
       * updateScoresAfterAppliedChanges (). See class GreedyHillClimbing for
       * an illustration of the use of this method. */
      void applyChangeWithoutScoreUpdate(const GraphChange &change);

      /// recompute all the scores after the application of several changes
      /** This method needs COMPULSORILY be used after applications of
       * applyChangeWithoutScoreUpdate in order to ensure the fact that
       * functions bestScore and bestChange return correct answers. See class
       * GreedyHillClimbing for an illustration of the use of this method. */
      void updateScoresAfterAppliedChanges();

      /// indicates whether a given change is valid or not
      bool isChangeValid(const GraphChange &change) const;

      /// sets the graph from which scores are computed
      void setGraph(DiGraph &graph, const std::vector<unsigned int> &modal);

      /// returns the set of queues sorted by decreasing top priority
      std::vector<std::pair<unsigned int, float>> nodesSortedByBestScore() const;

      /// returns the set of queues top priorities
      std::vector<std::pair<unsigned int, float>> nodesUnsortedWithScore() const;

      /// @}

      private:
      /// the scoring function
      SCORE *__score;

      /// the set of constraints used to determine valid changes
      STRUCTURAL_CONSTRAINT *__constraint;

      /// the generator that returns the set of possible changes
      GRAPH_CHANGES_GENERATOR *__changes_generator;

      /// a sequence containing all the possible changes
      Sequence<GraphChange> __changes;

      /// the scores for the head and tail of all the changes
      /** the scores are indexed by their index in sequence __changes */
      std::vector<std::pair<float, float>> __change_scores;

      /// for each node, a priority queue sorting GraphChanges by decreasing score
      /** within each queue, the changes are determined by their index in
       * sequence __changes. */
      std::vector<PriorityQueue<unsigned int, float, std::greater<float>>>
          __change_queue_per_node;

      /// a global priority queue indicating for each node its best score
      PriorityQueue<unsigned int, float, std::greater<float>> __node_queue;

      /// the set of changes known to be currently illegal (due to the constraints)
      /** within each queue, the changes are determined by their index in
       * sequence __changes. */
      Set<unsigned int> __illegal_changes;

      /// the current score of each node
      std::vector<float> __node_current_scores;

      /// the set of parents of each node (speeds-up score computations)
      std::vector<std::vector<unsigned int>> __parents;

      /// indicates whether we need to recompute whether the queue is empty or not
      bool __queues_valid{false};

      /// the set of queues to update when applying several changes
      Set<unsigned int> __queues_to_update;

      /// prepare the next pack of  score computations
      void __addScoreToCompute(unsigned int change_index) const;

      /// indicates whether a given change is valid or not
      bool __isChangeValid(unsigned int index) const;

      /// put a change into the illegal set
      void __invalidateChange(unsigned int change_index);

      /// remove the now legal changes from the illegal set
      void __illegal2LegalChanges(Set<unsigned int> &changes_to_recompute);

      /// finds the changes that are affected by a given node modification
      void __findLegalChangesNeedingUpdate(Set<unsigned int> &changes_to_recompute,
                                           NodeId target_node);

      /// perform the necessary updates of the scores
      void __updateScores(const Set<unsigned int> &changes_to_recompute);

      /// get from the graph change generator a new set of changes
      void __getNewChanges();
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.tcc>

#endif /* GUM_LEARNING_GRAPH_CHANGES_SELECTOR_4_DIGRAPH_H */
