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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <limits>

namespace gum {

  namespace learning {

    /// default constructor
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    INLINE GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::
       GraphChangesSelector4DiGraph(Score&                   score,
                                    STRUCTURAL_CONSTRAINT&   constraint,
                                    GRAPH_CHANGES_GENERATOR& changes_generator) :
        _score_(score.clone()),
        _constraint_(&constraint), _changes_generator_(&changes_generator) {
      _parents_.resize(32);
      GUM_CONSTRUCTOR(GraphChangesSelector4DiGraph);
    }

    /// copy constructor
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::
       GraphChangesSelector4DiGraph(
          const GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >&
             from) :
        _score_(from._score_ != nullptr ? from._score_->clone() : nullptr),
        _constraint_(from._constraint_), _changes_generator_(from._changes_generator_),
        _changes_(from._changes_), _change_scores_(from._change_scores_),
        _change_queue_per_node_(from._change_queue_per_node_), _node_queue_(from._node_queue_),
        _illegal_changes_(from._illegal_changes_),
        _node_current_scores_(from._node_current_scores_), _parents_(from._parents_),
        _queues_valid_(from._queues_valid_), _queues_to_update_(from._queues_to_update_) {
      // for debugging
      GUM_CONS_CPY(GraphChangesSelector4DiGraph);
    }


    /// move constructor
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::
       GraphChangesSelector4DiGraph(
          GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >&& from) :
        _score_(from._score_),
        _constraint_(std::move(from._constraint_)),
        _changes_generator_(std::move(from._changes_generator_)),
        _changes_(std::move(from._changes_)), _change_scores_(std::move(from._change_scores_)),
        _change_queue_per_node_(std::move(from._change_queue_per_node_)),
        _node_queue_(std::move(from._node_queue_)),
        _illegal_changes_(std::move(from._illegal_changes_)),
        _node_current_scores_(std::move(from._node_current_scores_)),
        _parents_(std::move(from._parents_)), _queues_valid_(std::move(from._queues_valid_)),
        _queues_to_update_(std::move(from._queues_to_update_)) {
      from._score_ = nullptr;
      // for debugging
      GUM_CONS_MOV(GraphChangesSelector4DiGraph);
    }


    /// destructor
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    INLINE
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR >::~GraphChangesSelector4DiGraph() {
      if (_score_ != nullptr) delete _score_;
      GUM_DESTRUCTOR(GraphChangesSelector4DiGraph);
    }


    /// copy operator
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >&
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::operator=(
          const GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >&
             from) {
      if (this != &from) {
        // remove the old score
        if (_score_ != nullptr) {
          delete _score_;
          _score_ = nullptr;
        }

        if (from._score_ != nullptr) _score_ = from._score_->clone();
        _constraint_            = from._constraint_;
        _changes_generator_     = from._changes_generator_;
        _changes_               = from._changes_;
        _change_scores_         = from._change_scores_;
        _change_queue_per_node_ = from._change_queue_per_node_;
        _node_queue_            = from._node_queue_;
        _illegal_changes_       = from._illegal_changes_;
        _node_current_scores_   = from._node_current_scores_;
        _parents_               = from._parents_;
        _queues_valid_          = from._queues_valid_;
        _queues_to_update_      = from._queues_to_update_;
      }

      return *this;
    }

    /// move operator
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >&
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::operator=(
          GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >&& from) {
      if (this != &from) {
        _score_      = from._score_;
        from._score_ = nullptr;

        _constraint_            = std::move(from._constraint_);
        _changes_generator_     = std::move(from._changes_generator_);
        _changes_               = std::move(from._changes_);
        _change_scores_         = std::move(from._change_scores_);
        _change_queue_per_node_ = std::move(from._change_queue_per_node_);
        _node_queue_            = std::move(from._node_queue_);
        _illegal_changes_       = std::move(from._illegal_changes_);
        _node_current_scores_   = std::move(from._node_current_scores_);
        _parents_               = std::move(from._parents_);
        _queues_valid_          = std::move(from._queues_valid_);
        _queues_to_update_      = std::move(from._queues_to_update_);
      }

      return *this;
    }


