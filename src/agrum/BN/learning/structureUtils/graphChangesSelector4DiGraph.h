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

#include <agrum/base/core/sortedPriorityQueue.h>

#include <agrum/BN/learning/scores_and_tests/score.h>

namespace gum {

  namespace learning {

    /** @class GraphChangesSelector4DiGraph
     * @brief The mecanism to compute the next available graph changes for
     * directed structure learning search algorithms.
     * @tparam INVARIABLE_CONSTRAINT_TYPE invariable constraints are those constraints whose
     *         satisfaction does not depend on the current DiGraph. For instance topological
     *         ordering constraints always yield the same results whatever the digraph.
     * @tparam VARIABLE_CONSTRAINT_TYPE variable constraints are those constraints that
     *         require examining the current DiGraph to determine whether they are satisfied
     *         or not. For instance, limits on the indegrees of the nodes can be reached or
     *         not depending on the DiGraph of interest.
     * @ingroup learning_group
     */
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    class GraphChangesSelector4DiGraph {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /**
       * @brief default constructor
       *
       * @param score the score used to select the best next change
       * @param invariable_constraints
       * @param variable_constraints
       * @param changes_generator
       */
      GraphChangesSelector4DiGraph(Score&                      score,
                                   INVARIABLE_CONSTRAINT_TYPE& invariable_constraints,
                                   VARIABLE_CONSTRAINT_TYPE&   variable_constraints);

      /// copy constructor
      GraphChangesSelector4DiGraph(
          const GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                              VARIABLE_CONSTRAINT_TYPE >& from);

      /// move constructor
      GraphChangesSelector4DiGraph(GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                                                 VARIABLE_CONSTRAINT_TYPE >&& from);

      /// destructor
      ~GraphChangesSelector4DiGraph();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >&
          operator=(const GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                                        VARIABLE_CONSTRAINT_TYPE >& from);

      /// move operator
      GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >&
          operator=(GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                                  VARIABLE_CONSTRAINT_TYPE >&& from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the best graph change to examine
      /** @throws NotFound exception is thrown if the selector is empty */
      const GraphChange& bestChange();

      /// return the score of the best graph change
      /** @warning If you want both to get the bestScore and the bestChange,
       * consider first to get the bestChange and, then, the score of this
       * best change, it will be faster than calling both the bestChange and
       * bestScore methods.
       * @throws NotFound exception is thrown if the selector is empty */
      double bestDeltaScore();

      /// return the score of a given change
      /**
       * @param change the GraphChange whose delta score we want
       * @param internal_change If the change is stored in memory within the
       * selector (essentially, it results from a call to method bestChange),
       * then set internal_change to true. This will speed-up the computations.
       * Otherwise, set it to false.
       * @throws NotFound exception is thrown if the selector is empty */
      double deltaScore(const GraphChange& change, const bool internal_change = false) const;

      /// returns the current score of a node
      double score(const NodeId node) const;

      /// indicate to the selector that a change has been applied
      void applyChange(const GraphChange& change);

      /// indicates whether a given change is valid or not
      bool isChangeValid(const GraphChange& change) const;

      /// sets the graph from which scores are computed
      void setGraph(DiGraph& graph);

      /** @brief adds some nodes that were removed because they did not belong to
       * the database
       *
       * This function should be called at the end of the learning in order to
       * add to the learnt graph all the nodes that were removed from it because
       * we had no data over them
       */
      void finalizeGraph(DiGraph& graph);

      /// sets whether or not the selector allows the application of arc additions
      void useArcAdditions(bool use);

      /// sets whether or not the selector allows the application of arc deletions
      void useArcDeletions(bool use);

      /// sets whether or not the selector allows the application of arc reversals
      void useArcReversals(bool use);

      /// sets whether or not the selector allows the application of arc triangle deletions
      void useArcTriangleDeletions(bool use);

      /// indicates whether the selector allows the application of arc additions
      bool arcAdditionsEnabled() const;

      /// indicates whether the selector allows the application of arc deletions
      bool arcDeletionsEnabled() const;

      /// indicates whether the selector allows the application of arc reversals
      bool arcReversalsEnabled() const;

      /// indicates whether the selector allows the application of arc triangle deletions
      bool arcTriangleDeletionsEnabled() const;

      /// returns the set of invariable constraints used by the selector
      INVARIABLE_CONSTRAINT_TYPE& invariableConstraints();

      /// returns the set of variable constraints used by the selector
      VARIABLE_CONSTRAINT_TYPE& variableConstraints();

      /// @}

      private:
      /// the scoring function
      Score* _score_;

      /**
       * @brief the set of constraints which, for a given change, always return
       * the same output
       *
       * An example of such a constraint is the slice order constraint: if x and
       * y are in slices 1 and 2 respectively, it will always be impossible to
       * add an arc y->x, whatever the current graph. Mandatory arcs and
       * forbidden arcs are also constraints of this type.
       */
      INVARIABLE_CONSTRAINT_TYPE* _invariable_constraints_;

      /** @brief the set of constraints whose outputs can change depending on
       * the current graph
       *
       * An example of such a constraint is the indegree constraint: depending
       * on the current number of parents of node x, it may or may not be
       * possible to add a new arc incoming into x.
       */
      VARIABLE_CONSTRAINT_TYPE* _variable_constraints_;


      /// the graph that we learn
      DiGraph* _graph_{nullptr};

      /// the current score of each node
      NodeProperty< double > _node_scores_;

      /// the set of parents of each node (speeds-up score computations)
      NodeProperty< std::vector< NodeId > > _node_parents_;

      /**
       * @brief the graph changes sorted by decreasing delta score value
       *
       * The graph changes in this queue are those that are allowed according to
       * the _invariable_constraints_. Their priority corresponds to the delta
       * that should be added to the overall score of the graph if the graph
       * change is applied.
       * @warning Note that there is never any guarantee that the changes in this
       * queue are allowed according to the _variable_constraints_, hence, to look
       * for the next best change, we should iterate over this queue and, for
       * each element, check whether it is variable-allowed.
       */
      SortedPriorityQueue< GraphChange, double > _sorted_changes_;

      /**
       * @brief the set of nodes removed from the graph passed in argument to the
       * selector because they do not appear in the database
       */
      NodeSet _removed_nodes_;

      bool _use_arc_additions_{true};
      bool _use_arc_deletions_{true};
      bool _use_arc_reversals_{true};
      bool _use_arc_triangle_deletions_{true};

      /// computes the new score of node given that we added it a new parent
      double _scoreAfterAddingParent_(const NodeId node, const NodeId new_parent);

      /// computes the new score of node given that we added it two new parents
      double _scoreAfterAddingParents_(const NodeId node,
                                       const NodeId new_parent1,
                                       const NodeId new_parent2);

      /// computes the new score of node given that we removed a parent
      double _scoreAfterRemovingParent_(const NodeId node, const NodeId parent);

      /// computes the new score of node given that we removed two parents
      double _scoreAfterRemovingParents_(const NodeId node,
                                         const NodeId parent1,
                                         const NodeId parent2);


      /// adds an ArcAddition to _sorted_changes_ if possible
      /** @warning if the invariable constraints forbid this ArcAddition,
       * then nothing is done. In particular, no exception is raised. */
      void _addArcAdditionToSortedChanges_(const ArcAddition& change);

      /// adds an ArcDeletion to _sorted_changes_ if possible
      /** @warning if the invariable constraints forbid this ArcDeletion,
       * then nothing is done. In particular, no exception is raised. */
      void _addArcDeletionToSortedChanges_(const ArcDeletion& change);

      /// adds an ArcReversal to _sorted_changes_ if possible
      /** @warning if the invariable constraints forbid this ArcReversal,
       * then nothing is done. In particular, no exception is raised. */
      void _addArcReversalToSortedChanges_(const ArcReversal& change);

      /// adds an ArcTriangleDeletion1 to _sorted_changes_ if possible
      /** @warning if the invariable constraints forbid this ArcTriangleDeletion1,
       * then nothing is done. In particular, no exception is raised. */
      void _addArcTriangleDeletion1ToSortedChanges_(const ArcTriangleDeletion1& change);

      /// adds an ArcTriangleDeletion2 to _sorted_changes_ if possible
      /** @warning if the invariable constraints forbid this ArcTriangleDeletion2,
       * then nothing is done. In particular, no exception is raised. */
      void _addArcTriangleDeletion2ToSortedChanges_(const ArcTriangleDeletion2& change);


      /// updates the score of a given ArcAddition
      void _updateArcAdditionScore_(const NodeId tail, const NodeId head);

      /// updates the score of a given ArcDeletion
      void _updateArcDeletionScore_(const NodeId tail, const NodeId head);

      /// updates the score of a given ArcReversal
      void _updateArcReversalScore_(const NodeId tail, const NodeId head);

      /** @brief update the score of the triangle deletions which contain a node whose
       * neighborhood has changed */
      void _updateTriangleDeletionsScoresFromNeighborhood_(const NodeId changed_node);


      /// add all the possible ArcAdditions to initialize _sorted_changes_
      void _initSortedChangesWithArcAdditions_(const DiGraph& graph);

      /// add all the possible ArcDeletions to initialize _sorted_changes_
      void _initSortedChangesWithArcDeletions_(const DiGraph& graph);

      /// add all the possible ArcReversals to initialize _sorted_changes_
      void _initSortedChangesWithArcReversals_(const DiGraph& graph);

      /// add all the possible ArcTriangleDeletions to initialize _sorted_changes_
      void _initSortedChangesWithArcTriangleDeletions_(const DiGraph& graph);


      /// indicate to the selector that an ArcAddition has been applied
      void _applyArcAddition_(const ArcAddition& change, bool update_contraints);

      /// indicate to the selector that an ArcDeletion has been applied
      void _applyArcDeletion_(const ArcDeletion& change, bool update_contraints);

      /// indicate to the selector that an ArcReversal has been applied
      void _applyArcReversal_(const ArcReversal& change, bool update_contraints);

      /// indicate to the selector that an ArcTriangleDeletion1 has been applied
      void _applyArcTriangleDeletion1_(const ArcTriangleDeletion1& change, bool update_contraints);

      /// indicate to the selector that an ArcTriangleDeletion2 has been applied
      void _applyArcTriangleDeletion2_(const ArcTriangleDeletion2& change, bool update_contraints);
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/BN/learning/structureUtils/graphChangesSelector4DiGraph_tpl.h>

#endif /* GUM_LEARNING_GRAPH_CHANGES_SELECTOR_4_DIGRAPH_H */
