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
 * @brief The mecanism to compute the next available graph changes for directed
 * structure learning search algorithms.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <limits>

namespace gum {

  namespace learning {

    /// default constructor
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        GraphChangesSelector4DiGraph(Score&                      score,
                                     INVARIABLE_CONSTRAINT_TYPE& invariable_constraints,
                                     VARIABLE_CONSTRAINT_TYPE&   variable_constraints) :
        _score_(&score), _invariable_constraints_(&invariable_constraints),
        _variable_constraints_(&variable_constraints) {
      GUM_CONSTRUCTOR(GraphChangesSelector4DiGraph);
    }

    /// copy constructor
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        GraphChangesSelector4DiGraph(
            const GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                                VARIABLE_CONSTRAINT_TYPE >& from) :
        _score_(from._score_), _invariable_constraints_(from._invariable_constraints_),
        _variable_constraints_(from._variable_constraints_), _graph_(from._graph_),
        _node_scores_(from._node_scores_), _node_parents_(from._node_parents_),
        _sorted_changes_(from._sorted_changes_), _removed_nodes_(from._removed_nodes_),
        _use_arc_additions_(from._use_arc_additions_),
        _use_arc_deletions_(from._use_arc_deletions_),
        _use_arc_reversals_(from._use_arc_reversals_),
        _use_arc_triangle_deletions_(from._use_arc_triangle_deletions_) {
      // for debugging
      GUM_CONS_CPY(GraphChangesSelector4DiGraph);
    }

    /// move constructor
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        GraphChangesSelector4DiGraph(
            GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >&&
                from) :
        _score_(from._score_), _invariable_constraints_(from._invariable_constraints_),
        _variable_constraints_(from._variable_constraints_), _graph_(from._graph_),
        _node_scores_(std::move(from._node_scores_)),
        _node_parents_(std::move(from._node_parents_)),
        _sorted_changes_(std::move(from._sorted_changes_)),
        _removed_nodes_(std::move(from._removed_nodes_)),
        _use_arc_additions_(from._use_arc_additions_),
        _use_arc_deletions_(from._use_arc_deletions_),
        _use_arc_reversals_(from._use_arc_reversals_),
        _use_arc_triangle_deletions_(from._use_arc_triangle_deletions_) {
      // for debugging
      GUM_CONS_MOV(GraphChangesSelector4DiGraph);
    }

    /// destructor
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                      VARIABLE_CONSTRAINT_TYPE >::~GraphChangesSelector4DiGraph() {
      GUM_DESTRUCTOR(GraphChangesSelector4DiGraph);
    }

    /// copy operator
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >&
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            operator=(const GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                                          VARIABLE_CONSTRAINT_TYPE >& from) {
      if (this != &from) {
        _score_                      = from._score_;
        _invariable_constraints_     = from._invariable_constraints_;
        _variable_constraints_       = from._variable_constraints_;
        _graph_                      = from._graph_;
        _node_scores_                = from._node_scores_;
        _node_parents_               = from._node_parents_;
        _sorted_changes_             = from._sorted_changes_;
        _removed_nodes_              = from._removed_nodes_;
        _use_arc_additions_          = from._use_arc_additions_;
        _use_arc_deletions_          = from._use_arc_deletions_;
        _use_arc_reversals_          = from._use_arc_reversals_;
        _use_arc_triangle_deletions_ = from._use_arc_triangle_deletions_;
      }

      return *this;
    }

    /// move operator
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >&
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            operator=(GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                                    VARIABLE_CONSTRAINT_TYPE >&& from) {
      if (this != &from) {
        _score_                      = from._score_;
        _invariable_constraints_     = from._invariable_constraints_;
        _variable_constraints_       = from._variable_constraints_;
        _graph_                      = from._graph_;
        _node_scores_                = std::move(from._node_scores_);
        _node_parents_               = std::move(from._node_parents_);
        _sorted_changes_             = std::move(from._sorted_changes_);
        _removed_nodes_              = std::move(from._removed_nodes_);
        _use_arc_additions_          = from._use_arc_additions_;
        _use_arc_deletions_          = from._use_arc_deletions_;
        _use_arc_reversals_          = from._use_arc_reversals_;
        _use_arc_triangle_deletions_ = from._use_arc_triangle_deletions_;
      }

      return *this;
    }

    /// returns the set of invariable constraints used by the selector
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE INVARIABLE_CONSTRAINT_TYPE&
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                      VARIABLE_CONSTRAINT_TYPE >::invariableConstraints() {
      return *_invariable_constraints_;
    }

    /// returns the set of variable constraints used by the selector
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE VARIABLE_CONSTRAINT_TYPE&
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                      VARIABLE_CONSTRAINT_TYPE >::variableConstraints() {
      return *_variable_constraints_;
    }

    /// sets the graph from which scores are computed
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                       VARIABLE_CONSTRAINT_TYPE >::setGraph(DiGraph& graph) {
      _graph_ = &graph;

      // remove the nodes of grapĥ that belong neither to the database nor to nodeId2Columns
      const DatabaseTable& database       = _score_->database();
      const auto&          nodeId2Columns = _score_->nodeId2Columns();

      _removed_nodes_.clear();
      if (nodeId2Columns.empty()) {
        const NodeId nb_nodes = NodeId(database.nbVariables());
        for (auto node: graph) {
          if (node >= nb_nodes) {
            graph.eraseNode(node);
            _removed_nodes_.insert(node);
          }
        }
      } else {
        for (auto node: graph) {
          if (!nodeId2Columns.existsFirst(node)) {
            graph.eraseNode(node);
            _removed_nodes_.insert(node);
          }
        }
      }

      // fill the DAG with all the missing nodes. This is convenient to allow the user
      // to just specify a partial graph
      if (nodeId2Columns.empty()) {
        const NodeId nb_nodes = NodeId(database.nbVariables());
        for (NodeId i = NodeId(0); i < nb_nodes; ++i) {
          if (!graph.existsNode(i)) { graph.addNodeWithId(i); }
        }
      } else {
        for (auto iter = nodeId2Columns.cbegin(); iter != nodeId2Columns.cend(); ++iter) {
          const NodeId id = iter.first();
          if (!graph.existsNode(id)) { graph.addNodeWithId(id); }
        }
      }

      // indicate to the constraints the new graph to which the learning will be applied
      _invariable_constraints_->setGraph(graph);
      _variable_constraints_->setGraph(graph);

      // save the set of parents of each node (this will speed-up the computations of the scores)
      const auto nb_nodes = graph.size();
      _node_parents_.clear();
      _node_parents_.resize(nb_nodes);
      {
        const std::vector< NodeId > empty_pars;
        for (const auto node: graph) {
          auto&          node_parents = _node_parents_.insert(node, empty_pars).second;
          const NodeSet& dag_parents  = graph.parents(node);
          if (!dag_parents.empty()) {
            node_parents.resize(dag_parents.size());
            Idx j = Idx(0);
            for (const auto par: dag_parents) {
              node_parents[j++] = par;
            }
          }
        }
      }

      // assign a score to each node given its parents in the current graph
      _score_->clearCache();
      _node_scores_.clear();
      _node_scores_.resize(nb_nodes);
      for (const auto node: graph) {
        _node_scores_.insert(node, _score_->score(node, _node_parents_[node]));
      }

      // compute all the possible changes
      _sorted_changes_.clear();
      _sorted_changes_.resize(nb_nodes * nb_nodes);

      if (_use_arc_additions_) _initSortedChangesWithArcAdditions_(graph);
      if (_use_arc_deletions_) _initSortedChangesWithArcDeletions_(graph);
      if (_use_arc_reversals_) _initSortedChangesWithArcReversals_(graph);
      if (_use_arc_triangle_deletions_) _initSortedChangesWithArcTriangleDeletions_(graph);
    }