    /// indicates whether a given change is valid or not
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    INLINE bool GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::
       isChangeValid(const GraphChange& change) const {
      return _constraint_->checkModification(change);
    }


    /// indicates whether a given change is valid or not
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    INLINE bool GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::
       _isChangeValid_(const std::size_t index) const {
      return isChangeValid(_changes_[index]);
    }


    /// sets the graph from which scores are computed
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::setGraph(
       DiGraph& graph) {
      // fill the DAG with all the missing nodes
      const DatabaseTable& database       = _score_->database();
      const auto&          nodeId2Columns = _score_->nodeId2Columns();

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


      // remove the node that do belong neither to the database
      // nor to nodeId2Columns
      if (nodeId2Columns.empty()) {
        const NodeId nb_nodes = NodeId(database.nbVariables());
        for (auto node: graph) {
          if (node >= nb_nodes) { graph.eraseNode(node); }
        }
      } else {
        for (auto node: graph) {
          if (!nodeId2Columns.existsFirst(node)) { graph.eraseNode(node); }
        }
      }


      //  _constraint_ is the constraint used by the selector to restrict the set
      // of applicable changes. However, the generator may have a different set
      // of constraints (e.g., a constraintSliceOrder needs be tested only by the
      // generator because the changes returned by the generator will always
      // statisfy this constraint, hence the selector needs not test this
      // constraint). Therefore, if the selector and generator have different
      // constraints, both should use method setGraph() to initialize
      // themselves.
      _constraint_->setGraph(graph);
      if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(&(_changes_generator_->constraint()))
          != _constraint_) {
        _changes_generator_->constraint().setGraph(graph);
      }

      _changes_generator_->setGraph(graph);


      // save the set of parents of each node (this will speed-up the
      // computations of the scores)
      const std::size_t nb_nodes = graph.size();
      {
        const std::vector< NodeId > empty_pars;
        _parents_.clear();
        _parents_.resize(nb_nodes);
        for (const auto node: graph) {
          auto&          node_parents = _parents_.insert(node, empty_pars).second;
          const NodeSet& dag_parents  = graph.parents(node);
          if (!dag_parents.empty()) {
            node_parents.resize(dag_parents.size());
            std::size_t j = std::size_t(0);
            for (const auto par: dag_parents) {
              node_parents[j] = par;
              ++j;
            }
          }
        }
      }

      // assign a score to each node given its parents in the current graph
      _node_current_scores_.clear();
      _node_current_scores_.resize(nb_nodes);
      for (const auto node: graph) {
        _node_current_scores_.insert(node, _score_->score(node, _parents_[node]));
      }

      // compute all the possible changes
      _changes_.clear();
      _changes_.resize(nb_nodes);
      for (const auto& change: *_changes_generator_) {
        _changes_ << change;
      }
      _changes_generator_->notifyGetCompleted();

      // determine the changes that are illegal and prepare the computation of
      // the scores of all the legal changes
      _illegal_changes_.clear();

      // set the  _change_scores_ and  _change_queue_per_node_ for legal changes
      _change_scores_.clear();
      _change_scores_.resize(_changes_.size(),
                             std::pair< double, double >(std::numeric_limits< double >::min(),
                                                         std::numeric_limits< double >::min()));
      _change_queue_per_node_.clear();
      _change_queue_per_node_.resize(nb_nodes);
      {
        const PriorityQueue< std::size_t, double, std::greater< double > > empty_prio;
        for (const auto node: graph) {
          _change_queue_per_node_.insert(node, empty_prio);
        }
      }

