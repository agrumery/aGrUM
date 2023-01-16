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
 * @brief The mecanism to compute the next available graph changes for directed
 * structure learning search algorithms.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_GRAPH_CHANGES_SELECTOR_4_DIGRAPH_H
#define GUM_LEARNING_GRAPH_CHANGES_SELECTOR_4_DIGRAPH_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/BN/learning/scores_and_tests/score.h>

namespace gum {

  namespace learning {

    /** @class GraphChangesSelector4DiGraph
     * @brief The mecanism to compute the next available graph changes for
     * directed structure learning search algorithms.
     * @ingroup learning_group
     */
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    class GraphChangesSelector4DiGraph {
      public:
      /// the type of the generator
      using GeneratorType = GRAPH_CHANGES_GENERATOR;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      GraphChangesSelector4DiGraph(Score&                   score,
                                   STRUCTURAL_CONSTRAINT&   constraint,
                                   GRAPH_CHANGES_GENERATOR& changes_generator);

      /// copy constructor
      GraphChangesSelector4DiGraph(
         const GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >&
            from);

      /// move constructor
      GraphChangesSelector4DiGraph(
         GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >&& from);

      /// destructor
      ~GraphChangesSelector4DiGraph();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >&
         operator=(const GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                                       GRAPH_CHANGES_GENERATOR >& from);

      /// move operator
      GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >& operator=(
         GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >&& from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the generator used by the selector
      GeneratorType& graphChangeGenerator() const noexcept;

      /// indicates whether the selector still contains graph changes
      bool empty();

      /** @brief indicates whether the selector contains graph changes related
       * to
       * the ith node */
      bool empty(const NodeId i);

      /// returns the best graph change to examine
      /** @throws NotFound exception is thrown if the selector is empty */
      const GraphChange& bestChange();

      /// returns the best graph change to examine related to the ith node
      /** The selector computes not only the best change possible but also the
       * best changes impacting the parents' set of each node. This method
       * allows to get the change that is considered the best for modifying the
       * parents' set of the ith node.
       * @throws NotFound exception is thrown if the selector is empty */
      const GraphChange& bestChange(const NodeId i);

      /// return the score of the best graph change
      /** @throws NotFound exception is thrown if the selector is empty */
      double bestScore();

      /// return the score of the best graph change related to the ith node
      /** The selector computes not only the best change possible but also the
       * best changes impacting the parents' set of each node. This method
       * allows to get the score of the change that is considered the best for
       * modifying the parents' set of the ith node.
       * @throws NotFound exception is thrown if the selector is empty */
      double bestScore(const NodeId i);

      /// indicate to the selector that a change has been applied
      void applyChange(const GraphChange& change);

      /// indicate to the selector that one of serveral changes has been applied
      /** This function is to be used rather than applyChange when we wish to
       * apply several changes at a time. It is faster than applyChange because
       * it does not recomputes the scores. Then, after applying all changes,
       * we shall compute the scores with function
       * updateScoresAfterAppliedChanges (). See class GreedyHillClimbing for
       * an illustration of the use of this method. */
      void applyChangeWithoutScoreUpdate(const GraphChange& change);

      /// recompute all the scores after the application of several changes
      /** This method needs COMPULSORILY be used after applications of
       * applyChangeWithoutScoreUpdate in order to ensure the fact that
       * functions bestScore and bestChange return correct answers. See class
       * GreedyHillClimbing for an illustration of the use of this method. */
      void updateScoresAfterAppliedChanges();

      /// indicates whether a given change is valid or not
      bool isChangeValid(const GraphChange& change) const;

      /// sets the graph from which scores are computed
      void setGraph(DiGraph& graph);

      /// returns the set of queues sorted by decreasing top priority
      std::vector< std::pair< NodeId, double > > nodesSortedByBestScore() const;

      /// returns the set of queues top priorities
      std::vector< std::pair< NodeId, double > > nodesUnsortedWithScore() const;

      /// @}

      private:
      /// the scoring function
      Score* _score_;

      /// the set of constraints used to determine valid changes
      STRUCTURAL_CONSTRAINT* _constraint_;

      /// the generator that returns the set of possible changes
      GRAPH_CHANGES_GENERATOR* _changes_generator_;

      /// a sequence containing all the possible changes
      Sequence< GraphChange > _changes_;

      /// the scores for the head and tail of all the changes
      /** the scores are indexed by their index in sequence  _changes_ */
      std::vector< std::pair< double, double > > _change_scores_;

      /// for each node, a priority queue sorting GraphChanges by decreasing score
      /** within each queue, the changes are determined by their index in
       * sequence  _changes_. */
      NodeProperty< PriorityQueue< std::size_t, double, std::greater< double > > >
         _change_queue_per_node_;

      /// a global priority queue indicating for each node its best score
      PriorityQueue< NodeId, double, std::greater< double > > _node_queue_;

      /// the set of changes known to be currently illegal (due to the constraints)
      /** within each queue, the changes are determined by their index in
       * sequence  _changes_. */
      Set< std::size_t > _illegal_changes_;

      /// the current score of each node
      NodeProperty< double > _node_current_scores_;

      /// the set of parents of each node (speeds-up score computations)
      NodeProperty< std::vector< NodeId > > _parents_;

      /// indicates whether we need to recompute whether the queue is empty or not
      bool _queues_valid_{false};

      /// the set of queues to update when applying several changes
      Set< NodeId > _queues_to_update_;

      /// indicates whether a given change is valid or not
      bool _isChangeValid_(const std::size_t index) const;

      /// put a change into the illegal set
      void _invalidateChange_(const std::size_t change_index);

      /// remove the now legal changes from the illegal set
      void _illegal2LegalChanges_(Set< std::size_t >& changes_to_recompute);

      /// finds the changes that are affected by a given node modification
      void _findLegalChangesNeedingUpdate_(Set< std::size_t >& changes_to_recompute,
                                           const NodeId        target_node);

      /// perform the necessary updates of the scores
      void _updateScores_(const Set< std::size_t >& changes_to_recompute);

      /// get from the graph change generator a new set of changes
      void _getNewChanges_();
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/BN/learning/structureUtils/graphChangesSelector4DiGraph_tpl.h>

#endif /* GUM_LEARNING_GRAPH_CHANGES_SELECTOR_4_DIGRAPH_H */