    /// sets whether or not the selector allows the application of arc additions
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                       VARIABLE_CONSTRAINT_TYPE >::useArcAdditions(bool use) {
      if (use) {
        // do something only if we did not yet allow the application of arc additions
        if (!_use_arc_additions_) {
          // add the arc additions to _sorted_changes_
          if (_graph_ != nullptr) _initSortedChangesWithArcAdditions_(*_graph_);
          _use_arc_additions_ = true;
        }
      } else {
        // do something only if we currently allowed the application of arc additions
        if (_use_arc_additions_) {
          // remove the arc additions from _sorted_changes_
          for (auto& change: _sorted_changes_) {
            if (change.type() == GraphChangeType::ARC_ADDITION) {
              _sorted_changes_.erase(change, true);
            }
          }
          _use_arc_additions_ = false;
        }
      }
    }

    /// sets whether or not the selector allows the application of arc deletions
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                       VARIABLE_CONSTRAINT_TYPE >::useArcDeletions(bool use) {
      if (use) {
        // do something only if we did not yet allow the application of arc deletions
        if (!_use_arc_deletions_) {
          // add the arc deletions to _sorted_changes_
          if (_graph_ != nullptr) _initSortedChangesWithArcDeletions_(*_graph_);
          _use_arc_deletions_ = true;
        }
      } else {
        // do something only if we currently allowed the application of arc deletions
        if (_use_arc_deletions_) {
          // remove the arc deletions from _sorted_changes_
          for (auto& change: _sorted_changes_) {
            if (change.type() == GraphChangeType::ARC_DELETION) {
              _sorted_changes_.erase(change, true);
            }
          }
          _use_arc_deletions_ = false;
        }
      }
    }

    /// sets whether or not the selector allows the application of arc reversals
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                       VARIABLE_CONSTRAINT_TYPE >::useArcReversals(bool use) {
      if (use) {
        // do something only if we did not yet allow the application of arc reversals
        if (!_use_arc_reversals_) {
          // add the arc reversals to _sorted_changes_
          if (_graph_ != nullptr) _initSortedChangesWithArcReversals_(*_graph_);
          _use_arc_reversals_ = true;
        }
      } else {
        // do something only if we currently allowed the application of arc reversals
        if (_use_arc_reversals_) {
          // remove the arc reversals from _sorted_changes_
          for (auto& change: _sorted_changes_) {
            if (change.type() == GraphChangeType::ARC_REVERSAL) {
              _sorted_changes_.erase(change, true);
            }
          }
          _use_arc_reversals_ = false;
        }
      }
    }

    /// sets whether or not the selector allows the application of arc triangle deletions
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        useArcTriangleDeletions(bool use) {
      if (use) {
        // do something only if we did not yet allow the application of arc triangle deletions
        if (!_use_arc_triangle_deletions_) {
          // add the arc triangle deletions to _sorted_changes_
          if (_graph_ != nullptr) _initSortedChangesWithArcTriangleDeletions_(*_graph_);
          _use_arc_triangle_deletions_ = true;
        }
      } else {
        // do something only if we currently allowed the application of arc triangle deletions
        if (_use_arc_triangle_deletions_) {
          // remove the arc triangle deletions from _sorted_changes_
          for (auto& change: _sorted_changes_) {
            if ((change.type() == GraphChangeType::ARC_TRIANGLE_DELETION1)
                || (change.type() == GraphChangeType::ARC_TRIANGLE_DELETION2)) {
              _sorted_changes_.erase(change, true);
            }
          }
          _use_arc_triangle_deletions_ = false;
        }
      }
    }

    /// compute the new score of node given that we added it a new parent
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE double
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _scoreAfterAddingParent_(const NodeId node, const NodeId new_parent) {
      auto& parents = _node_parents_[node];
      parents.push_back(new_parent);
      const double new_score = _score_->score(node, parents);
      parents.pop_back();
      return new_score;
    }

    /// adds some nodes that were removed because they did not belong to the database
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                       VARIABLE_CONSTRAINT_TYPE >::finalizeGraph(DiGraph& graph) {
      for (const auto node: _removed_nodes_) {
        graph.addNodeWithId(node);
      }
    }

    /// compute the new score of node given that we removed a parent
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE double
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _scoreAfterRemovingParent_(const NodeId node, const NodeId parent) {
      auto& parents = _node_parents_[node];
      for (auto& par: parents) {
        if (par == parent) {
          par = *(parents.rbegin());
          parents.pop_back();
          break;
        }
      }
      const double new_score = _score_->score(node, parents);
      parents.push_back(parent);
      return new_score;
    }

    /// compute the new score of node given that we added it a new parent
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE double
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _scoreAfterAddingParents_(const NodeId node,
                                      const NodeId new_parent1,
                                      const NodeId new_parent2) {
      auto& parents = _node_parents_[node];
      parents.push_back(new_parent1);
      parents.push_back(new_parent2);
      const double new_score = _score_->score(node, parents);
      parents.pop_back();
      parents.pop_back();
      return new_score;
    }

    /// compute the new score of node given that we removed two parents
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE double
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _scoreAfterRemovingParents_(const NodeId node,
                                        const NodeId parent1,
                                        const NodeId parent2) {
      auto& parents = _node_parents_[node];
      for (auto& par: parents) {
        if (par == parent1) {
          par = *(parents.rbegin());
          parents.pop_back();
          break;
        }
      }
      for (auto& par: parents) {
        if (par == parent2) {
          par = *(parents.rbegin());
          parents.pop_back();
          break;
        }
      }
      const double new_score = _score_->score(node, parents);
      parents.push_back(parent1);
      parents.push_back(parent2);
      return new_score;
    }