      for (std::size_t i = std::size_t(0); i < _changes_.size(); ++i) {
        if (!_isChangeValid_(i)) {
          _illegal_changes_.insert(i);
        } else {
          const GraphChange& change = _changes_[i];

          switch (change.type()) {
            case GraphChangeType::ARC_ADDITION: {
              auto& parents = _parents_[change.node2()];
              parents.push_back(change.node1());
              const double delta
                 = _score_->score(change.node2(), parents) - _node_current_scores_[change.node2()];
              parents.pop_back();

              _change_scores_[i].second = delta;
              _change_queue_per_node_[change.node2()].insert(i, delta);
            } break;

            case GraphChangeType::ARC_DELETION: {
              auto& parents = _parents_[change.node2()];
              for (auto& par: parents) {
                if (par == change.node1()) {
                  par = *(parents.rbegin());
                  parents.pop_back();
                  break;
                }
              }
              const double delta
                 = _score_->score(change.node2(), parents) - _node_current_scores_[change.node2()];
              parents.push_back(change.node1());

              _change_scores_[i].second = delta;
              _change_queue_per_node_[change.node2()].insert(i, delta);
            } break;

            case GraphChangeType::ARC_REVERSAL: {
              // remove arc ( node1 -> node2 )
              auto& parents2 = _parents_[change.node2()];
              for (auto& par: parents2) {
                if (par == change.node1()) {
                  par = *(parents2.rbegin());
                  parents2.pop_back();
                  break;
                }
              }

              const double delta2
                 = _score_->score(change.node2(), parents2) - _node_current_scores_[change.node2()];
              parents2.push_back(change.node1());

              // add arc ( node2 -> node1 )
              auto& parents1 = _parents_[change.node1()];
              parents1.push_back(change.node2());
              const double delta1
                 = _score_->score(change.node1(), parents1) - _node_current_scores_[change.node1()];
              parents1.pop_back();

              _change_scores_[i].first  = delta1;
              _change_scores_[i].second = delta2;

              const double delta = delta1 + delta2;
              _change_queue_per_node_[change.node1()].insert(i, delta);
              _change_queue_per_node_[change.node2()].insert(i, delta);

            } break;

            default: {
              GUM_ERROR(NotImplementedYet,
                        "Method setGraph of GraphChangesSelector4DiGraph "
                           << "does not handle yet graph change of type " << change.type());
            }
          }
        }
      }

      // update the global queue
      _node_queue_.clear();
      for (const auto node: graph) {
        _node_queue_.insert(node,
                            _change_queue_per_node_[node].empty()
                               ? std::numeric_limits< double >::min()
                               : _change_queue_per_node_[node].topPriority());
      }
      _queues_valid_ = true;
      _queues_to_update_.clear();
    }


    /// put a change into the illegal set
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::
       _invalidateChange_(const std::size_t change_index) {
      const GraphChange& change = _changes_[change_index];
      if (change.type() == GraphChangeType::ARC_REVERSAL) {
        // remove the tail change from its priority queue
        PriorityQueue< std::size_t, double, std::greater< double > >& queue1
           = _change_queue_per_node_[change.node1()];
        queue1.erase(change_index);

        // recompute the top priority for the changes of the head
        const double new_priority
           = queue1.empty() ? std::numeric_limits< double >::min() : queue1.topPriority();
        _node_queue_.setPriority(change.node1(), new_priority);
      }

      // remove the head change from its priority queue
      PriorityQueue< std::size_t, double, std::greater< double > >& queue2
         = _change_queue_per_node_[change.node2()];
      queue2.erase(change_index);

      // recompute the top priority for the changes of the head
      const double new_priority
         = queue2.empty() ? std::numeric_limits< double >::min() : queue2.topPriority();
      _node_queue_.setPriority(change.node2(), new_priority);

      // put the change into the illegal set
      _illegal_changes_.insert(change_index);
    }


    /// indicates whether the selector still contain graph changes
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    bool GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::empty() {
      // put into the illegal change set all the top elements of the different
      // queues that are not valid anymore
      if (!_queues_valid_) {
        for (auto& queue_pair: _change_queue_per_node_) {
          auto& queue = queue_pair.second;
          while (!queue.empty() && !_isChangeValid_(queue.top())) {
            _invalidateChange_(queue.top());
          }
        }
        _queues_valid_ = true;
      }

      return _node_queue_.topPriority() == std::numeric_limits< double >::min();
    }


