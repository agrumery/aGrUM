/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <limits>

namespace gum {

  namespace learning {

    /// default constructor
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    INLINE GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                         GRAPH_CHANGES_GENERATOR,
                                         ALLOC >::
       GraphChangesSelector4DiGraph(Score< ALLOC >&          score,
                                    STRUCTURAL_CONSTRAINT&   constraint,
                                    GRAPH_CHANGES_GENERATOR& changes_generator) :
        score__(score.clone()),
        constraint__(&constraint), changes_generator__(&changes_generator) {
      parents__.resize(32);
      GUM_CONSTRUCTOR(GraphChangesSelector4DiGraph);
    }

    /// copy constructor
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                  GRAPH_CHANGES_GENERATOR,
                                  ALLOC >::
       GraphChangesSelector4DiGraph(
          const GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                              GRAPH_CHANGES_GENERATOR,
                                              ALLOC >& from) :
        score__(from.score__ != nullptr ? from.score__->clone() : nullptr),
        constraint__(from.constraint__),
        changes_generator__(from.changes_generator__), changes__(from.changes__),
        change_scores__(from.change_scores__),
        change_queue_per_node__(from.change_queue_per_node__),
        node_queue__(from.node_queue__), illegal_changes__(from.illegal_changes__),
        node_current_scores__(from.node_current_scores__),
        parents__(from.parents__), queues_valid__(from.queues_valid__),
        queues_to_update__(from.queues_to_update__) {
      // for debugging
      GUM_CONS_CPY(GraphChangesSelector4DiGraph);
    }

    /// move constructor
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                  GRAPH_CHANGES_GENERATOR,
                                  ALLOC >::
       GraphChangesSelector4DiGraph(
          GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                        GRAPH_CHANGES_GENERATOR,
                                        ALLOC >&& from) :
        score__(from.score__),
        constraint__(std::move(from.constraint__)),
        changes_generator__(std::move(from.changes_generator__)),
        changes__(std::move(from.changes__)),
        change_scores__(std::move(from.change_scores__)),
        change_queue_per_node__(std::move(from.change_queue_per_node__)),
        node_queue__(std::move(from.node_queue__)),
        illegal_changes__(std::move(from.illegal_changes__)),
        node_current_scores__(std::move(from.node_current_scores__)),
        parents__(std::move(from.parents__)),
        queues_valid__(std::move(from.queues_valid__)),
        queues_to_update__(std::move(from.queues_to_update__)) {
      from.score__ = nullptr;
      // for debugging
      GUM_CONS_MOV(GraphChangesSelector4DiGraph);
    }

    /// destructor
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    INLINE GraphChangesSelector4DiGraph<

       STRUCTURAL_CONSTRAINT,
       GRAPH_CHANGES_GENERATOR,
       ALLOC >::~GraphChangesSelector4DiGraph() {
      if (score__ != nullptr) {
        ALLOC< Score< ALLOC > > allocator(score__->getAllocator());
        allocator.destroy(score__);
        allocator.deallocate(score__, 1);
      }
      GUM_DESTRUCTOR(GraphChangesSelector4DiGraph);
    }

    /// copy operator
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                  GRAPH_CHANGES_GENERATOR,
                                  ALLOC >&
          GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                        GRAPH_CHANGES_GENERATOR,
                                        ALLOC >::
          operator=(const GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                                        GRAPH_CHANGES_GENERATOR,
                                                        ALLOC >& from) {
      if (this != &from) {
        // remove the old score
        if (score__ != nullptr) {
          ALLOC< Score< ALLOC > > allocator(score__->getAllocator());
          allocator.destroy(score__);
          allocator.deallocate(score__, 1);
          score__ = nullptr;
        }

        if (from.score__ != nullptr) score__ = from.score__->clone();
        constraint__            = from.constraint__;
        changes_generator__     = from.changes_generator__;
        changes__               = from.changes__;
        change_scores__         = from.change_scores__;
        change_queue_per_node__ = from.change_queue_per_node__;
        node_queue__            = from.node_queue__;
        illegal_changes__       = from.illegal_changes__;
        node_current_scores__   = from.node_current_scores__;
        parents__               = from.parents__;
        queues_valid__          = from.queues_valid__;
        queues_to_update__      = from.queues_to_update__;
      }

      return *this;
    }

