
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
 * @brief The mecanism to compute the next available graph changes for directed
 * structure learning search algorithms.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
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
        __score(score.clone()),
        __constraint(&constraint), __changes_generator(&changes_generator) {
      __parents.resize(32);
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
        __score(from.__score != nullptr ? from.__score->clone() : nullptr),
        __constraint(from.__constraint),
        __changes_generator(from.__changes_generator), __changes(from.__changes),
        __change_scores(from.__change_scores),
        __change_queue_per_node(from.__change_queue_per_node),
        __node_queue(from.__node_queue), __illegal_changes(from.__illegal_changes),
        __node_current_scores(from.__node_current_scores),
        __parents(from.__parents), __queues_valid(from.__queues_valid),
        __queues_to_update(from.__queues_to_update) {
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
        __score(from.__score),
        __constraint(std::move(from.__constraint)),
        __changes_generator(std::move(from.__changes_generator)),
        __changes(std::move(from.__changes)),
        __change_scores(std::move(from.__change_scores)),
        __change_queue_per_node(std::move(from.__change_queue_per_node)),
        __node_queue(std::move(from.__node_queue)),
        __illegal_changes(std::move(from.__illegal_changes)),
        __node_current_scores(std::move(from.__node_current_scores)),
        __parents(std::move(from.__parents)),
        __queues_valid(std::move(from.__queues_valid)),
        __queues_to_update(std::move(from.__queues_to_update)) {
      from.__score = nullptr;
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
      if (__score != nullptr) {
        ALLOC< Score< ALLOC > > allocator(__score->getAllocator());
        allocator.destroy(__score);
        allocator.deallocate(__score, 1);
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
        if (__score != nullptr) {
          ALLOC< Score< ALLOC > > allocator(__score->getAllocator());
          allocator.destroy(__score);
          allocator.deallocate(__score, 1);
          __score = nullptr;
        }

        if (from.__score != nullptr) __score = from.__score->clone();
        __constraint = from.__constraint;
        __changes_generator = from.__changes_generator;
        __changes = from.__changes;
        __change_scores = from.__change_scores;
        __change_queue_per_node = from.__change_queue_per_node;
        __node_queue = from.__node_queue;
        __illegal_changes = from.__illegal_changes;
        __node_current_scores = from.__node_current_scores;
        __parents = from.__parents;
        __queues_valid = from.__queues_valid;
        __queues_to_update = from.__queues_to_update;
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
          GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                        GRAPH_CHANGES_GENERATOR,
                                        ALLOC >::
          operator=(GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                                  GRAPH_CHANGES_GENERATOR,
                                                  ALLOC >&& from) {
      if (this != &from) {
        __score = from.__score;
        from.__score = nullptr;

        __constraint = std::move(from.__constraint);
        __changes_generator = std::move(from.__changes_generator);
        __changes = std::move(from.__changes);
        __change_scores = std::move(from.__change_scores);
        __change_queue_per_node = std::move(from.__change_queue_per_node);
        __node_queue = std::move(from.__node_queue);
        __illegal_changes = std::move(from.__illegal_changes);
        __node_current_scores = std::move(from.__node_current_scores);
        __parents = std::move(from.__parents);
        __queues_valid = std::move(from.__queues_valid);
        __queues_to_update = std::move(from.__queues_to_update);
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
      return __constraint->checkModification(change);
    }


    /// indicates whether a given change is valid or not
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    INLINE bool
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR,
                                     ALLOC >::__isChangeValid(const std::size_t
                                                                 index) const {
      return isChangeValid(__changes[index]);
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
      const DatabaseTable< ALLOC >& database = __score->database();
      const auto&                   nodeId2Columns = __score->nodeId2Columns();

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
        for (auto node : graph) {
          if (node >= nb_nodes) { graph.eraseNode(node); }
        }
      } else {
        for (auto node : graph) {
          if (!nodeId2Columns.existsFirst(node)) { graph.eraseNode(node); }
        }
      }


      // __constraint is the constraint used by the selector to restrict the set
      // of applicable changes. However, the generator may have a different set
      // of constraints (e.g., a constraintSliceOrder needs be tested only by the
      // generator because the changes returned by the generator will always
      // statisfy this constraint, hence the selector needs not test this
      // constraint). Therefore, if the selector and generator have different
      // constraints, both should use method setGraph() to initialize
      // themselves.
      __constraint->setGraph(graph);
      if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
             &(__changes_generator->constraint()))
          != __constraint) {
        __changes_generator->constraint().setGraph(graph);
      }

      __changes_generator->setGraph(graph);


      // save the set of parents of each node (this will speed-up the
      // computations of the scores)
      const std::size_t nb_nodes = graph.size();
      {
        const std::vector< NodeId, ALLOC< NodeId > > empty_pars;
        __parents.clear();
        __parents.resize(nb_nodes);
        for (const auto node : graph) {
          auto&          node_parents = __parents.insert(node, empty_pars).second;
          const NodeSet& dag_parents = graph.parents(node);
          if (!dag_parents.empty()) {
            node_parents.resize(dag_parents.size());
            std::size_t j = std::size_t(0);
            for (const auto par : dag_parents) {
              node_parents[j] = par;
              ++j;
            }
          }
        }
      }

      // assign a score to each node given its parents in the current graph
      __node_current_scores.clear();
      __node_current_scores.resize(nb_nodes);
      for (const auto node : graph) {
        __node_current_scores.insert(node, __score->score(node, __parents[node]));
      }

      // compute all the possible changes
      __changes.clear();
      __changes.resize(nb_nodes);
      for (const auto& change : *__changes_generator) {
        __changes << change;
      }
      __changes_generator->notifyGetCompleted();

      // determine the changes that are illegal and prepare the computation of
      // the scores of all the legal changes
      __illegal_changes.clear();

      // set the __change_scores and __change_queue_per_node for legal changes
      __change_scores.clear();
      __change_scores.resize(
         __changes.size(),
         std::pair< double, double >(std::numeric_limits< double >::min(),
                                     std::numeric_limits< double >::min()));
      __change_queue_per_node.clear();
      __change_queue_per_node.resize(nb_nodes);
      {
        const PriorityQueue< std::size_t, double, std::greater< double > >
           empty_prio;
        for (const auto node : graph) {
          __change_queue_per_node.insert(node, empty_prio);
        }
      }

      for (std::size_t i = std::size_t(0); i < __changes.size(); ++i) {
        if (!__isChangeValid(i)) {
          __illegal_changes.insert(i);
        } else {
          const GraphChange& change = __changes[i];

          switch (change.type()) {
            case GraphChangeType::ARC_ADDITION: {
              auto& parents = __parents[change.node2()];
              parents.push_back(change.node1());
              const double delta = __score->score(change.node2(), parents)
                                   - __node_current_scores[change.node2()];
              parents.pop_back();

              __change_scores[i].second = delta;
              __change_queue_per_node[change.node2()].insert(i, delta);
            } break;

            case GraphChangeType::ARC_DELETION: {
              auto& parents = __parents[change.node2()];
              for (auto& par : parents) {
                if (par == change.node1()) {
                  par = *(parents.rbegin());
                  parents.pop_back();
                  break;
                }
              }
              const double delta = __score->score(change.node2(), parents)
                                   - __node_current_scores[change.node2()];
              parents.push_back(change.node1());

              __change_scores[i].second = delta;
              __change_queue_per_node[change.node2()].insert(i, delta);
            } break;

            case GraphChangeType::ARC_REVERSAL: {
              // remove arc ( node1 -> node2 )
              auto& parents2 = __parents[change.node2()];
              for (auto& par : parents2) {
                if (par == change.node1()) {
                  par = *(parents2.rbegin());
                  parents2.pop_back();
                  break;
                }
              }

              const double delta2 = __score->score(change.node2(), parents2)
                                    - __node_current_scores[change.node2()];
              parents2.push_back(change.node1());

              // add arc ( node2 -> node1 )
              auto& parents1 = __parents[change.node1()];
              parents1.push_back(change.node2());
              const double delta1 = __score->score(change.node1(), parents1)
                                    - __node_current_scores[change.node1()];
              parents1.pop_back();

              __change_scores[i].first = delta1;
              __change_scores[i].second = delta2;

              const double delta = delta1 + delta2;
              __change_queue_per_node[change.node1()].insert(i, delta);
              __change_queue_per_node[change.node2()].insert(i, delta);

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
      __node_queue.clear();
      for (const auto node : graph) {
        __node_queue.insert(node,
                            __change_queue_per_node[node].empty()
                               ? std::numeric_limits< double >::min()
                               : __change_queue_per_node[node].topPriority());
      }
      __queues_valid = true;
      __queues_to_update.clear();
    }


    /// put a change into the illegal set
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void
       GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                     GRAPH_CHANGES_GENERATOR,
                                     ALLOC >::__invalidateChange(const std::size_t
                                                                    change_index) {
      const GraphChange& change = __changes[change_index];
      if (change.type() == GraphChangeType::ARC_REVERSAL) {
        // remove the tail change from its priority queue
        PriorityQueue< std::size_t, double, std::greater< double > >& queue1 =
           __change_queue_per_node[change.node1()];
        queue1.erase(change_index);

        // recompute the top priority for the changes of the head
        const double new_priority = queue1.empty()
                                       ? std::numeric_limits< double >::min()
                                       : queue1.topPriority();
        __node_queue.setPriority(change.node1(), new_priority);
      }

      // remove the head change from its priority queue
      PriorityQueue< std::size_t, double, std::greater< double > >& queue2 =
         __change_queue_per_node[change.node2()];
      queue2.erase(change_index);

      // recompute the top priority for the changes of the head
      const double new_priority = queue2.empty()
                                     ? std::numeric_limits< double >::min()
                                     : queue2.topPriority();
      __node_queue.setPriority(change.node2(), new_priority);

      // put the change into the illegal set
      __illegal_changes.insert(change_index);
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
      if (!__queues_valid) {
        for (auto& queue_pair : __change_queue_per_node) {
          auto& queue = queue_pair.second;
          while (!queue.empty() && !__isChangeValid(queue.top())) {
            __invalidateChange(queue.top());
          }
        }
        __queues_valid = true;
      }

      return __node_queue.topPriority() == std::numeric_limits< double >::min();
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
      if (!__queues_valid) {
        for (auto& queue_pair : __change_queue_per_node) {
          auto& queue = queue_pair.second;
          while (!queue.empty() && !__isChangeValid(queue.top())) {
            __invalidateChange(queue.top());
          }
        }
        __queues_valid = true;
      }

      return __change_queue_per_node[node].empty();
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
        return __changes[__change_queue_per_node[__node_queue.top()].top()];
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
        return __changes[__change_queue_per_node[node].top()];
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
        return __change_queue_per_node[__node_queue.top()].topPriority();
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
        return __change_queue_per_node[node].topPriority();
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
       ALLOC >::__illegal2LegalChanges(Set< std::size_t >& changes_to_recompute) {
      for (auto iter = __illegal_changes.beginSafe();
           iter != __illegal_changes.endSafe();
           ++iter) {
        if (__isChangeValid(*iter)) {
          const GraphChange& change = __changes[*iter];
          if (change.type() == GraphChangeType::ARC_REVERSAL) {
            __change_queue_per_node[change.node1()].insert(
               *iter, std::numeric_limits< double >::min());
          }
          __change_queue_per_node[change.node2()].insert(
             *iter, std::numeric_limits< double >::min());

          changes_to_recompute.insert(*iter);
          __illegal_changes.erase(iter);
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
       __findLegalChangesNeedingUpdate(Set< std::size_t >& changes_to_recompute,
                                       const NodeId        target_node) {
      const HashTable< std::size_t, Size >& changes =
         __change_queue_per_node[target_node].allValues();
      for (auto iter = changes.cbeginSafe(); iter != changes.cendSafe(); ++iter) {
        if (!changes_to_recompute.exists(iter.key())) {
          if (__isChangeValid(iter.key())) {
            changes_to_recompute.insert(iter.key());
          } else {
            __invalidateChange(iter.key());
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
       ALLOC >::__updateScores(const Set< std::size_t >& changes_to_recompute) {
      Set< NodeId > modified_nodes(changes_to_recompute.size());

      for (const auto change_index : changes_to_recompute) {
        const GraphChange& change = __changes[change_index];

        switch (change.type()) {
          case GraphChangeType::ARC_ADDITION: {
            // add the arc
            auto& parents = __parents[change.node2()];
            parents.push_back(change.node1());
            const double delta = __score->score(change.node2(), parents)
                                 - __node_current_scores[change.node2()];
            parents.pop_back();

            // update the score
            __change_scores[change_index].second = delta;

            // update the head queue
            __change_queue_per_node[change.node2()].setPriority(change_index,
                                                                delta);
            // indicate which queue was modified
            modified_nodes.insert(change.node2());
          } break;

          case GraphChangeType::ARC_DELETION: {
            // remove the arc
            auto& parents = __parents[change.node2()];
            for (auto& par : parents) {
              if (par == change.node1()) {
                par = *(parents.rbegin());
                parents.pop_back();
                break;
              }
            }
            const double delta = __score->score(change.node2(), parents)
                                 - __node_current_scores[change.node2()];
            parents.push_back(change.node1());

            // update the score
            __change_scores[change_index].second = delta;

            // update the head queue
            __change_queue_per_node[change.node2()].setPriority(change_index,
                                                                delta);
            // indicate which queue was modified
            modified_nodes.insert(change.node2());
          } break;

          case GraphChangeType::ARC_REVERSAL: {
            // remove arc ( node1 -> node2 )
            auto& parents2 = __parents[change.node2()];
            for (auto& par : parents2) {
              if (par == change.node1()) {
                par = *(parents2.rbegin());
                parents2.pop_back();
                break;
              }
            }

            const double delta2 = __score->score(change.node2(), parents2)
                                  - __node_current_scores[change.node2()];
            parents2.push_back(change.node1());

            // add arc ( node2 -> node1 )
            auto& parents1 = __parents[change.node1()];
            parents1.push_back(change.node2());
            const double delta1 = __score->score(change.node1(), parents1)
                                  - __node_current_scores[change.node1()];
            parents1.pop_back();

            // update the scores
            __change_scores[change_index].first = delta1;
            __change_scores[change_index].second = delta2;

            // update the queues
            const double delta = delta1 + delta2;
            __change_queue_per_node[change.node1()].setPriority(change_index,
                                                                delta);
            __change_queue_per_node[change.node2()].setPriority(change_index,
                                                                delta);

            // indicate which queues were modified
            modified_nodes.insert(change.node1());
            modified_nodes.insert(change.node2());
          } break;

          default: {
            GUM_ERROR(NotImplementedYet,
                      "Method __updateScores of GraphChangesSelector4DiGraph "
                         << "does not handle yet graph change of type "
                         << change.type());
          }
        }
      }

      // update the node queue
      for (const auto node : modified_nodes) {
        __node_queue.setPriority(node,
                                 __change_queue_per_node[node].empty()
                                    ? std::numeric_limits< double >::min()
                                    : __change_queue_per_node[node].topPriority());
      }
    }


    /// get from the graph change generator a new set of changes
    template < typename STRUCTURAL_CONSTRAINT,
               typename GRAPH_CHANGES_GENERATOR,
               template < typename >
               class ALLOC >
    void GraphChangesSelector4DiGraph< STRUCTURAL_CONSTRAINT,
                                       GRAPH_CHANGES_GENERATOR,
                                       ALLOC >::__getNewChanges() {
      // ask the graph change generator for all its available changes
      for (const auto& change : *__changes_generator) {
        // check that the change does not already exist
        if (!__changes.exists(change)) {
          // add the new change. To make the addition simple, we put the new
          // change into the illegal changes set. Afterwards, the applyChange
          // function will put the legal changes again into the queues
          __illegal_changes.insert(__changes.size());
          __changes << change;
          __change_scores.push_back(
             std::pair< double, double >(std::numeric_limits< double >::min(),
                                         std::numeric_limits< double >::min()));
        }
      }

      // indicate to the generator that we have finished retrieving its changes
      __changes_generator->notifyGetCompleted();
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
      const std::size_t change_index = __changes.pos(change);

      // perform the change
      Set< std::size_t > changes_to_recompute;
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION: {
          // update the current score
          __node_current_scores[change.node2()] +=
             __change_scores[change_index].second;
          __parents[change.node2()].push_back(change.node1());

          // inform the constraint that the graph has been modified
          __constraint->modifyGraph(static_cast< const ArcAddition& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(__changes_generator->constraint()))
              != __constraint) {
            __changes_generator->constraint().modifyGraph(
               static_cast< const ArcAddition& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph(
             static_cast< const ArcAddition& >(change));
          __getNewChanges();

          // check whether some illegal changes can be put into the valid queues
          __illegal2LegalChanges(changes_to_recompute);
          __invalidateChange(change_index);
          __findLegalChangesNeedingUpdate(changes_to_recompute, change.node2());
          __updateScores(changes_to_recompute);
        } break;

        case GraphChangeType::ARC_DELETION: {
          // update the current score
          __node_current_scores[change.node2()] +=
             __change_scores[change_index].second;
          auto& parents = __parents[change.node2()];
          for (auto& par : parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          __constraint->modifyGraph(static_cast< const ArcDeletion& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(__changes_generator->constraint()))
              != __constraint) {
            __changes_generator->constraint().modifyGraph(
               static_cast< const ArcDeletion& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph(
             static_cast< const ArcDeletion& >(change));
          __getNewChanges();

          // check whether some illegal changes can be put into the valid queues
          __illegal2LegalChanges(changes_to_recompute);
          __invalidateChange(change_index);
          __findLegalChangesNeedingUpdate(changes_to_recompute, change.node2());
          __updateScores(changes_to_recompute);
        } break;

        case GraphChangeType::ARC_REVERSAL: {
          // update the current score
          __node_current_scores[change.node1()] +=
             __change_scores[change_index].first;
          __node_current_scores[change.node2()] +=
             __change_scores[change_index].second;
          __parents[change.node1()].push_back(change.node2());
          auto& parents = __parents[change.node2()];
          for (auto& par : parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          __constraint->modifyGraph(static_cast< const ArcReversal& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(__changes_generator->constraint()))
              != __constraint) {
            __changes_generator->constraint().modifyGraph(
               static_cast< const ArcReversal& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph(
             static_cast< const ArcReversal& >(change));
          __getNewChanges();

          // check whether some illegal changes can be put into the valid queues
          __illegal2LegalChanges(changes_to_recompute);
          __invalidateChange(change_index);
          __findLegalChangesNeedingUpdate(changes_to_recompute, change.node1());
          __findLegalChangesNeedingUpdate(changes_to_recompute, change.node2());
          __updateScores(changes_to_recompute);
        } break;

        default:
          GUM_ERROR(NotImplementedYet,
                    "Method applyChange of GraphChangesSelector4DiGraph "
                       << "does not handle yet graph change of type "
                       << change.type());
      }

      __queues_valid = false;
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
      const std::size_t change_index = __changes.pos(change);

      // perform the change
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION: {
          // update the current score
          __node_current_scores[change.node2()] +=
             __change_scores[change_index].second;
          __parents[change.node2()].push_back(change.node1());

          // inform the constraint that the graph has been modified
          __constraint->modifyGraph(static_cast< const ArcAddition& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(__changes_generator->constraint()))
              != __constraint) {
            __changes_generator->constraint().modifyGraph(
               static_cast< const ArcAddition& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph(
             static_cast< const ArcAddition& >(change));
          __getNewChanges();

          // indicate that we have just applied the change
          __invalidateChange(change_index);

          // indicate that the queue to which the change belongs needs be
          // updated
          __queues_to_update.insert(change.node2());
        } break;

        case GraphChangeType::ARC_DELETION: {
          // update the current score
          __node_current_scores[change.node2()] +=
             __change_scores[change_index].second;
          auto& parents = __parents[change.node2()];
          for (auto& par : parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          __constraint->modifyGraph(static_cast< const ArcDeletion& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(__changes_generator->constraint()))
              != __constraint) {
            __changes_generator->constraint().modifyGraph(
               static_cast< const ArcDeletion& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph(
             static_cast< const ArcDeletion& >(change));
          __getNewChanges();

          // indicate that we have just applied the change
          __invalidateChange(change_index);

          // indicate that the queue to which the change belongs needs be
          // updated
          __queues_to_update.insert(change.node2());
        } break;

        case GraphChangeType::ARC_REVERSAL: {
          // update the current score
          __node_current_scores[change.node1()] +=
             __change_scores[change_index].first;
          __node_current_scores[change.node2()] +=
             __change_scores[change_index].second;
          __parents[change.node1()].push_back(change.node2());
          auto& parents = __parents[change.node2()];
          for (auto& par : parents) {
            if (par == change.node1()) {
              par = *(parents.rbegin());
              parents.pop_back();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          __constraint->modifyGraph(static_cast< const ArcReversal& >(change));
          if (reinterpret_cast< STRUCTURAL_CONSTRAINT* >(
                 &(__changes_generator->constraint()))
              != __constraint) {
            __changes_generator->constraint().modifyGraph(
               static_cast< const ArcReversal& >(change));
          }

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph(
             static_cast< const ArcReversal& >(change));
          __getNewChanges();

          // indicate that we have just applied the change
          __invalidateChange(change_index);

          // indicate that the queue to which the change belongs needs be
          // updated
          __queues_to_update.insert(change.node1());
          __queues_to_update.insert(change.node2());
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
      for (auto iter = __illegal_changes.beginSafe();
           iter != __illegal_changes.endSafe();
           ++iter) {
        if (__isChangeValid(*iter)) {
          new_legal_changes.insert(*iter);
          __illegal_changes.erase(iter);
        }
      }

      // update the scores that need be updated
      Set< std::size_t > changes_to_recompute;
      for (const auto& node : __queues_to_update) {
        __findLegalChangesNeedingUpdate(changes_to_recompute, node);
      }
      __queues_to_update.clear();

      // put the previously illegal changes that are now legal into their queues
      for (const auto change_index : new_legal_changes) {
        const GraphChange& change = __changes[change_index];
        if (change.type() == GraphChangeType::ARC_REVERSAL) {
          __change_queue_per_node[change.node1()].insert(
             change_index, std::numeric_limits< double >::min());
        }
        __change_queue_per_node[change.node2()].insert(
           change_index, std::numeric_limits< double >::min());

        changes_to_recompute.insert(change_index);
      }

      // compute the scores that we need
      __updateScores(changes_to_recompute);

      __queues_valid = false;
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
      std::vector< std::pair< NodeId, double > > result(__node_queue.size());
      for (std::size_t i = std::size_t(0); i < __node_queue.size(); ++i) {
        result[i].first = __node_queue[i];
        result[i].second = __node_queue.priorityByPos(i);
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
      std::vector< std::pair< NodeId, double > > result(__node_queue.size());
      for (std::size_t i = std::size_t(0); i < __node_queue.size(); ++i) {
        result[i].first = __node_queue[i];
        result[i].second = __node_queue.priorityByPos(i);
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
                                     ALLOC >::graphChangeGenerator() const
       noexcept {
      return *__changes_generator;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