    /// indicates whether the selector still contain graph changes
    /// in the ith queue
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    bool GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::empty(
       const NodeId node) {
      // put into the illegal change set all the top elements of the different
      // queues that are not valid anymore
      if (!_queues_valid_) {
        for (auto& queue_pair: _change_queue_per_node_) {
          auto& queue = queue_pair.second;
          while (!queue.empty() && !_isChangeValid_(queue.top())) {
            _invalidateChange_(queue.top());
          }
        }
        _queues_valid_ = true;
      }

      return _change_queue_per_node_[node].empty();
    }


    /// returns the best graph change to examine
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    INLINE const GraphChange&
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR >::bestChange() {
      if (!empty()) return _changes_[_change_queue_per_node_[_node_queue_.top()].top()];
      else GUM_ERROR(NotFound, "there exists no graph change applicable")
    }


    /// returns the best graph change to examine in the ith queue
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    INLINE const GraphChange&
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::bestChange(
          const NodeId node) {
      if (!empty(node)) return _changes_[_change_queue_per_node_[node].top()];
      else GUM_ERROR(NotFound, "there exists no graph change applicable")
    }


    /// return the score of the best graph change
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    INLINE double
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::bestScore() {
      if (!empty()) return _change_queue_per_node_[_node_queue_.top()].topPriority();
      else GUM_ERROR(NotFound, "there exists no graph change applicable")
    }


    /// return the score of the best graph change in the ith queue
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    INLINE double
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::bestScore(
          const NodeId node) {
      if (!empty(node)) return _change_queue_per_node_[node].topPriority();
      else GUM_ERROR(NotFound, "there exists no graph change applicable")
    }


    /// remove the now legal changes from the illegal set
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::
       _illegal2LegalChanges_(Set< std::size_t >& changes_to_recompute) {
      for (auto iter = _illegal_changes_.beginSafe(); iter != _illegal_changes_.endSafe(); ++iter) {
        if (_isChangeValid_(*iter)) {
          const GraphChange& change = _changes_[*iter];
          if (change.type() == GraphChangeType::ARC_REVERSAL) {
            _change_queue_per_node_[change.node1()].insert(*iter,
                                                           std::numeric_limits< double >::min());
          }
          _change_queue_per_node_[change.node2()].insert(*iter,
                                                         std::numeric_limits< double >::min());

          changes_to_recompute.insert(*iter);
          _illegal_changes_.erase(iter);
        }
      }
    }


    /// finds the changes that are affected by a given node modification
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::
       _findLegalChangesNeedingUpdate_(Set< std::size_t >& changes_to_recompute,
                                       const NodeId        target_node) {
      const HashTable< std::size_t, Size >& changes
         = _change_queue_per_node_[target_node].allValues();
      for (auto iter = changes.cbeginSafe(); iter != changes.cendSafe(); ++iter) {
        if (!changes_to_recompute.exists(iter.key())) {
          if (_isChangeValid_(iter.key())) {
            changes_to_recompute.insert(iter.key());
          } else {
            _invalidateChange_(iter.key());
          }
        }
      }
    }