    /// adds an ArcAddition to _sorted_changes_ if possible
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE void
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _addArcAdditionToSortedChanges_(const ArcAddition& change) {
      // only add the arc addition if this is allowed by the graph and the constraints
      if (!_invariable_constraints_->isAlwaysInvalid(change)) {
        const NodeId node1 = change.node1();
        const NodeId node2 = change.node2();

        // compute the delta in the score resulting from adding node1 to the set
        // of parents of the node2
        // currently invalid changes are assigned a -infty score
        const double delta = !_graph_->existsArc(Arc(node1, node2))
            ? _scoreAfterAddingParent_(node2, node1) - _node_scores_[node2]
            : std::numeric_limits< double >::lowest();

        if (delta == std::numeric_limits< double >::lowest()) {
          std::cout << "infAddAdd (" << node1 << ", " << node2 << ") = " << ++_nb_inf_ << std::endl;
        }

        _sorted_changes_.insert(change, delta);
      }
    }

    /// adds an ArcDeletion to _sorted_changes_ if possible
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE void
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _addArcDeletionToSortedChanges_(const ArcDeletion& change) {
      // only add the arc reversal if this is allowed by the constraints
      if (!_invariable_constraints_->isAlwaysInvalid(change)) {
        const NodeId tail = change.node1();
        const NodeId head = change.node2();

        // compute the delta in the score resulting from removing the tail from
        // the set of parents of the head
        // currently invalid changes are assigned a -infty score
        const double delta = _graph_->existsArc(Arc(tail, head))
            ? _scoreAfterRemovingParent_(head, tail) - _node_scores_[head]
            : std::numeric_limits< double >::lowest();

        if (delta == std::numeric_limits< double >::lowest()) {
          std::cout << "infAddDel (" << tail << ", " << head << ") = " <<  ++_nb_inf_ << std::endl;
        }
        _sorted_changes_.insert(change, delta);
      }
    }

    /// adds an ArcReversal to _sorted_changes_ if possible
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE void
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _addArcReversalToSortedChanges_(const ArcReversal& change) {
      // only add the arc reversal if this is allowed by the constraints
      if (!_invariable_constraints_->isAlwaysInvalid(change)) {
        const NodeId tail = change.node1();
        const NodeId head = change.node2();

        // compute the delta in the score resulting from removing the tail from the
        // set of parents of the head and that resulting from adding the head to
        // the tail
        // currently invalid changes are assigned a -infty score
        const double delta =
            _graph_->existsArc(Arc(tail, head)) && !_graph_->existsArc(Arc(head, tail))
                ? _scoreAfterRemovingParent_(head, tail) - _node_scores_[head]
                      + _scoreAfterAddingParent_(tail, head) - _node_scores_[tail]
                : std::numeric_limits< double >::lowest();
        if (delta == std::numeric_limits< double >::lowest()) {
          std::cout << "infAddRev (" << tail << ", " << head << ") = " << ++_nb_inf_ << std::endl;
        }
        _sorted_changes_.insert(change, delta);
      }
    }

    /// adds an ArcTriangleDeletion1 to _sorted_changes_ if possible
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE void
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _addArcTriangleDeletion1ToSortedChanges_(const ArcTriangleDeletion1& change) {
      // only add the arc reversal if this is allowed by the constraints
      if (!_invariable_constraints_->isAlwaysInvalid(change)) {
        const auto node1 = change.node1();
        const auto node2 = change.node2();
        const auto node3 = change.node3();

        // the arc triangle deletion will substitute triangle
        // node1 -> node2 -> node3 + node1 -> node3 into the following v-structure:
        // node2 -> node1 <- node3. The modifications will therefore be:
        // 1/ add node2 and node3 to the set of parents of node1
        // 2/ remove node1 from the set of parents of node2
        // 3/ remove node1 and node2 from the set of parents of node3
        // compute the update in the score due to these 3 operations
        // But currently invalid changes are assigned a -infty score

        const double delta
            = _graph_->existsArc(Arc(node1, node2)) && _graph_->existsArc(Arc(node2, node3))
                   && _graph_->existsArc(Arc(node1, node3))
                   && !_graph_->existsArc(Arc(node2, node1))
                   && !_graph_->existsArc(Arc(node3, node1))
                ? _scoreAfterAddingParents_(node1, node2, node3) - _node_scores_[node1]
                      + _scoreAfterRemovingParent_(node2, node1) - _node_scores_[node2]
                      + _scoreAfterRemovingParents_(node3, node1, node2) - _node_scores_[node3]
                : std::numeric_limits< double >::lowest();
        if (delta == std::numeric_limits< double >::lowest()) {
          std::cout << "infAddTr1 (" << node1 << ", " << node2 << ", " << node3 << ") = "  << ++_nb_inf_ << std::endl;
        }
        _sorted_changes_.insert(change, delta);
      }
    }

    /// adds an ArcTriangleDeletion2 to _sorted_changes_ if possible
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE void
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _addArcTriangleDeletion2ToSortedChanges_(const ArcTriangleDeletion2& change) {
      // only add the arc reversal if this is allowed by the constraints
      if (!_invariable_constraints_->isAlwaysInvalid(change)) {
        const auto node1 = change.node1();
        const auto node2 = change.node2();
        const auto node3 = change.node3();

        // the arc triangle deletion will substitute triangle
        // node1 -> node2 -> node3 + node1 -> node3 into the following v-structure:
        // node1 -> node2 <- node3. The modifications will therefore be:
        // 1/ add node3 to the set of parents of node2
        // 2/ remove node1 and node2 from the set of parents of node3
        // compute the update in the score due to these 2 operations
        // But currently invalid changes are assigned a -infty score

        const double delta
            = _graph_->existsArc(Arc(node1, node2)) && _graph_->existsArc(Arc(node2, node3))
                   && _graph_->existsArc(Arc(node1, node3))
                   && !_graph_->existsArc(Arc(node3, node2))
            ? _scoreAfterAddingParent_(node2, node3) - _node_scores_[node2]
                  + _scoreAfterRemovingParents_(node3, node1, node2)
                  - _node_scores_[node3]
            : std::numeric_limits< double >::lowest();
        if (delta == std::numeric_limits< double >::lowest()) {
          std::cout << "infAddTr2 (" << node1 << ", " << node2 << ", " << node3 << ") = " << ++_nb_inf_ << std::endl;
        }
        _sorted_changes_.insert(change, delta);
      }
    }

    /// updates the score of a given ArcAddition
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE void
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _updateArcAdditionScore_(const NodeId tail, const NodeId head) {
      const auto addition = _sorted_changes_.tryGet(ArcAddition(tail, head));
      if (addition.has_value()) {
        // currently unavailable changes have a -infty priority
        const double delta = !_graph_->existsArc(tail, head)
            ? _scoreAfterAddingParent_(head, tail) - _node_scores_[head]
            : std::numeric_limits< double >::lowest();
        _sorted_changes_.setPriority(addition.value(), delta, true);
      }
    }