    /// move operator
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                  GRAPH_CHANGES_GENERATOR,
                                  ALLOC >&
                   GraphChangesSelector4DiGraph<
                      STRUCTURAL_CONSTRAINT,
                      GRAPH_CHANGES_GENERATOR,
                      ALLOC >::operator=(GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                                           GRAPH_CHANGES_GENERATOR,
                                                           ALLOC >&& from) {
      if (this != &from) {
        score__      = from.score__;
        from.score__ = nullptr;

        constraint__            = std::move(from.constraint__);
        changes_generator__     = std::move(from.changes_generator__);
        changes__               = std::move(from.changes__);
        change_scores__         = std::move(from.change_scores__);
        change_queue_per_node__ = std::move(from.change_queue_per_node__);
        node_queue__            = std::move(from.node_queue__);
        illegal_changes__       = std::move(from.illegal_changes__);
        node_current_scores__ = std::move(from.node_current_scores__);
        parents__             = std::move(from.parents__);
        queues_valid__        = std::move(from.queues_valid__);
        queues_to_update__    = std::move(from.queues_to_update__);
      }

      return *this;
    }


    /// indicates whether a given change is valid or not
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    INLINE bool
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR,
                                     ALLOC >::isChangeValid(const GraphChange&
                                                               change) const {
      return constraint__->checkModification(change);
    }


    /// indicates whether a given change is valid or not
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    INLINE bool
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR,
                                     ALLOC >::isChangeValid__(const std::size_t
                                                                 index) const {
      return isChangeValid(changes__[index]);
    }