    /// perform the necessary updates of the scores
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::
       _updateScores_(const Set< std::size_t >& changes_to_recompute) {
      Set< NodeId > modified_nodes(changes_to_recompute.size());

      for (const auto change_index: changes_to_recompute) {
        const GraphChange& change = _changes_[change_index];

        switch (change.type()) {
          case GraphChangeType::ARC_ADDITION: {
            // add the arc
            auto& parents = _parents_[change.node2()];
            parents.push_back(change.node1());
            const double delta
               = _score_->score(change.node2(), parents) - _node_current_scores_[change.node2()];
            parents.pop_back();

            // update the score
            _change_scores_[change_index].second = delta;

            // update the head queue
            _change_queue_per_node_[change.node2()].setPriority(change_index, delta);
            // indicate which queue was modified
            modified_nodes.insert(change.node2());
          } break;

          case GraphChangeType::ARC_DELETION: {
            // remove the arc
            auto& parents = _parents_[change.node2()];
            for (auto& par: parents) {
              if (par == change.node1()) {
                par = *(parents.rbegin());
                parents.pop_back();
                break;
              }
            }
            const double delta
               = _score_->score(change.node2(), parents) - _node_current_scores_[change.node2()];
            parents.push_back(change.node1());

            // update the score
            _change_scores_[change_index].second = delta;

            // update the head queue
            _change_queue_per_node_[change.node2()].setPriority(change_index, delta);
            // indicate which queue was modified
            modified_nodes.insert(change.node2());
          } break;

          case GraphChangeType::ARC_REVERSAL: {
            // remove arc ( node1 -> node2 )
            auto& parents2 = _parents_[change.node2()];
            for (auto& par: parents2) {
              if (par == change.node1()) {
                par = *(parents2.rbegin());
                parents2.pop_back();
                break;
              }
            }

            const double delta2
               = _score_->score(change.node2(), parents2) - _node_current_scores_[change.node2()];
            parents2.push_back(change.node1());

            // add arc ( node2 -> node1 )
            auto& parents1 = _parents_[change.node1()];
            parents1.push_back(change.node2());
            const double delta1
               = _score_->score(change.node1(), parents1) - _node_current_scores_[change.node1()];
            parents1.pop_back();

            // update the scores
            _change_scores_[change_index].first  = delta1;
            _change_scores_[change_index].second = delta2;

            // update the queues
            const double delta = delta1 + delta2;
            _change_queue_per_node_[change.node1()].setPriority(change_index, delta);
            _change_queue_per_node_[change.node2()].setPriority(change_index, delta);

            // indicate which queues were modified
            modified_nodes.insert(change.node1());
            modified_nodes.insert(change.node2());
          } break;

          default: {
            GUM_ERROR(NotImplementedYet,
                      "Method  _updateScores_ of GraphChangesSelector4DiGraph "
                         << "does not handle yet graph change of type " << change.type());
          }
        }
      }

      // update the node queue
      for (const auto node: modified_nodes) {
        _node_queue_.setPriority(node,
                                 _change_queue_per_node_[node].empty()
                                    ? std::numeric_limits< double >::min()
                                    : _change_queue_per_node_[node].topPriority());
      }
    }


    /// get from the graph change generator a new set of changes
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                       GRAPH_CHANGES_GENERATOR >::_getNewChanges_() {
      // ask the graph change generator for all its available changes
      for (const auto& change: *_changes_generator_) {
        // check that the change does not already exist
        if (!_changes_.exists(change)) {
          // add the new change. To make the addition simple, we put the new
          // change into the illegal changes set. Afterwards, the applyChange
          // function will put the legal changes again into the queues
          _illegal_changes_.insert(_changes_.size());
          _changes_ << change;
          _change_scores_.push_back(
             std::pair< double, double >(std::numeric_limits< double >::min(),
                                         std::numeric_limits< double >::min()));
        }
      }

      // indicate to the generator that we have finished retrieving its changes
      _changes_generator_->notifyGetCompleted();
    }