    /// updates the score of a given ArcDeletion
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE void
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _updateArcDeletionScore_(const NodeId tail, const NodeId head) {
      const auto deletion = _sorted_changes_.tryGet(ArcDeletion(tail, head));
      if (deletion.has_value()) {
        // currently unavailable changes have a -infty priority
        const double delta = _graph_->existsArc(tail, head)
            ? _scoreAfterRemovingParent_(head, tail) - _node_scores_[head]
            : std::numeric_limits< double >::lowest();
        _sorted_changes_.setPriority(deletion.value(), delta, true);
      }
    }

    /// updates the score of a given ArcReversal
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE void
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            _updateArcReversalScore_(const NodeId tail, const NodeId head) {
      const auto reversal = _sorted_changes_.tryGet(ArcReversal(tail, head));
      if (reversal.has_value()) {
        const double delta =
            _graph_->existsArc(tail, head) && !_graph_->existsArc(head, tail)
            ? _scoreAfterRemovingParent_(head, tail) - _node_scores_[head]
                  + _scoreAfterAddingParent_(tail, head) - _node_scores_[tail]
            : std::numeric_limits< double >::lowest();
        _sorted_changes_.setPriority(reversal.value(), delta, true);
      }
    }

    /** @brief update the score of the triangle deletions which contain a node whose
     * neighborhood has changed */
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        _updateTriangleDeletionsScoresFromNeighborhood_(const NodeId changed_node) {
      const auto& children_changed_node = _graph_->children(changed_node);
      const auto& parents_changed_node  = _graph_->parents(changed_node);
      const auto  score_changed_node    = _node_scores_[changed_node];

      // we consider here triangles node1 -> node2 -> node3 + node1 -> node3
      // Three cases can occur: changed_node is 1/ node1, 2/ node2, 3/ node3
      // case1:
      for (const auto node2: children_changed_node) {
        for (const auto node3: children_changed_node) {
          if (_graph_->parents(node3).contains(node2)) { // changed_node -> node2 -> node3
            double delta3 = std::numeric_limits< double >::lowest();

            // case 1 for ArcTriangleDeletion1
            {
              auto change
                  = _sorted_changes_.tryGet(ArcTriangleDeletion1(changed_node, node2, node3));
              if (change.has_value()) {
                const bool is_valid = !_graph_->existsArc(node2, changed_node)
                                   && !_graph_->existsArc(node3, changed_node);
                // the delta score at node3
                delta3 = is_valid ? _scoreAfterRemovingParents_(node3, changed_node, node2)
                                        - _node_scores_[node3]
                                  : delta3;
                // the following delta score just computes the score of the
                // v-structure that we would get if we applied the arc triangle
                // deletion minus the current score of the 3 nodes involved
                // in the triangle
                const double delta = is_valid
                                       ? _scoreAfterAddingParents_(changed_node, node3, node2)
                                             - score_changed_node
                                             + _scoreAfterRemovingParent_(node2, changed_node)
                                             - _node_scores_[node2] + delta3
                                       : std::numeric_limits< double >::lowest();
                _sorted_changes_.setPriority(change.value(), delta, true);
              }
            }

            // case 1 for ArcTriangleDeletion2
            {
              auto change
                  = _sorted_changes_.tryGet(ArcTriangleDeletion2(changed_node, node2, node3));
              if (change.has_value()) {
                const bool is_valid
                    = !_graph_->existsArc(changed_node, node2) && !_graph_->existsArc(node3, node2);
                delta3             = is_valid && (delta3 == std::numeric_limits< double >::lowest())
                                       ? _scoreAfterRemovingParents_(node3, changed_node, node2)
                                 - _node_scores_[node3]
                                       : delta3;
                const double delta = is_valid ? _scoreAfterAddingParent_(node2, node3)
                                                    - _node_scores_[node2] + delta3
                                              : std::numeric_limits< double >::lowest();
                _sorted_changes_.setPriority(change.value(), delta, true);
              }
            }
          }
        }
      }

      // case2:
      for (const auto node1: parents_changed_node) {
        for (const auto node3: children_changed_node) {
          if (_graph_->parents(node3).contains(node1)) { // node1 -> changed_node -> node3
            double delta3 = std::numeric_limits< double >::lowest();

            // case 2 for ArcTriangleDeletion1
            {
              auto change = _sorted_changes_.tryGet(ArcTriangleDeletion1(node1, changed_node, node3));
              if (change.has_value()) {
                const bool is_valid
                    = !_graph_->existsArc(changed_node, node1) && !_graph_->existsArc(node3, node1);
                // the delta score at node3
                delta3 = is_valid ? _scoreAfterRemovingParents_(node3, node1, changed_node)
                                        - _node_scores_[node3]
                                  : delta3;
                const double delta = is_valid
                                       ? _scoreAfterAddingParents_(node1, changed_node, node3)
                                             - _node_scores_[node1]
                                             + _scoreAfterRemovingParent_(changed_node, node1)
                                             - score_changed_node + delta3
                                       : std::numeric_limits< double >::lowest();
                _sorted_changes_.setPriority(change.value(), delta, true);
              }
            }

            // case 2 for ArcTriangleDeletion2
            {
              auto change
                  = _sorted_changes_.tryGet(ArcTriangleDeletion2(node1, changed_node, node3));
              if (change.has_value()) {
                const bool is_valid = !_graph_->existsArc(node1, changed_node)
                                   && !_graph_->existsArc(node3, changed_node);
                delta3             = is_valid && (delta3 == std::numeric_limits< double >::lowest())
                                       ? _scoreAfterRemovingParents_(node3, node1, changed_node)
                                 - _node_scores_[node3]
                                       : delta3;
                const double delta = is_valid ? _scoreAfterAddingParent_(changed_node, node3)
                                                    - score_changed_node + delta3
                                              : std::numeric_limits< double >::lowest();
                _sorted_changes_.setPriority(change.value(), delta, true);
              }
            }
          }
        }
      }

      // case 3:
      for (const auto node1: parents_changed_node) {
        for (const auto node2: parents_changed_node) {
          if (_graph_->parents(node2).contains(node1)) { // node1 -> node2 -> changed_node
            double delta3 = std::numeric_limits< double >::lowest();

            // case 3 for ArcTriangleDeletion1
            {
              auto change
                  = _sorted_changes_.tryGet(ArcTriangleDeletion1(node1, node2, changed_node));
              if (change.has_value()) {
                const bool is_valid
                    = !_graph_->existsArc(node2, node1) && !_graph_->existsArc(changed_node, node1);
                // the delta score at changed_node
                delta3 = is_valid ? _scoreAfterRemovingParents_(changed_node, node1, node2)
                                        - score_changed_node
                                  : delta3;
                const double delta = is_valid
                                       ? _scoreAfterAddingParents_(node1, changed_node, node2)
                                             - _node_scores_[node1]
                                             + _scoreAfterRemovingParent_(node2, node1)
                                             - _node_scores_[node2] + delta3
                                       : std::numeric_limits< double >::lowest();
                _sorted_changes_.setPriority(change.value(), delta, true);
              }
            }

            // case 3 for ArcTriangleDeletion2
            {
               auto change
                  = _sorted_changes_.tryGet(ArcTriangleDeletion2(node1, node2, changed_node));
               if (change.has_value()) {
                 const bool is_valid = !_graph_->existsArc(node1, node2)
                                    && !_graph_->existsArc(changed_node, node2);
                 delta3 = is_valid && (delta3 == std::numeric_limits< double >::lowest())
                            ? _scoreAfterRemovingParents_(changed_node, node1, node2)
                                  - score_changed_node
                            : delta3;
                 const double delta = is_valid ? _scoreAfterAddingParent_(node2, changed_node)
                                                     - _node_scores_[node2] + delta3
                                               : std::numeric_limits< double >::lowest();
                 _sorted_changes_.setPriority(change.value(), delta, true);
               }
            }
          }
        }
      }
    }