    /// sets the graph from which scores are computed
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                       GRAPH_CHANGES_GENERATOR,
                                       ALLOC >::setGraph(DiGraph& graph) {
      // fill the DAG with all the missing nodes
      const DatabaseTable< ALLOC >& database       = score__->database();
      const auto&                   nodeId2Columns = score__->nodeId2Columns();

      if (nodeId2Columns.empty()) {
        const NodeId nb_nodes = NodeId(database.nbVariables());
        for (NodeId i = NodeId(0); i < nb_nodes; ++i) {
          if (!graph.existsNode(i)) { graph.addNodeWithId(i); }
        }
      } else {
        for (auto iter = nodeId2Columns.cbegin(); iter != nodeId2Columns.cend();
             ++iter) {
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


      // constraint__ is the constraint used by the selector to restrict the set
      // of applicable changes. However, the generator may have a different set
      // of constraints (e.g., a constraintSliceOrder needs be tested only by the
      // generator because the changes returned by the generator will always
      // statisfy this constraint, hence the selector needs not test this
      // constraint). Therefore, if the selector and generator have different
      // constraints, both should use method setGraph() to initialize
      // themselves.
      constraint__->setGraph(graph);
      if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
             &(changes_generator__->constraint()))
          != constraint__) {
        changes_generator__->constraint().setGraph(graph);
      }

      changes_generator__->setGraph(graph);


      // save the set of parents of each node (this will speed-up the
      // computations of the scores)
      const std::size_t nb_nodes = graph.size();
      {
        const std::vector< NodeId, ALLOC< NodeId > > empty_pars;
        parents__.clear();
        parents__.resize(nb_nodes);
        for (const auto node: graph) {
          auto&          node_parents = parents__.insert(node, empty_pars).second;
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
      node_current_scores__.clear();
      node_current_scores__.resize(nb_nodes);
      for (const auto node: graph) {
        node_current_scores__.insert(node, score__->score(node, parents__[node]));
      }

      // compute all the possible changes
      changes__.clear();
      changes__.resize(nb_nodes);
      for (const auto& change: *changes_generator__) {
        changes__ << change;
      }
      changes_generator__->notifyGetCompleted();

      // determine the changes that are illegal and prepare the computation of
      // the scores of all the legal changes
      illegal_changes__.clear();

      // set the change_scores__ and change_queue_per_node__ for legal changes
      change_scores__.clear();
      change_scores__.resize(
         changes__.size(),
         std::pair< double, double >(std::numeric_limits< double >::min(),
                                     std::numeric_limits< double >::min()));
      change_queue_per_node__.clear();
      change_queue_per_node__.resize(nb_nodes);
      {
        const PriorityQueue< std::size_t, double, std::greater< double > >
           empty_prio;
        for (const auto node: graph) {
          change_queue_per_node__.insert(node, empty_prio);
        }
      }

      for (std::size_t i = std::size_t(0); i < changes__.size(); ++i) {
        if (!isChangeValid__(i)) {
          illegal_changes__.insert(i);
        } else {
          const GraphChange& change = changes__[i];

          switch (change.type()) {
            case GraphChangeType::ARC_ADDITION: {
              auto& parents = parents__[change.node2()];
              parents.push_back(change.node1());
              const double delta = score__->score(change.node2(), parents)
                                 - node_current_scores__[change.node2()];
              parents.pop_back();

              change_scores__[i].second = delta;
              change_queue_per_node__[change.node2()].insert(i, delta);
            } break;

            case GraphChangeType::ARC_DELETION: {
              auto& parents = parents__[change.node2()];
              for (auto& par: parents) {
                if (par == change.node1()) {
                  par = *(parents.rbegin());
                  parents.pop_back();
                  break;
                }
              }
              const double delta = score__->score(change.node2(), parents)
                                 - node_current_scores__[change.node2()];
              parents.push_back(change.node1());

              change_scores__[i].second = delta;
              change_queue_per_node__[change.node2()].insert(i, delta);
            } break;

            case GraphChangeType::ARC_REVERSAL: {
              // remove arc ( node1 -> node2 )
              auto& parents2 = parents__[change.node2()];
              for (auto& par: parents2) {
                if (par == change.node1()) {
                  par = *(parents2.rbegin());
                  parents2.pop_back();
                  break;
                }
              }

              const double delta2 = score__->score(change.node2(), parents2)
                                  - node_current_scores__[change.node2()];
              parents2.push_back(change.node1());

              // add arc ( node2 -> node1 )
              auto& parents1 = parents__[change.node1()];
              parents1.push_back(change.node2());
              const double delta1 = score__->score(change.node1(), parents1)
                                  - node_current_scores__[change.node1()];
              parents1.pop_back();

              change_scores__[i].first  = delta1;
              change_scores__[i].second = delta2;

              const double delta = delta1 + delta2;
              change_queue_per_node__[change.node1()].insert(i, delta);
              change_queue_per_node__[change.node2()].insert(i, delta);

            } break;

            default: {
              GUM_ERROR(NotImplementedYet,
                        "Method setGraph of GraphChangesSelector4DiGraph "
                           << "does not handle yet graph change of type "
                           << change.type());
            }
          }
        }
      }

      // update the global queue
      node_queue__.clear();
      for (const auto node: graph) {
        node_queue__.insert(node,
                            change_queue_per_node__[node].empty()
                               ? std::numeric_limits< double >::min()
                               : change_queue_per_node__[node].topPriority());
      }
      queues_valid__ = true;
      queues_to_update__.clear();
    }


    /// put a change into the illegal set
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR,
                                     ALLOC >::invalidateChange__(const std::size_t
                                                                    change_index) {
      const GraphChange& change = changes__[change_index];
      if (change.type() == GraphChangeType::ARC_REVERSAL) {
        // remove the tail change from its priority queue
        PriorityQueue< std::size_t, double, std::greater< double > >& queue1
           = change_queue_per_node__[change.node1()];
        queue1.erase(change_index);

        // recompute the top priority for the changes of the head
        const double new_priority = queue1.empty()
                                     ? std::numeric_limits< double >::min()
                                     : queue1.topPriority();
        node_queue__.setPriority(change.node1(), new_priority);
      }

      // remove the head change from its priority queue
      PriorityQueue< std::size_t, double, std::greater< double > >& queue2
         = change_queue_per_node__[change.node2()];
      queue2.erase(change_index);

      // recompute the top priority for the changes of the head
      const double new_priority = queue2.empty()
                                   ? std::numeric_limits< double >::min()
                                   : queue2.topPriority();
      node_queue__.setPriority(change.node2(), new_priority);

      // put the change into the illegal set
      illegal_changes__.insert(change_index);
    }


    /// indicates whether the selector still contain graph changes
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    bool GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                       GRAPH_CHANGES_GENERATOR,
                                       ALLOC >::empty() {
      // put into the illegal change set all the top elements of the different
      // queues that are not valid anymore
      if (!queues_valid__) {
        for (auto& queue_pair: change_queue_per_node__) {
          auto& queue = queue_pair.second;
          while (!queue.empty() && !isChangeValid__(queue.top())) {
            invalidateChange__(queue.top());
          }
        }
        queues_valid__ = true;
      }

      return node_queue__.topPriority() == std::numeric_limits< double >::min();
    }


    /// indicates whether the selector still contain graph changes
    /// in the ith queue
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    bool GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                       GRAPH_CHANGES_GENERATOR,
                                       ALLOC >::empty(const NodeId node) {
      // put into the illegal change set all the top elements of the different
      // queues that are not valid anymore
      if (!queues_valid__) {
        for (auto& queue_pair: change_queue_per_node__) {
          auto& queue = queue_pair.second;
          while (!queue.empty() && !isChangeValid__(queue.top())) {
            invalidateChange__(queue.top());
          }
        }
        queues_valid__ = true;
      }

      return change_queue_per_node__[node].empty();
    }


    /// returns the best graph change to examine
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    INLINE const GraphChange&
                 GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                               GRAPH_CHANGES_GENERATOR,
                                               ALLOC >::bestChange() {
      if (!empty())
        return changes__[change_queue_per_node__[node_queue__.top()].top()];
      else
        GUM_ERROR(NotFound, "there exists no graph change applicable");
    }


    /// returns the best graph change to examine in the ith queue
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    INLINE const GraphChange&
                 GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                               GRAPH_CHANGES_GENERATOR,
                                               ALLOC >::bestChange(const NodeId node) {
      if (!empty(node))
        return changes__[change_queue_per_node__[node].top()];
      else
        GUM_ERROR(NotFound, "there exists no graph change applicable");
    }


    /// return the score of the best graph change
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    INLINE double GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                                GRAPH_CHANGES_GENERATOR,
                                                ALLOC >::bestScore() {
      if (!empty())
        return change_queue_per_node__[node_queue__.top()].topPriority();
      else
        GUM_ERROR(NotFound, "there exists no graph change applicable");
    }


    /// return the score of the best graph change in the ith queue
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    INLINE double
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR,
                                     ALLOC >::bestScore(const NodeId node) {
      if (!empty(node))
        return change_queue_per_node__[node].topPriority();
      else
        GUM_ERROR(NotFound, "there exists no graph change applicable");
    }


    /// remove the now legal changes from the illegal set
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void GraphChangesSelector4DiGraph<
       STRUCTURAL_CONSTRAINT,
       GRAPH_CHANGES_GENERATOR,
       ALLOC >::illegal2LegalChanges__(Set< std::size_t >& changes_to_recompute) {
      for (auto iter = illegal_changes__.beginSafe();
           iter != illegal_changes__.endSafe();
           ++iter) {
        if (isChangeValid__(*iter)) {
          const GraphChange& change = changes__[*iter];
          if (change.type() == GraphChangeType::ARC_REVERSAL) {
            change_queue_per_node__[change.node1()].insert(
               *iter,
               std::numeric_limits< double >::min());
          }
          change_queue_per_node__[change.node2()].insert(
             *iter,
             std::numeric_limits< double >::min());

          changes_to_recompute.insert(*iter);
          illegal_changes__.erase(iter);
        }
      }
    }


    /// finds the changes that are affected by a given node modification
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                       GRAPH_CHANGES_GENERATOR,
                                       ALLOC >::
       findLegalChangesNeedingUpdate__(Set< std::size_t >& changes_to_recompute,
                                       const NodeId        target_node) {
      const HashTable< std::size_t, Size >& changes
         = change_queue_per_node__[target_node].allValues();
      for (auto iter = changes.cbeginSafe(); iter != changes.cendSafe(); ++iter) {
        if (!changes_to_recompute.exists(iter.key())) {
          if (isChangeValid__(iter.key())) {
            changes_to_recompute.insert(iter.key());
          } else {
            invalidateChange__(iter.key());
          }
        }
      }
    }


    /// perform the necessary updates of the scores
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void GraphChangesSelector4DiGraph<
       STRUCTURAL_CONSTRAINT,
       GRAPH_CHANGES_GENERATOR,
       ALLOC >::updateScores__(const Set< std::size_t >& changes_to_recompute) {
      Set< NodeId > modified_nodes(changes_to_recompute.size());

      for (const auto change_index: changes_to_recompute) {
        const GraphChange& change = changes__[change_index];

        switch (change.type()) {
          case GraphChangeType::ARC_ADDITION: {
            // add the arc
            auto& parents = parents__[change.node2()];
            parents.push_back(change.node1());
            const double delta = score__->score(change.node2(), parents)
                               - node_current_scores__[change.node2()];
            parents.pop_back();

            // update the score
            change_scores__[change_index].second = delta;

            // update the head queue
            change_queue_per_node__[change.node2()].setPriority(change_index,
                                                                delta);
            // indicate which queue was modified
            modified_nodes.insert(change.node2());
          } break;

          case GraphChangeType::ARC_DELETION: {
            // remove the arc
            auto& parents = parents__[change.node2()];
            for (auto& par: parents) {
              if (par == change.node1()) {
                par = *(parents.rbegin());
                parents.pop_back();
                break;
              }
            }
            const double delta = score__->score(change.node2(), parents)
                               - node_current_scores__[change.node2()];
            parents.push_back(change.node1());

            // update the score
            change_scores__[change_index].second = delta;

            // update the head queue
            change_queue_per_node__[change.node2()].setPriority(change_index,
                                                                delta);
            // indicate which queue was modified
            modified_nodes.insert(change.node2());
          } break;

          case GraphChangeType::ARC_REVERSAL: {
            // remove arc ( node1 -> node2 )
            auto& parents2 = parents__[change.node2()];
            for (auto& par: parents2) {
              if (par == change.node1()) {
                par = *(parents2.rbegin());
                parents2.pop_back();
                break;
              }
            }

            const double delta2 = score__->score(change.node2(), parents2)
                                - node_current_scores__[change.node2()];
            parents2.push_back(change.node1());

            // add arc ( node2 -> node1 )
            auto& parents1 = parents__[change.node1()];
            parents1.push_back(change.node2());
            const double delta1 = score__->score(change.node1(), parents1)
                                - node_current_scores__[change.node1()];
            parents1.pop_back();

            // update the scores
            change_scores__[change_index].first  = delta1;
            change_scores__[change_index].second = delta2;

            // update the queues
            const double delta = delta1 + delta2;
            change_queue_per_node__[change.node1()].setPriority(change_index,
                                                                delta);
            change_queue_per_node__[change.node2()].setPriority(change_index,
                                                                delta);

            // indicate which queues were modified
            modified_nodes.insert(change.node1());
            modified_nodes.insert(change.node2());
          } break;

          default: {
            GUM_ERROR(NotImplementedYet,
                      "Method updateScores__ of GraphChangesSelector4DiGraph "
                         << "does not handle yet graph change of type "
                         << change.type());
          }
        }
      }

      // update the node queue
      for (const auto node: modified_nodes) {
        node_queue__.setPriority(node,
                                 change_queue_per_node__[node].empty()
                                    ? std::numeric_limits< double >::min()
                                    : change_queue_per_node__[node].topPriority());
      }
    }


    /// get from the graph change generator a new set of changes
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                       GRAPH_CHANGES_GENERATOR,
                                       ALLOC >::getNewChanges__() {
      // ask the graph change generator for all its available changes
      for (const auto& change: *changes_generator__) {
        // check that the change does not already exist
        if (!changes__.exists(change)) {
          // add the new change. To make the addition simple, we put the new
          // change into the illegal changes set. Afterwards, the applyChange
          // function will put the legal changes again into the queues
          illegal_changes__.insert(changes__.size());
          changes__ << change;
          change_scores__.push_back(
             std::pair< double, double >(std::numeric_limits< double >::min(),
                                         std::numeric_limits< double >::min()));
        }
      }

      // indicate to the generator that we have finished retrieving its changes
      changes_generator__->notifyGetCompleted();
    }


    /// indicate to the selector that its best score has been applied
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                       GRAPH_CHANGES_GENERATOR,
                                       ALLOC >::applyChange(const GraphChange&
                                                               change) {
      // first, we get the index of the change
      const std::size_t change_index = changes__.pos(change);

      // perform the change
      Set< std::size_t > changes_to_recompute;
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION: {
          // update the current score
          node_current_scores__[change.node2()]
             += change_scores__[change_index].second;
          parents__[change.node2()].push_back(change.node1());

          // inform the constraint that the graph has been modified
          constraint__->modifyGraph(static_cast< const ArcAddition& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(changes_generator__->constraint()))
              != constraint__) {
            changes_generator__->constraint().modifyGraph(
               static_cast< const ArcAddition& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling illegal2LegalChanges__
          changes_generator__->modifyGraph(
             static_cast< const ArcAddition& >(change));
          getNewChanges__();

          // check whether some illegal changes can be put into the valid queues
          illegal2LegalChanges__(changes_to_recompute);
          invalidateChange__(change_index);
          findLegalChangesNeedingUpdate__(changes_to_recompute, change.node2());
          updateScores__(changes_to_recompute);
        } break;

        case GraphChangeType::ARC_DELETION: {
          // update the current score
          node_current_scores__[change.node2()]
             += change_scores__[change_index].second;
          auto& parents = parents__[change.node2()];
          for (auto& par: parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          constraint__->modifyGraph(static_cast< const ArcDeletion& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(changes_generator__->constraint()))
              != constraint__) {
            changes_generator__->constraint().modifyGraph(
               static_cast< const ArcDeletion& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling illegal2LegalChanges__
          changes_generator__->modifyGraph(
             static_cast< const ArcDeletion& >(change));
          getNewChanges__();

          // check whether some illegal changes can be put into the valid queues
          illegal2LegalChanges__(changes_to_recompute);
          invalidateChange__(change_index);
          findLegalChangesNeedingUpdate__(changes_to_recompute, change.node2());
          updateScores__(changes_to_recompute);
        } break;

        case GraphChangeType::ARC_REVERSAL: {
          // update the current score
          node_current_scores__[change.node1()]
             += change_scores__[change_index].first;
          node_current_scores__[change.node2()]
             += change_scores__[change_index].second;
          parents__[change.node1()].push_back(change.node2());
          auto& parents = parents__[change.node2()];
          for (auto& par: parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          constraint__->modifyGraph(static_cast< const ArcReversal& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(changes_generator__->constraint()))
              != constraint__) {
            changes_generator__->constraint().modifyGraph(
               static_cast< const ArcReversal& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling illegal2LegalChanges__
          changes_generator__->modifyGraph(
             static_cast< const ArcReversal& >(change));
          getNewChanges__();

          // check whether some illegal changes can be put into the valid queues
          illegal2LegalChanges__(changes_to_recompute);
          invalidateChange__(change_index);
          findLegalChangesNeedingUpdate__(changes_to_recompute, change.node1());
          findLegalChangesNeedingUpdate__(changes_to_recompute, change.node2());
          updateScores__(changes_to_recompute);
        } break;

        default:
          GUM_ERROR(NotImplementedYet,
                    "Method applyChange of GraphChangesSelector4DiGraph "
                       << "does not handle yet graph change of type "
                       << change.type());
      }

      queues_valid__ = false;
    }


    /// applies several changes at a time
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void GraphChangesSelector4DiGraph<
       STRUCTURAL_CONSTRAINT,
       GRAPH_CHANGES_GENERATOR,
       ALLOC >::applyChangeWithoutScoreUpdate(const GraphChange& change) {
      // first, we get the index of the change
      const std::size_t change_index = changes__.pos(change);

      // perform the change
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION: {
          // update the current score
          node_current_scores__[change.node2()]
             += change_scores__[change_index].second;
          parents__[change.node2()].push_back(change.node1());

          // inform the constraint that the graph has been modified
          constraint__->modifyGraph(static_cast< const ArcAddition& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(changes_generator__->constraint()))
              != constraint__) {
            changes_generator__->constraint().modifyGraph(
               static_cast< const ArcAddition& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling illegal2LegalChanges__
          changes_generator__->modifyGraph(
             static_cast< const ArcAddition& >(change));
          getNewChanges__();

          // indicate that we have just applied the change
          invalidateChange__(change_index);

          // indicate that the queue to which the change belongs needs be
          // updated
          queues_to_update__.insert(change.node2());
        } break;

        case GraphChangeType::ARC_DELETION: {
          // update the current score
          node_current_scores__[change.node2()]
             += change_scores__[change_index].second;
          auto& parents = parents__[change.node2()];
          for (auto& par: parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          constraint__->modifyGraph(static_cast< const ArcDeletion& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(changes_generator__->constraint()))
              != constraint__) {
            changes_generator__->constraint().modifyGraph(
               static_cast< const ArcDeletion& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling illegal2LegalChanges__
          changes_generator__->modifyGraph(
             static_cast< const ArcDeletion& >(change));
          getNewChanges__();

          // indicate that we have just applied the change
          invalidateChange__(change_index);

          // indicate that the queue to which the change belongs needs be
          // updated
          queues_to_update__.insert(change.node2());
        } break;

        case GraphChangeType::ARC_REVERSAL: {
          // update the current score
          node_current_scores__[change.node1()]
             += change_scores__[change_index].first;
          node_current_scores__[change.node2()]
             += change_scores__[change_index].second;
          parents__[change.node1()].push_back(change.node2());
          auto& parents = parents__[change.node2()];
          for (auto& par: parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          constraint__->modifyGraph(static_cast< const ArcReversal& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(changes_generator__->constraint()))
              != constraint__) {
            changes_generator__->constraint().modifyGraph(
               static_cast< const ArcReversal& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling illegal2LegalChanges__
          changes_generator__->modifyGraph(
             static_cast< const ArcReversal& >(change));
          getNewChanges__();

          // indicate that we have just applied the change
          invalidateChange__(change_index);

          // indicate that the queue to which the change belongs needs be
          // updated
          queues_to_update__.insert(change.node1());
          queues_to_update__.insert(change.node2());
        } break;

        default:
          GUM_ERROR(NotImplementedYet,
                    "Method applyChangeWithoutScoreUpdate of "
                       << "GraphChangesSelector4DiGraph "
                       << "does not handle yet graph change of type "
                       << change.type());
      }
    }


    /// applies several changes at a time
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                       GRAPH_CHANGES_GENERATOR,
                                       ALLOC >::updateScoresAfterAppliedChanges() {
      // determine which changes in the illegal set are now legal
      Set< std::size_t > new_legal_changes;
      for (auto iter = illegal_changes__.beginSafe();
           iter != illegal_changes__.endSafe();
           ++iter) {
        if (isChangeValid__(*iter)) {
          new_legal_changes.insert(*iter);
          illegal_changes__.erase(iter);
        }
      }

      // update the scores that need be updated
      Set< std::size_t > changes_to_recompute;
      for (const auto& node: queues_to_update__) {
        findLegalChangesNeedingUpdate__(changes_to_recompute, node);
      }
      queues_to_update__.clear();

      // put the previously illegal changes that are now legal into their queues
      for (const auto change_index: new_legal_changes) {
        const GraphChange& change = changes__[change_index];
        if (change.type() == GraphChangeType::ARC_REVERSAL) {
          change_queue_per_node__[change.node1()].insert(
             change_index,
             std::numeric_limits< double >::min());
        }
        change_queue_per_node__[change.node2()].insert(
           change_index,
           std::numeric_limits< double >::min());

        changes_to_recompute.insert(change_index);
      }

      // compute the scores that we need
      updateScores__(changes_to_recompute);

      queues_valid__ = false;
    }


    /// returns the set of queues sorted by decreasing top priority
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    std::vector< std::pair< NodeId, double > >
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR,
                                     ALLOC >::nodesSortedByBestScore() const {
      std::vector< std::pair< NodeId, double > > result(node_queue__.size());
      for (std::size_t i = std::size_t(0); i < node_queue__.size(); ++i) {
        result[i].first  = node_queue__[i];
        result[i].second = node_queue__.priorityByPos(i);
      }

      std::sort(result.begin(),
                result.end(),
                [](const std::pair< NodeId, double >& a,
                   const std::pair< NodeId, double >& b) -> bool {
                  return a.second > b.second;
                });

      return result;
    }


    /// returns the set of queues sorted by decreasing top priority
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    std::vector< std::pair< NodeId, double > >
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR,
                                     ALLOC >::nodesUnsortedWithScore() const {
      std::vector< std::pair< NodeId, double > > result(node_queue__.size());
      for (std::size_t i = std::size_t(0); i < node_queue__.size(); ++i) {
        result[i].first  = node_queue__[i];
        result[i].second = node_queue__.priorityByPos(i);
      }

      return result;
    }


    /// returns the generator used by the selector
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    INLINE typename GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                                  GRAPH_CHANGES_GENERATOR,
                                                  ALLOC >::GeneratorType&
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR,
                                     ALLOC >::graphChangeGenerator()
          const noexcept {
      return *changes_generator__;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