    /// indicate to the selector that its best score has been applied
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    void
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::applyChange(
          const GraphChange& change) {
      // first, we get the index of the change
      const std::size_t change_index = _changes_.pos(change);

      // perform the change
      Set< std::size_t > changes_to_recompute;
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION: {
          // update the current score
          _node_current_scores_[change.node2()] += _change_scores_[change_index].second;
          _parents_[change.node2()].push_back(change.node1());

          // inform the constraint that the graph has been modified
          _constraint_->modifyGraph(static_cast< const ArcAddition& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(&(_changes_generator_->constraint()))
              != _constraint_) {
            _changes_generator_->constraint().modifyGraph(
               static_cast< const ArcAddition& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling  _illegal2LegalChanges_
          _changes_generator_->modifyGraph(static_cast< const ArcAddition& >(change));
          _getNewChanges_();

          // check whether some illegal changes can be put into the valid queues
          _illegal2LegalChanges_(changes_to_recompute);
          _invalidateChange_(change_index);
          _findLegalChangesNeedingUpdate_(changes_to_recompute, change.node2());
          _updateScores_(changes_to_recompute);
        } break;

        case GraphChangeType::ARC_DELETION: {
          // update the current score
          _node_current_scores_[change.node2()] += _change_scores_[change_index].second;
          auto& parents = _parents_[change.node2()];
          for (auto& par: parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          _constraint_->modifyGraph(static_cast< const ArcDeletion& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(&(_changes_generator_->constraint()))
              != _constraint_) {
            _changes_generator_->constraint().modifyGraph(
               static_cast< const ArcDeletion& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling  _illegal2LegalChanges_
          _changes_generator_->modifyGraph(static_cast< const ArcDeletion& >(change));
          _getNewChanges_();

          // check whether some illegal changes can be put into the valid queues
          _illegal2LegalChanges_(changes_to_recompute);
          _invalidateChange_(change_index);
          _findLegalChangesNeedingUpdate_(changes_to_recompute, change.node2());
          _updateScores_(changes_to_recompute);
        } break;

        case GraphChangeType::ARC_REVERSAL: {
          // update the current score
          _node_current_scores_[change.node1()] += _change_scores_[change_index].first;
          _node_current_scores_[change.node2()] += _change_scores_[change_index].second;
          _parents_[change.node1()].push_back(change.node2());
          auto& parents = _parents_[change.node2()];
          for (auto& par: parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          _constraint_->modifyGraph(static_cast< const ArcReversal& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(&(_changes_generator_->constraint()))
              != _constraint_) {
            _changes_generator_->constraint().modifyGraph(
               static_cast< const ArcReversal& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling  _illegal2LegalChanges_
          _changes_generator_->modifyGraph(static_cast< const ArcReversal& >(change));
          _getNewChanges_();

          // check whether some illegal changes can be put into the valid queues
          _illegal2LegalChanges_(changes_to_recompute);
          _invalidateChange_(change_index);
          _findLegalChangesNeedingUpdate_(changes_to_recompute, change.node1());
          _findLegalChangesNeedingUpdate_(changes_to_recompute, change.node2());
          _updateScores_(changes_to_recompute);
        } break;

        default:
          GUM_ERROR(NotImplementedYet,
                    "Method applyChange of GraphChangesSelector4DiGraph "
                       << "does not handle yet graph change of type " << change.type());
      }

      _queues_valid_ = false;
    }


    /// applies several changes at a time
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT, GRAPH_CHANGES_GENERATOR >::
       applyChangeWithoutScoreUpdate(const GraphChange& change) {
      // first, we get the index of the change
      const std::size_t change_index = _changes_.pos(change);

      // perform the change
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION: {
          // update the current score
          _node_current_scores_[change.node2()] += _change_scores_[change_index].second;
          _parents_[change.node2()].push_back(change.node1());

          // inform the constraint that the graph has been modified
          _constraint_->modifyGraph(static_cast< const ArcAddition& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(&(_changes_generator_->constraint()))
              != _constraint_) {
            _changes_generator_->constraint().modifyGraph(
               static_cast< const ArcAddition& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling  _illegal2LegalChanges_
          _changes_generator_->modifyGraph(static_cast< const ArcAddition& >(change));
          _getNewChanges_();

          // indicate that we have just applied the change
          _invalidateChange_(change_index);

          // indicate that the queue to which the change belongs needs be
          // updated
          _queues_to_update_.insert(change.node2());
        } break;

        case GraphChangeType::ARC_DELETION: {
          // update the current score
          _node_current_scores_[change.node2()] += _change_scores_[change_index].second;
          auto& parents = _parents_[change.node2()];
          for (auto& par: parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          _constraint_->modifyGraph(static_cast< const ArcDeletion& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(&(_changes_generator_->constraint()))
              != _constraint_) {
            _changes_generator_->constraint().modifyGraph(
               static_cast< const ArcDeletion& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling  _illegal2LegalChanges_
          _changes_generator_->modifyGraph(static_cast< const ArcDeletion& >(change));
          _getNewChanges_();

          // indicate that we have just applied the change
          _invalidateChange_(change_index);

          // indicate that the queue to which the change belongs needs be
          // updated
          _queues_to_update_.insert(change.node2());
        } break;

        case GraphChangeType::ARC_REVERSAL: {
          // update the current score
          _node_current_scores_[change.node1()] += _change_scores_[change_index].first;
          _node_current_scores_[change.node2()] += _change_scores_[change_index].second;
          _parents_[change.node1()].push_back(change.node2());
          auto& parents = _parents_[change.node2()];
          for (auto& par: parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          _constraint_->modifyGraph(static_cast< const ArcReversal& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(&(_changes_generator_->constraint()))
              != _constraint_) {
            _changes_generator_->constraint().modifyGraph(
               static_cast< const ArcReversal& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling  _illegal2LegalChanges_
          _changes_generator_->modifyGraph(static_cast< const ArcReversal& >(change));
          _getNewChanges_();

          // indicate that we have just applied the change
          _invalidateChange_(change_index);

          // indicate that the queue to which the change belongs needs be
          // updated
          _queues_to_update_.insert(change.node1());
          _queues_to_update_.insert(change.node2());
        } break;

        default:
          GUM_ERROR(NotImplementedYet,
                    "Method applyChangeWithoutScoreUpdate of "
                       << "GraphChangesSelector4DiGraph "
                       << "does not handle yet graph change of type " << change.type());
      }
    }


    /// applies several changes at a time
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    void
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR >::updateScoresAfterAppliedChanges() {
      // determine which changes in the illegal set are now legal
      Set< std::size_t > new_legal_changes;
      for (auto iter = _illegal_changes_.beginSafe(); iter != _illegal_changes_.endSafe(); ++iter) {
        if (_isChangeValid_(*iter)) {
          new_legal_changes.insert(*iter);
          _illegal_changes_.erase(iter);
        }
      }

      // update the scores that need be updated
      Set< std::size_t > changes_to_recompute;
      for (const auto& node: _queues_to_update_) {
        _findLegalChangesNeedingUpdate_(changes_to_recompute, node);
      }
      _queues_to_update_.clear();

      // put the previously illegal changes that are now legal into their queues
      for (const auto change_index: new_legal_changes) {
        const GraphChange& change = _changes_[change_index];
        if (change.type() == GraphChangeType::ARC_REVERSAL) {
          _change_queue_per_node_[change.node1()].insert(change_index,
                                                         std::numeric_limits< double >::min());
        }
        _change_queue_per_node_[change.node2()].insert(change_index,
                                                       std::numeric_limits< double >::min());

        changes_to_recompute.insert(change_index);
      }

      // compute the scores that we need
      _updateScores_(changes_to_recompute);

      _queues_valid_ = false;
    }


    /// returns the set of queues sorted by decreasing top priority
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    std::vector< std::pair< NodeId, double > >
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR >::nodesSortedByBestScore() const {
      std::vector< std::pair< NodeId, double > > result(_node_queue_.size());
      for (std::size_t i = std::size_t(0); i < _node_queue_.size(); ++i) {
        result[i].first  = _node_queue_[i];
        result[i].second = _node_queue_.priorityByPos(i);
      }

      std::sort(result.begin(),
                result.end(),
                [](const std::pair< NodeId, double >& a,
                   const std::pair< NodeId, double >& b) -> bool { return a.second > b.second; });

      return result;
    }


    /// returns the set of queues sorted by decreasing top priority
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    std::vector< std::pair< NodeId, double > >
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR >::nodesUnsortedWithScore() const {
      std::vector< std::pair< NodeId, double > > result(_node_queue_.size());
      for (std::size_t i = std::size_t(0); i < _node_queue_.size(); ++i) {
        result[i].first  = _node_queue_[i];
        result[i].second = _node_queue_.priorityByPos(i);
      }

      return result;
    }


    /// returns the generator used by the selector
    template < typename STRUCTURAL_CONSTRAINT, typename GRAPH_CHANGES_GENERATOR >
    INLINE typename GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                                  GRAPH_CHANGES_GENERATOR >::GeneratorType&
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR >::graphChangeGenerator()
          const noexcept {
      return *_changes_generator_;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