    /// add all the possible ArcAdditions to initialize _sorted_changes_
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        _initSortedChangesWithArcAdditions_(const DiGraph& graph) {
      // for each pair of nodes (node1,node2) that are not adjacent, try an arc addition
      for (auto iter1 = graph.begin(); iter1 != graph.end(); ++iter1) {
        const NodeId node1 = *iter1;

        // here, we enforce that node2 > node to avoid having to check the adjacency twice:
        // one for pair (node1,node2) and the second for pair (node2,node1)
        auto iter2 = iter1;
        for (++iter2; iter2 != graph.end(); ++iter2) {
          const NodeId node2 = *iter2;
          _addArcAdditionToSortedChanges_(ArcAddition(node1, node2));
          _addArcAdditionToSortedChanges_(ArcAddition(node2, node1));
        }
      }
    }

    /// add all the possible ArcDeletions to initialize _sorted_changes_
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        _initSortedChangesWithArcDeletions_(const DiGraph& graph) {
      // for each arc in graph, try to add an arc deletion
      for (const auto& arc: graph.arcs()) {
        _addArcDeletionToSortedChanges_(ArcDeletion(arc.tail(), arc.head()));
      }
    }

    /// add all the possible ArcReversals to initialize _sorted_changes_
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        _initSortedChangesWithArcReversals_(const DiGraph& graph) {
      // for each arc in graph, try to add an arc reversal
      for (const auto& arc: graph.arcs()) {
        _addArcReversalToSortedChanges_(ArcReversal(arc.tail(), arc.head()));
      }
    }

    /// add all the possible ArcTriangleDeletions to initialize _sorted_changes_
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        _initSortedChangesWithArcTriangleDeletions_(const DiGraph& graph) {
      // for each node "node1" having two children node2 and node3 such that node3 is
      // also a child of node2, we have a triangle
      for (const auto node1: graph) {
        const auto& children1 = graph.children(node1);
        for (const auto node2: children1) {
          const auto& children2 = graph.children(node2);
          for (const auto node3: children2) {
            if (children1.exists(node3)) {
              // here, we have a triangle node1 -> node2 -> node3 + node1 -> node3.
              // add the two possible arc triangle deletions (the v-structure cannot
              // be on node3 since this would correspond to a deletion of arc
              // node1 -> node2)
              _addArcTriangleDeletion1ToSortedChanges_(ArcTriangleDeletion1(node1, node2, node3));
              _addArcTriangleDeletion2ToSortedChanges_(ArcTriangleDeletion2(node1, node2, node3));
            }
          }
        }
      }
    }

    /// returns the best graph change that can be applied
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE const GraphChange&
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                      VARIABLE_CONSTRAINT_TYPE >::bestChange() {
      for (auto iter = _sorted_changes_.begin(); iter != _sorted_changes_.end(); ++iter) {
        if (_variable_constraints_->checkModification(*iter)) { return *iter; }
      }

      GUM_ERROR(NotFound, "there exists no graph change applicable")
    }

    /// return the score of the best graph change
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE double GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE,
                                                VARIABLE_CONSTRAINT_TYPE >::bestDeltaScore() {
      for (auto iter = _sorted_changes_.begin(); iter != _sorted_changes_.end(); ++iter) {
        if (_variable_constraints_->checkModification(*iter)) { return iter.priority(); }
      }

      GUM_ERROR(NotFound, "there exists no graph change applicable")
    }

    /// return the delta score of a change in the ith queue
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE double
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            deltaScore(const GraphChange& change, const bool internal_change) const {
      try {
        return _sorted_changes_.priority(change, internal_change);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "Change " << change.toString() << " will never be applicable")
      }
    }

    /// returns the current score of a node
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE double
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::score(
            const NodeId node) const {
      return _node_scores_[node];
    }

    /// indicates whether a given change is valid or not
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    INLINE bool
        GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
            isChangeValid(const GraphChange& change) const {
      return !_invariable_constraints_->isAlwaysInvalid(change)
          && _variable_constraints_->checkModification(change);
    }

    /// indicate to the selector that an ArcAddition has been applied
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        _applyArcAddition_(const ArcAddition& change) {
      // get the delta score of the change and update the score of the head
      const auto tail        = change.node1();
      const auto head        = change.node2();
      const auto delta_score = _sorted_changes_.priority(change);
      _node_scores_[head] += delta_score;

      // update the graph
      _graph_->addArc(tail, head);
      _node_parents_[head].push_back(tail);
      const auto& children_head = _graph_->children(head);
      const auto& parents_head  = _graph_->parents(head);

      // indicate to the constraints that we added a new arc
      _invariable_constraints_->modifyGraph(change);
      _variable_constraints_->modifyGraph(change);

      // now, we have to update the _sorted_changes_. Here, several modifications
      // are needed:
      // 1/ Remove the arc addition tail -> head we just applied
      // 2/ Update The scores of the changes impacted by head's additional
      //    ingoing arc:
      //    2.a/ all the additions of arcs whose heads are equal to "head"
      //         should be updated
      //    2.b/ The scores of all the deletions of arcs whose heads are equal
      //         to "head" should be updated
      //    2.c/ The scores of all the arc reversals involving "head" should
      //         be updated
      //    2.d/ The scores of all the arc triangle deletions in which one of the
      //         nodes is head should be updated
      // 3/ Add the changes induced by the application of the arc addition:
      //    3.a/ Add the reverse change: Arc tail->head's deletion
      //    3.b/ Arc tail->head's reversal should be added
      //    3.c/ The new arc may have created some new triangles, hence we should
      //         also add their possible removals to the _sorted_changes_

      _sorted_changes_.erase(change, true);   // case 1

      // case 2.a:
      if (_use_arc_additions_) {
        for (const auto node: *_graph_) {
          // here, no need to check whether node is different from tail because,
          // in case 1, we just removed ArcAddition(tail, head)
          _updateArcAdditionScore_(node, head);
        }
      }

      // cases 2.b and 3.a
      if (_use_arc_deletions_) {
        // case 2.b:
        for (const auto node: parents_head) {
          // here, no need to check whether node is different from tail because,
          // as we just added Arc tail -> head, this arc did not belonged yet to
          // _sorted_changes_, hence the latter cannot contain yet
          // ArcDeletion tail -> head
          _updateArcDeletionScore_(node, head);
        }

        // case 3.a:
        const ArcDeletion deletion(tail, head);
        if (!_invariable_constraints_->isAlwaysInvalid(deletion)) {
          _sorted_changes_.insert(deletion, -delta_score);
        }
      }

      // cases 2.c and 3.b
      if (_use_arc_reversals_) {
        // case 2.c: hre, we consider the reversals of arcs node -> head
        for (const auto node: parents_head) {
          // here, no need to check whether node is different from tail because
          // we just added Arc tail -> head, hence ArcReversal(tail, head) cannot
          // belong yet to _sorted_changes_
          _updateArcReversalScore_(node, head);
        }

        // case 2.c: we consider the reversals of arcs head -> node
        for (const auto node: children_head) {
          _updateArcReversalScore_(head, node);
        }

        // case 3.b:
        // we must add an arc reversal tail->head
        _addArcReversalToSortedChanges_(ArcReversal(tail, head));
      }

      // cases 2.d and 3.c:
      if (_use_arc_triangle_deletions_) {
        // case 2.d:
        _updateTriangleDeletionsScoresFromNeighborhood_(head);

        // case 3.c:
        // there are 3 types of triangles: a/ tail->node->head, b/ tail->head->node
        // and c/ node->tail->head. We should examine these three cases

        // cases a/ and c/
        const auto& children_tail = _graph_->children(tail);
        const auto& parents_tail  = _graph_->parents(tail);
        for (const auto node: parents_head) {
          if (children_tail.contains(node)) {         // case a
            _addArcTriangleDeletion1ToSortedChanges_(ArcTriangleDeletion1(tail, node, head));
            _addArcTriangleDeletion2ToSortedChanges_(ArcTriangleDeletion2(tail, node, head));
          } else if (parents_tail.contains(node)) {   // case c
            _addArcTriangleDeletion1ToSortedChanges_(ArcTriangleDeletion1(node, tail, head));
            _addArcTriangleDeletion2ToSortedChanges_(ArcTriangleDeletion2(node, tail, head));
          }
        }

        // case b/
        for (const auto node: children_tail) {
          if (children_head.exists(node)) {
            _addArcTriangleDeletion1ToSortedChanges_(ArcTriangleDeletion1(tail, head, node));
            _addArcTriangleDeletion2ToSortedChanges_(ArcTriangleDeletion2(tail, head, node));
          }
        }
      }
    }

    /// indicate to the selector that an ArcDeletion has been applied
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        _applyArcDeletion_(const ArcDeletion& change) {
      // get the delta score of the change and update the score of the head
      const auto tail        = change.node1();
      const auto head        = change.node2();
      const auto delta_score = _sorted_changes_.priority(change);
      _node_scores_[head] += delta_score;

      // update the graph and the set of parents of the head
      _graph_->eraseArc(Arc(tail, head));
      auto& parents = _node_parents_[head];
      for (auto& par: parents) {
        if (par == tail) {
          par = *(parents.rbegin());
          parents.pop_back();
          break;
        }
      }
      const auto& children_head = _graph_->children(head);
      const auto& parents_head  = _graph_->parents(head);

      // indicate to the constraints that we removed the arc
      _invariable_constraints_->modifyGraph(change);
      _variable_constraints_->modifyGraph(change);

      // now, we have to update the _sorted_changes_. Here, several modifications
      // are needed:
      //  1/ if we removed arc tail -> head, this deletion should be removed from
      //     _sorted_changes_
      //  2/ since arc tail -> head existed, it could also have been reversed.
      //     Hence, we should remove this reversal from _sorted_changes_
      //  3/ the addition of arc tail -> head should be added to _sorted_changes_
      //  4/ if arc head -> tail exists, its reversal is now possible, so add it to
      //     _sorted_changes_
      //  5/ The scores of all the additions of arcs whose heads are equal to
      //     "head" should be updated
      //  6/ The scores of all the deletions of arcs whose heads are equal to
      //     "head" should be updated
      //  7/ The scores of all the arc reversals involving "head" should be updated
      //  8/ The removed arc may have been involved into some triangles, hence we
      //     should also remove them from _sorted_changes_
      //  9/ The scores of the triangle deletions which include head as one of their
      //     nodes should be updated
      // 10/ Some arc triangles deletions which, when applied, would have created
      //     Arc tail -> head could not be inserted yet into _sorted_changes_
      //     because this arc already existed. Now, they can be created.
      _sorted_changes_.erase(change);   // case 1

      // cases 3 and 5
      if (_use_arc_additions_) {
        // case 5
        for (const auto node: *_graph_) {
          // here, no need to check whether node is different from tail because
          // we just added the arc tail -> head to the graph, hence
          // Addition(tail, head) does not belong yet to _sorted_changes_
          _updateArcAdditionScore_(node, head);
        }

        // case 3
        const ArcAddition addition(tail, head);
        if (!_invariable_constraints_->isAlwaysInvalid(addition)) {
          _sorted_changes_.insert(addition, -delta_score);
        }
      }

      // case 6
      for (const auto node: parents_head) {
        // here, no need to check whether node is different from tail because
        // we just erased in case 1 ArcDeletion(tail, head)
        _updateArcDeletionScore_(node, head);
      }

      // cases 2, 4 and 7
      if (_use_arc_reversals_) {
        _sorted_changes_.erase(ArcReversal(tail, head));   // case 2

        // case 7:
        // case 7.1: we consider the reversals of arcs node -> head
        for (const auto node: parents_head) {
          // here, no need to check whether node is different from tail because
          // we just erased in case 2 ArcReversal(tail, head)
          _updateArcReversalScore_(node, head);
        }

        // case 7.2: we consider the reversals of arcs head -> node
        for (const auto node: children_head) {
          // Here, no need to check whether node is different from tail.
          // Actually, if node = tail, we are considering in this for loop change
          // ArcReversal(head,tail). This means that arc head->tail already
          // existed in the graph and that we consider the possibility of
          // substituting it by arc tail -> head. But since, in this method, we
          // apply ArcDeletion(tail, head), this means that this arc already
          // existed. Hence ArcReversal(head,tail) could not have belonged
          // to _sorted_changes_
          _updateArcReversalScore_(head, node);
        }

        // case 4: if arc head -> tail exists, add its reversal to _sorted_changes_
        _addArcReversalToSortedChanges_(ArcReversal(head, tail));
      }

      // cases 8 and 9:
      if (_use_arc_triangle_deletions_) {
        // case 8:
        // there are 3 types of triangles: a/ tail->node->head, b/ tail->head->node
        // and c/ node->tail->head. We should examine these three cases

        // cases a/ and c/
        const auto& children_tail = _graph_->children(tail);
        const auto& parents_tail  = _graph_->parents(tail);
        for (const auto node: parents_head) {
          if (children_tail.contains(node)) {   // case a
            _sorted_changes_.erase(ArcTriangleDeletion1(tail, node, head));
            _sorted_changes_.erase(ArcTriangleDeletion2(tail, node, head));
          }
          if (parents_tail.contains(node)) {   // case c
            _sorted_changes_.erase(ArcTriangleDeletion1(node, tail, head));
            _sorted_changes_.erase(ArcTriangleDeletion2(node, tail, head));
          }
        }

        // case b/
        for (const auto node: children_tail) {
          if (children_head.contains(node)) {
            _sorted_changes_.erase(ArcTriangleDeletion1(tail, head, node));
            _sorted_changes_.erase(ArcTriangleDeletion2(tail, head, node));
          }
        }

        // case 9:
        _updateTriangleDeletionsScoresFromNeighborhood_(head);

        // case 10:
        // there are 3 cases to consider: a/ ArcTriangleDeletion1(head, tail, x),
        // b/ ArcTriangleDeletion(head, x, tail) and
        // c/ ArcTriangleDeletion2(x, head, tail)
        // case a:
        for (const auto node: children_head) {
          if (children_tail.contains(node)) {
            _addArcTriangleDeletion1ToSortedChanges_(ArcTriangleDeletion1(head, tail, node));
          }
        }

        for (const auto node: parents_tail) {
          // case b:
          if (children_head.contains(node)) {
            _addArcTriangleDeletion1ToSortedChanges_(ArcTriangleDeletion1(head, node, tail));
          }

          // case c:
          if (parents_head.contains(node)) {
            _addArcTriangleDeletion2ToSortedChanges_(ArcTriangleDeletion2(node, head, tail));
          }
        }
      }
    }

    /// indicate to the selector that an ArcReversal has been applied
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        _applyArcReversal_(const ArcReversal& change) {
      // update the graph
      const auto tail = change.node1();
      const auto head = change.node2();
      _graph_->eraseArc(Arc(tail, head));
      _graph_->addArc(head, tail);
      const auto& children_head = _graph_->children(head);
      const auto& parents_head  = _graph_->parents(head);
      const auto& children_tail = _graph_->children(tail);
      const auto& parents_tail  = _graph_->parents(tail);

      auto& node_parents_tail = _node_parents_[tail];
      node_parents_tail.push_back(head);
      auto& node_parents_head = _node_parents_[head];
      for (auto& par: node_parents_head) {
        if (par == tail) {
          par = *(node_parents_head.rbegin());
          node_parents_head.pop_back();
          break;
        }
      }

      // update the scores of the head and of the tail
      const auto overall_delta_score = _sorted_changes_.priority(change);
      const auto delta_score_tail = _score_->score(tail, node_parents_tail) - _node_scores_[tail];

      _node_scores_[head] += overall_delta_score - delta_score_tail;
      _node_scores_[tail] += delta_score_tail;
      const auto tail_score = _node_scores_[tail];

      // indicate to the constraints that we reversed an arc
      _invariable_constraints_->modifyGraph(change);
      _variable_constraints_->modifyGraph(change);

      // now, we have to update the _sorted_changes_. Here, several modifications
      // are needed:
      //  1/ if we reversed Arc tail -> head, then this reversal should be removed
      //     from _sorted_changes_
      //  2/ since arc tail -> head existed, its deletion should be also removed
      //     from _sorted_changes_.
      //  3/ Arc head->tail's deletion should be added
      //  4/ Arc head->tail's reversal should be added
      //  5/ The scores of all the additions of arcs whose heads are equal to
      //     "head" or "tail" should be updated
      //  6/ The scores of all the deletions of arcs whose heads are equal to
      //     "head" or "tail" should be updated
      //  7/ The scores of all the arc reversals involving "head" or "tail"
      //     should be updated
      //  8/ Arc tail -> head has been removed. It may have been involved into
      //     some triangles, hence we should also remove them from _sorted_changes_
      //  9/ The scores of the triangle deletions that involve either head or tail
      //     as one of their nodes should be updated.
      // 10/ Arc head -> tail has been added. It may have created some triangles,
      //     hence we should also add their possible removals to the
      //     _sorted_changes_
      _sorted_changes_.erase(change);   // case 1

      // case 5
      if (_use_arc_additions_) {
        for (const auto node: *_graph_) {
          // here, no need to check whether node is equal to tail. Actually, since
          // we apply ArcReversal(tail, head), this means that arc tail -> head
          // belonged to the graph, hence ArcAddition(tail, head) cannot belong
          // yet to _sorted_changes_
          _updateArcAdditionScore_(node, head);

          try {
            // Here, if node = head, then we consider change ArcAddition(head, tail).
            // Since, in this method, we apply ArcReversal(tail, head), which
            // already creates arc head->tail, ArcAddition(head, tail) has now
            // become impossible and should be removed from _sorted_changes_
            const auto& addition = _sorted_changes_[ArcAddition(node, tail)];
            if (node != head) {
              const double delta = _scoreAfterAddingParent_(tail, node) - tail_score;
              _sorted_changes_.setPriority(addition, delta, true);
            } else {
              _sorted_changes_.erase(addition, true);
            }
          } catch (NotFound&) {}
        }
      }

      // cases 2, 3 and 6:
      if (_use_arc_deletions_) {
        _sorted_changes_.erase(ArcDeletion(tail, head));   // case 2

        // case 6:
        for (const auto node: parents_head) {
          // here, no need to check whether node = tail because we just removed in
          // case 2 change ArcDeletion(tail, head)
          _updateArcDeletionScore_(node, head);
        }

        for (const auto node: parents_tail) {
          // Here, no need to check whether node = head. Actuallu, if node = head,
          // we are considering ArcDeletion(head,tail). But, in this method, we
          // apply ArcReversal(tail, head), which creates arc head->tail. Hence,
          // since this arc did not exist before the application of change,
          // ArcDeletion(head,tail) cannot belong yet to _sorted_changes_
          _updateArcDeletionScore_(node, tail);
        }

        // case 3:
        const ArcDeletion deletion(head, tail);
        if (!_invariable_constraints_->isAlwaysInvalid(deletion)) {
          _sorted_changes_.insert(deletion, -delta_score_tail);
        }
      }

      // case 7:
      // case 7.1: we consider the reversals of arcs node -> head
      for (const auto node: parents_head) {
        // here, no need to check whether node = tail because we removed
        // ArcReversal(tail, head) in case 1
        _updateArcReversalScore_(node, head);
      }

      // case 7.2: we consider the reversals of arcs head -> node
      for (const auto node: children_head) {
        // here, no need to check whether node = tail. Actually, for
        // ArcReversal(head, tail) to belong to _sorted_changes_, _graph_ should
        // have contained arc head -> tail before the execution of the current
        // method. But this is impossible because change = ArcReversal(tail, head),
        // which means that arc head -> tail did not exist.
        _updateArcReversalScore_(head, node);
      }

      // case 7.3: we consider the reversals of arcs node -> tail
      for (const auto node: parents_tail) {
        // here, no need to check whether node = head. Actually, for
        // ArcReversal(head,tail) to belong to _sorted_changes_, _graph_ should
        // have contained arc head -> tail before the execution of the current
        // method. But this is impossible because change = ArcReversal(tail, head),
        // which means that arc head -> tail did not exist.
        _updateArcReversalScore_(node, tail);
      }

      // case 7.4: we consider the reversals of arcs tail -> node
      for (const auto node: children_tail) {
        // here, no need to check whether node = tail because we removed
        // ArcReversal(tail, head) in case 1
        _updateArcReversalScore_(tail, node);
      }

      // case 4
      const ArcReversal reversal(head, tail);
      if (!_invariable_constraints_->isAlwaysInvalid(reversal)) {
        _sorted_changes_.insert(reversal, -overall_delta_score);
      }

      if (_use_arc_triangle_deletions_) {
        // case 8:
        // there are 3 types of triangles: a/ tail->node->head, b/ tail->head->node
        // and c/ node->tail->head. We should examine these three cases
        for (const auto node: parents_head) {   // cases a/ and c/
          if (children_tail.contains(node)) {   // case a
            _sorted_changes_.erase(ArcTriangleDeletion1(tail, node, head));
            _sorted_changes_.erase(ArcTriangleDeletion2(tail, node, head));
          }
          if (parents_tail.exists(node)) {   // case c
            _sorted_changes_.erase(ArcTriangleDeletion1(node, tail, head));
            _sorted_changes_.erase(ArcTriangleDeletion2(node, tail, head));
          }
        }

        for (const auto node: children_tail) {   // case b/
          if (children_head.contains(node)) {
            _sorted_changes_.erase(ArcTriangleDeletion1(tail, head, node));
            _sorted_changes_.erase(ArcTriangleDeletion2(tail, head, node));
          }
        }

        // case 9:
        _updateTriangleDeletionsScoresFromNeighborhood_(head);
        _updateTriangleDeletionsScoresFromNeighborhood_(tail);

        // case 10:
        // there are 3 types of triangles: a/ head->node->tail, b/ head->tail->node
        // and c/ node->head->tail. We should examine these three cases
        for (const auto node: parents_tail) {            // cases a/ and c/
          if (_graph_->parents(node).contains(head)) {   // case a
            _addArcTriangleDeletion1ToSortedChanges_(ArcTriangleDeletion1(head, node, tail));
            _addArcTriangleDeletion2ToSortedChanges_(ArcTriangleDeletion2(head, node, tail));
          }
          if (parents_head.exists(node)) {   // case c
            _addArcTriangleDeletion1ToSortedChanges_(ArcTriangleDeletion1(node, head, tail));
            _addArcTriangleDeletion2ToSortedChanges_(ArcTriangleDeletion2(node, head, tail));
          }
        }

        // case b/
        for (const auto node: children_head) {
          if (_graph_->parents(node).contains(tail)) {
            _addArcTriangleDeletion1ToSortedChanges_(ArcTriangleDeletion1(head, tail, node));
            _addArcTriangleDeletion2ToSortedChanges_(ArcTriangleDeletion2(head, tail, node));
          }
        }
      }
    }

    /// indicate to the selector that an ArcTriangleDeletion1 has been applied
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        _applyArcTriangleDeletion1_(const ArcTriangleDeletion1& change) {
      const auto node1 = change.node1();
      const auto node2 = change.node2();
      const auto node3 = change.node3();

      // remove the arc triangle deletion
      _sorted_changes_.erase(change);

      // apply the deletion:
      // 1/ remove arc node2 -> node3
      _applyArcDeletion_(ArcDeletion(node2, node3));

      // 2/ try to reverse arc node1 -> node2
      const ArcReversal reversal12(node1, node2);
      bool              reversal12_applied = false;
      if (_variable_constraints_->checkModification(reversal12)) {
        _applyArcReversal_(reversal12);
        reversal12_applied = true;
      } else {
        _applyArcDeletion_(ArcDeletion(node1, node2));
      }

      // 3/ try to reverse node1 -> node3
      const ArcReversal reversal13(node1, node3);
      bool              reversal13_applied = false;
      if (_variable_constraints_->checkModification(reversal13)) {
        _applyArcReversal_(reversal13);
        reversal13_applied = true;
      } else {
        _applyArcDeletion_(ArcDeletion(node1, node3));
      }

      // 4/ finalize the reversal of arcs node1 -> node2 and node1 -> node3
      if (!reversal12_applied) { _applyArcAddition_(ArcAddition(node2, node1)); }
      if (!reversal13_applied) { _applyArcAddition_(ArcAddition(node3, node1)); }
    }

    /// indicate to the selector that an ArcTriangleDeletion2 has been applied
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        _applyArcTriangleDeletion2_(const ArcTriangleDeletion2& change) {
      const auto node1 = change.node1();
      const auto node2 = change.node2();
      const auto node3 = change.node3();

      // remove the arc triangle deletion
      _sorted_changes_.erase(change);

      // apply the deletion:
      // 1/ remove arc node1 -> node3
      _applyArcDeletion_(ArcDeletion(node1, node3));

      // 2/ try to reverse arc node2 -> node4
      _applyArcReversal_(ArcReversal(node2, node3));
    }

    /// indicate to the selector that a change has been applied
    template < typename INVARIABLE_CONSTRAINT_TYPE, typename VARIABLE_CONSTRAINT_TYPE >
    void GraphChangesSelector4DiGraph< INVARIABLE_CONSTRAINT_TYPE, VARIABLE_CONSTRAINT_TYPE >::
        applyChange(const GraphChange& change) {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION :
          _applyArcAddition_(static_cast< const ArcAddition& >(change));
          break;

        case GraphChangeType::ARC_DELETION :
          _applyArcDeletion_(static_cast< const ArcDeletion& >(change));
          break;

        case GraphChangeType::ARC_REVERSAL :
          _applyArcReversal_(static_cast< const ArcReversal& >(change));
          break;

        case GraphChangeType::ARC_TRIANGLE_DELETION1 :
          _applyArcTriangleDeletion1_(static_cast< const ArcTriangleDeletion1& >(change));
          break;

        case GraphChangeType::ARC_TRIANGLE_DELETION2 :
          _applyArcTriangleDeletion2_(static_cast< const ArcTriangleDeletion2& >(change));
          break;

        default :
          GUM_ERROR(NotImplementedYet,
                    "Method applyChange of GraphChangesSelector4DiGraph "
                        << "does not handle yet graph change of type " << change.typeAsString());
      }
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
