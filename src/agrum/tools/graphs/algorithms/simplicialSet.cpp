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
 * @brief source implementations of simplicial set
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/graphs/algorithms/simplicialSet.h>
#include <agrum/tools/graphs/graphElements.h>
#include <limits>
#include <limits>
#include <sstream>
#include <sstream>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/simplicialSet_inl.h>
#endif   // GUM_NO_INLINE

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* ===========================================================================
   */
  /* ===========================================================================
   */
  /* ===  CLASS FOR RETRIEVING SIMPLICIAL, ALMOST AND QUASI SIMPLICIAL NODES ===
   */
  /* ===========================================================================
   */
  /* ===========================================================================
   */

  /// constructor. initialize the simplicial set w.r.t. a given graph__
  SimplicialSet::SimplicialSet(UndiGraph*                    graph,
                               const NodeProperty< double >* log_domain_sizes,
                               NodeProperty< double >*       log_weights,
                               double                        theRatio,
                               double                        theThreshold) :
      graph__(graph != nullptr
                 ? graph
                 : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                     "SimplicialSet requires a valid graph")),
      log_weights__(log_weights != nullptr
                       ? log_weights
                       : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                           "SimplicialSet "
                                           "requires non-null log weights")),
      log_domain_sizes__(log_domain_sizes != nullptr
                            ? log_domain_sizes
                            : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                                "SimplicialSet "
                                                "requires non-null domain sizes")),
      simplicial_nodes__(std::less< double >(), graph__->size()),
      almost_simplicial_nodes__(std::less< double >(), graph__->size()),
      quasi_simplicial_nodes__(std::less< double >(), graph__->size()),
      containing_list__(graph__->size()),
      nb_triangles__(graph__->size() * graph__->size() / 2),
      nb_adjacent_neighbours__(graph__->size()), quasi_ratio__(theRatio),
      log_threshold__(std::log(1 + theThreshold)) {
    if (graph != nullptr) {
      // for debugging purposes
      GUM_CONSTRUCTOR(SimplicialSet);
    }

    // end of initialization: compute nb_triangles__, nb_adjacent_neighbours__,
    // etc
    initialize__();
  }

  /// copy constructor
  SimplicialSet::SimplicialSet(const SimplicialSet&          from,
                               UndiGraph*                    graph,
                               const NodeProperty< double >* log_domain_sizes,
                               NodeProperty< double >*       log_weights,
                               bool                          avoid_check) :
      graph__(graph != nullptr
                 ? graph
                 : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                     "SimplicialSet requires a valid graph")),
      log_weights__(log_weights != nullptr
                       ? log_weights
                       : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                           "SimplicialSet "
                                           "requires non-null log weights")),
      log_domain_sizes__(
         log_domain_sizes != nullptr
            ? log_domain_sizes
            : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                "SimplicialSet "
                                "requires non-null domain sizes")) {
    if (!avoid_check) {
      // check whether the graph, the log weights and the log_modalities
      // are similar to those of from
      if ((graph__ == from.graph__) || (log_weights__ == from.log_weights__)
          || (*graph__ != *from.graph__)
          || (*log_domain_sizes__ != *from.log_domain_sizes__)) {
        GUM_ERROR(OperationNotAllowed,
                  "SimplicialSet requires fresh copies of "
                  "graph, log weights and log domain sizes");
      }
    }

    // copy the current content of from
    *log_weights__            = *from.log_weights__;
    simplicial_nodes__        = from.simplicial_nodes__;
    almost_simplicial_nodes__ = from.almost_simplicial_nodes__;
    quasi_simplicial_nodes__  = from.quasi_simplicial_nodes__;
    containing_list__         = from.containing_list__;
    nb_triangles__            = from.nb_triangles__;
    nb_adjacent_neighbours__  = from.nb_adjacent_neighbours__;
    log_tree_width__          = from.log_tree_width__;
    quasi_ratio__             = from.quasi_ratio__;
    log_threshold__           = from.log_threshold__;
    changed_status__          = from.changed_status__;
    we_want_fill_ins__        = from.we_want_fill_ins__;
    fill_ins_list__           = from.fill_ins_list__;

    // for debugging purposes
    GUM_CONS_CPY(SimplicialSet);
  }

  /// move constructor
  SimplicialSet::SimplicialSet(SimplicialSet&& from) :
      graph__(from.graph__), log_weights__(from.log_weights__),
      log_domain_sizes__(from.log_domain_sizes__),
      simplicial_nodes__(std::move(from.simplicial_nodes__)),
      almost_simplicial_nodes__(std::move(from.almost_simplicial_nodes__)),
      quasi_simplicial_nodes__(std::move(from.quasi_simplicial_nodes__)),
      containing_list__(std::move(from.containing_list__)),
      nb_triangles__(std::move(from.nb_triangles__)),
      nb_adjacent_neighbours__(std::move(from.nb_adjacent_neighbours__)),
      log_tree_width__(from.log_tree_width__), quasi_ratio__(from.quasi_ratio__),
      log_threshold__(from.log_threshold__),
      changed_status__(std::move(from.changed_status__)),
      we_want_fill_ins__(from.we_want_fill_ins__),
      fill_ins_list__(std::move(from.fill_ins_list__)) {
    // for debugging purposes
    GUM_CONS_MOV(SimplicialSet);
  }

  /// destructor
  SimplicialSet::~SimplicialSet() {
    // for debugging purposes
    GUM_DESTRUCTOR(SimplicialSet);
  }

  /// adds the necessary edges so that node 'id' and its neighbors form a clique
  void SimplicialSet::makeClique(const NodeId id) {
    // first, update the list to which belongs the node
    if (changed_status__.contains(id)) updateList__(id);

    // to make id be a clique, we may have to add edges. Hence, this will create
    // new triangles and we should update the number of triangles passing
    // through the new edges. Moreover, we should also update the number of
    // adjacent neighbors for each node involved in these new triangles as well
    // as the new weights of the nodes. Finally, we should update the
    // simplicial,
    // almost and quasi simplicial lists. However, to optimize the code, we use
    // a lazy list update: we just update a hashtable indicating whether a list
    // update should be performed for a given node. This enables performing the
    // updates only when necessary. if the node is known to be simplicial, there
    // is nothing to do
    if (simplicial_nodes__.contains(id)) {
      return;
    } else if (almost_simplicial_nodes__.contains(id)) {
      // get the neighbor that does not form a clique with the other neighbors
      // recall that id is an almost simplicial node if there exists a node,
      // say Y, such that, after deleting Y, id and its adjacent nodes form a
      // clique.
      const NodeSet& nei    = graph__->neighbours(id);
      Size           nb_adj = nei.size();
      Size           nb     = nb_adjacent_neighbours__[id];

      // nb_almost = the number of edges that should link the neighbors of
      // node id, after node Y mentioned above has been removed. Recall that
      // these neighbors and id form a clique. Hence this corresponds to the
      // number of triangles involving id and 2 of its neighbors, after node
      // Y has been removed.
      Size   nb_almost = ((nb_adj - 1) * (nb_adj - 2)) / 2;
      NodeId node1     = 0;

      for (const auto current_node: nei) {
        if (nb_almost == nb - nb_triangles__[Edge(current_node, id)]) {
          // we found the neighbor we were looking for: nb = the number of
          // pairs of neighbors of id that are adjacent. In other words, this
          // is the number of triangles involving node id. Now remove from it
          // the
          // triangles involving edge (id,node1), and you get the number of
          // triangles involving id, but not node1. If id is almost simplicial,
          // then this number should be equal to the set of combinations of
          // all the possible pairs of neighbors of id except node1, hence
          // to nb_almost.
          node1 = current_node;
          break;
        }
      }

      double  log_domain_size_node1 = (*log_domain_sizes__)[node1];
      double& log_weights_node1__   = (*log_weights__)[node1];

      // now, to make a clique between id and its neighbors, there just remains
      // to add the missing edges between node1 and the other neighbors of id.

      // nb_n1 will contain the number of pairs of neighbors of node1 that
      // will be adjacent after the clique is constructed but that
      // are not yet adjacent
      unsigned int nb_n1 = 0;

      // update the number of triangles of the edges and keep track of the
      // nodes involved.
      for (const auto node2: nei) {
        if ((node2 != node1) && !graph__->existsEdge(node1, node2)) {
          // add the edge
          const Edge e1_2(node1, node2);
          graph__->addEdge(node1, node2);

          if (we_want_fill_ins__) fill_ins_list__.insert(e1_2);

          if (!changed_status__.contains(node2)) changed_status__.insert(node2);

          log_weights_node1__ += (*log_domain_sizes__)[node2];
          (*log_weights__)[node2] += log_domain_size_node1;
          nb_triangles__.insert(e1_2, 0);

          // nb_n2 will contain the number of pairs of neighbors of node2 that
          // will be adjacent after the clique is constructed but that
          // are not yet adjacent
          unsigned int nb_n2 = 0;

          // for all common neighbors of node1 and node2, update the number of
          // triangles as well as the number of adjacent neighbors
          if (graph__->neighbours(node1).size()
              <= graph__->neighbours(node2).size()) {
            for (const auto neighbor: graph__->neighbours(node1)) {
              if (graph__->existsEdge(neighbor, node2)) {
                // here iter->other (node1) is a neighbor of node1 and node2
                // hence there is a new triangle to be taken into account:
                // that between node1, node2 and iterEdge->other( node1 )
                ++nb_n1;
                ++nb_n2;
                ++nb_adjacent_neighbours__[neighbor];
                ++nb_triangles__[Edge(node1, neighbor)];
                ++nb_triangles__[Edge(node2, neighbor)];

                if (!changed_status__.contains(neighbor))
                  changed_status__.insert(neighbor);
              }
            }
          } else {
            for (const auto neighbor: graph__->neighbours(node2)) {
              if (graph__->existsEdge(neighbor, node1)) {
                // here iter->other (node2) is a neighbor of node1 and node2
                // hence there is a new triangle to be taken into account:
                // that between node1, node2 and iterEdge->other( node2 )
                ++nb_n1;
                ++nb_n2;
                ++nb_adjacent_neighbours__[neighbor];
                ++nb_triangles__[Edge(node2, neighbor)];
                ++nb_triangles__[Edge(node1, neighbor)];

                if (!changed_status__.contains(neighbor))
                  changed_status__.insert(neighbor);
              }
            }
          }

          nb_adjacent_neighbours__[node2] += nb_n2;
          nb_triangles__[e1_2] += nb_n2;
        }
      }

      if (!changed_status__.contains(node1)) changed_status__.insert(node1);

      nb_adjacent_neighbours__[node1] += nb_n1;
    } else {
      // here, id is neither a simplicial node nor an almost simplicial node

      // update the number of triangles of the edges and keep track of the
      // nodes involved.
      const NodeSet& nei = graph__->neighbours(id);

      for (auto iter1 = nei.begin(); iter1 != nei.end(); ++iter1) {
        NodeId       node1                 = *iter1;
        double       log_domain_size_node1 = (*log_domain_sizes__)[node1];
        double&      log_weights_node1__   = (*log_weights__)[node1];
        bool         node1_status          = false;
        unsigned int nb_n1                 = 0;

        auto iterEdge2 = iter1;

        for (++iterEdge2; iterEdge2 != nei.end(); ++iterEdge2) {
          const NodeId node2 = *iterEdge2;
          const Edge   e1_2(node1, node2);

          if (!graph__->existsEdge(e1_2)) {
            // add the edge
            graph__->addEdge(node1, node2);

            if (we_want_fill_ins__) fill_ins_list__.insert(e1_2);

            if (!changed_status__.contains(node2)) changed_status__.insert(node2);

            node1_status = true;
            log_weights_node1__ += (*log_domain_sizes__)[node2];
            (*log_weights__)[node2] += log_domain_size_node1;
            nb_triangles__.insert(e1_2, 0);

            // for all common neighbors of node1 and node2, update the number
            // of triangles as well as the number of adjacent neighbors
            unsigned int nb_n2 = 0;

            if (graph__->neighbours(node1).size()
                <= graph__->neighbours(node2).size()) {
              for (const auto neighbor: graph__->neighbours(node1))
                if (graph__->existsEdge(neighbor, node2)) {
                  // here iterEdge->other (node1) is a neighbor of
                  // both node1 and node2
                  ++nb_n1;
                  ++nb_n2;
                  ++nb_adjacent_neighbours__[neighbor];
                  ++nb_triangles__[Edge(node1, neighbor)];
                  ++nb_triangles__[Edge(node2, neighbor)];

                  if (!changed_status__.contains(neighbor))
                    changed_status__.insert(neighbor);
                }
            } else {
              for (const auto neighbor: graph__->neighbours(node2)) {
                if (graph__->existsEdge(neighbor, node1)) {
                  // here iterEdge->other (node2) is a neighbor of
                  // both node1 and node2
                  ++nb_n1;
                  ++nb_n2;
                  ++nb_adjacent_neighbours__[neighbor];
                  ++nb_triangles__[Edge(node2, neighbor)];
                  ++nb_triangles__[Edge(node1, neighbor)];

                  if (!changed_status__.contains(neighbor))
                    changed_status__.insert(neighbor);
                }
              }
            }

            nb_adjacent_neighbours__[node2] += nb_n2;
            nb_triangles__[e1_2] += nb_n2;
          }
        }

        nb_adjacent_neighbours__[node1] += nb_n1;

        if (node1_status && !changed_status__.contains(node1))
          changed_status__.insert(node1);
      }
    }

    // update the changed_status__ of node id as well as its containing list
    if (!simplicial_nodes__.contains(id)) {
      if (changed_status__.contains(id)) changed_status__.erase(id);

      switch (containing_list__[id]) {
        case Belong__::ALMOST_SIMPLICIAL:
          almost_simplicial_nodes__.erase(id);
          break;

        case Belong__::QUASI_SIMPLICIAL:
          quasi_simplicial_nodes__.erase(id);
          break;

        default:
          break;
      }

      simplicial_nodes__.insert(id, (*log_weights__)[id]);
      containing_list__[id] = Belong__::SIMPLICIAL;
    } else {
      if (changed_status__.contains(id)) { changed_status__.erase(id); }
    }
  }

  /// removes a node and its adjacent edges from the underlying graph__
  void SimplicialSet::eraseClique(const NodeId id) {
    // check if the node we wish to remove actually belongs to the graph__
    if (!graph__->exists(id)) {
      GUM_ERROR(NotFound, "Node " << id << " does not belong to the graph");
    }

    const NodeSet nei = graph__->neighbours(id);

    // check that node id is actually a clique
    Size nb_adj = nei.size();
    if (nb_adjacent_neighbours__[id] != (nb_adj * (nb_adj - 1)) / 2) {
      GUM_ERROR(NotFound, "Node " << id << " is not a clique");
    }

    // remove the node and its adjacent edges
    double log_domain_size_id = (*log_domain_sizes__)[id];
    for (auto iter1 = nei.begin(); iter1 != nei.end(); ++iter1) {
      const NodeId node1 = *iter1;
      nb_adjacent_neighbours__[node1] -= nb_adj - 1;
      (*log_weights__)[node1] -= log_domain_size_id;

      if (!changed_status__.contains(node1)) changed_status__.insert(node1);

      nb_triangles__.erase(Edge(node1, id));

      auto iter2 = iter1;
      for (++iter2; iter2 != nei.end(); ++iter2)
        --nb_triangles__[Edge(node1, *iter2)];
    }

    log_tree_width__ = std::max(log_tree_width__, (*log_weights__)[id]);

    switch (containing_list__[id]) {
      case Belong__::SIMPLICIAL:
        simplicial_nodes__.erase(id);
        break;

      case Belong__::ALMOST_SIMPLICIAL:
        almost_simplicial_nodes__.erase(id);
        break;

      case Belong__::QUASI_SIMPLICIAL:
        quasi_simplicial_nodes__.erase(id);
        break;

      default:
        break;
    }

    nb_adjacent_neighbours__.erase(id);
    containing_list__.erase(id);
    changed_status__.erase(id);
    graph__->eraseNode(id);
    log_weights__->erase(id);
  }

  /// removes a node and its adjacent edges from the underlying graph__
  void SimplicialSet::eraseNode(const NodeId id) {
    // check if the node we wish to remove actually belongs to the graph__
    if (!graph__->exists(id)) {
      GUM_ERROR(NotFound, "Node " << id << " does not belong to the graph");
    }

    // remove the node and its adjacent edges
    const NodeSet& nei = graph__->neighbours(id);

    for (auto iter = nei.beginSafe(); iter != nei.endSafe();
         ++iter)   // safe iterator needed here for deletions
      eraseEdge(Edge(*iter, id));

    switch (containing_list__[id]) {
      case Belong__::SIMPLICIAL:
        simplicial_nodes__.erase(id);
        break;

      case Belong__::ALMOST_SIMPLICIAL:
        almost_simplicial_nodes__.erase(id);
        break;

      case Belong__::QUASI_SIMPLICIAL:
        quasi_simplicial_nodes__.erase(id);
        break;

      default:
        break;
    }

    nb_adjacent_neighbours__.erase(id);
    containing_list__.erase(id);
    changed_status__.erase(id);
    graph__->eraseNode(id);
    log_weights__->erase(id);
  }

  /// removes a node and its adjacent edges from the underlying graph__
  void SimplicialSet::eraseEdge(const Edge& edge) {
    // check if the edge we wish to remove actually belongs to the graph__
    if (!graph__->existsEdge(edge)) {
      GUM_ERROR(NotFound, "Edge " << edge << " does not belong to the graph");
    }

    // get the extremal nodes of the edge
    const NodeId node1 = edge.first();
    const NodeId node2 = edge.second();

    // remove the edge
    graph__->eraseEdge(edge);
    nb_triangles__.erase(edge);

    // update the log_weights__ of both nodes
    (*log_weights__)[node1] -= (*log_domain_sizes__)[node2];
    (*log_weights__)[node2] -= (*log_domain_sizes__)[node1];

    // update the number of triangles and the adjacent neighbors
    unsigned int nb_neigh_n1_n2 = 0;
    for (const auto othernode: graph__->neighbours(node1)) {
      if (graph__->existsEdge(node2, othernode)) {
        // udate the number of triangles passing through the egdes of the
        // graph__
        --nb_triangles__[Edge(node1, othernode)];
        --nb_triangles__[Edge(node2, othernode)];
        // update the neighbors
        ++nb_neigh_n1_n2;
        --nb_adjacent_neighbours__[othernode];

        if (!changed_status__.contains(othernode))
          changed_status__.insert(othernode);
      }
    }

    nb_adjacent_neighbours__[node1] -= nb_neigh_n1_n2;
    nb_adjacent_neighbours__[node2] -= nb_neigh_n1_n2;

    if (!changed_status__.contains(node1)) changed_status__.insert(node1);
    if (!changed_status__.contains(node2)) changed_status__.insert(node2);
  }

  /// adds a new edge to the graph__ and recomputes the simplicial set
  void SimplicialSet::addEdge(NodeId node1, NodeId node2) {
    // if the edge already exists, do nothing
    const Edge edge(node1, node2);

    if (graph__->existsEdge(edge)) return;

    // update the log_weights__ of both nodes
    (*log_weights__)[node1] += (*log_domain_sizes__)[node2];
    (*log_weights__)[node2] += (*log_domain_sizes__)[node1];

    unsigned int nb_triangle_in_new_edge = 0;
    unsigned int nb_neigh_n1_n2          = 0;

    for (const auto othernode: graph__->neighbours(node1)) {
      if (graph__->existsEdge(node2, othernode)) {
        // udate the number of triangles passing through the egdes of the
        // graph__
        ++nb_triangles__[Edge(node1, othernode)];
        ++nb_triangles__[Edge(node2, othernode)];
        ++nb_triangle_in_new_edge;

        // update the neighbors
        ++nb_neigh_n1_n2;
        ++nb_adjacent_neighbours__[othernode];

        if (!changed_status__.contains(othernode))
          changed_status__.insert(othernode);
      }
    }

    nb_adjacent_neighbours__[node1] += nb_neigh_n1_n2;
    nb_adjacent_neighbours__[node2] += nb_neigh_n1_n2;

    // add the edge
    graph__->addEdge(node1, node2);
    nb_triangles__.insert(edge, nb_triangle_in_new_edge);

    if (!changed_status__.contains(node1)) changed_status__.insert(node1);
    if (!changed_status__.contains(node2)) changed_status__.insert(node2);
  }

  /// put node id in the correct simplicial/almost simplicial/quasi simplicial
  /// list
  void SimplicialSet::updateList__(const NodeId id) {
    // check if the node belongs to the graph__
    if (!graph__->exists(id)) {
      GUM_ERROR(NotFound, "Node " << id << " could not be found");
    }

    // check if the status of the node has changed
    if (!changed_status__.contains(id)) return;

    changed_status__.erase(id);

    Belong__&      belong = containing_list__[id];
    const NodeSet& nei    = graph__->neighbours(id);
    Size           nb_adj = nei.size();

    // check if the node should belong to the simplicial set
    if (nb_adjacent_neighbours__[id] == (nb_adj * (nb_adj - 1)) / 2) {
      if (belong != Belong__::SIMPLICIAL) {
        if (belong == Belong__::ALMOST_SIMPLICIAL)
          almost_simplicial_nodes__.erase(id);
        else if (belong == Belong__::QUASI_SIMPLICIAL)
          quasi_simplicial_nodes__.erase(id);

        simplicial_nodes__.insert(id, (*log_weights__)[id]);
        belong = Belong__::SIMPLICIAL;
      }

      return;
    }

    // check if the node should belong to the almost simplicial set
    Size nb_almost = ((nb_adj - 1) * (nb_adj - 2)) / 2;
    Size nb        = nb_adjacent_neighbours__[id];

    for (const auto cur: nei) {
      if (nb_almost == nb - nb_triangles__[Edge(cur, id)]) {
        // the node is an almost simplicial node
        if (belong != Belong__::ALMOST_SIMPLICIAL) {
          if (belong == Belong__::SIMPLICIAL)
            simplicial_nodes__.erase(id);
          else if (belong == Belong__::QUASI_SIMPLICIAL)
            quasi_simplicial_nodes__.erase(id);

          almost_simplicial_nodes__.insert(id, (*log_weights__)[id]);
          belong = Belong__::ALMOST_SIMPLICIAL;
        } else
          almost_simplicial_nodes__.setPriority(id, (*log_weights__)[id]);

        return;
      }
    }

    // check if the node should belong to the quasi simplicial nodes
    if (nb_adjacent_neighbours__[id] / ((nb_adj * (nb_adj - 1)) / 2)
        >= quasi_ratio__) {
      if (belong != Belong__::QUASI_SIMPLICIAL) {
        if (belong == Belong__::SIMPLICIAL)
          simplicial_nodes__.erase(id);
        else if (belong == Belong__::ALMOST_SIMPLICIAL)
          almost_simplicial_nodes__.erase(id);

        quasi_simplicial_nodes__.insert(id, (*log_weights__)[id]);
        belong = Belong__::QUASI_SIMPLICIAL;
      } else
        quasi_simplicial_nodes__.setPriority(id, (*log_weights__)[id]);

      return;
    }

    // the node does not belong to any list, so remove the node from
    // its current list
    if (belong == Belong__::QUASI_SIMPLICIAL)
      quasi_simplicial_nodes__.erase(id);
    else if (belong == Belong__::ALMOST_SIMPLICIAL)
      almost_simplicial_nodes__.erase(id);
    else if (belong == Belong__::SIMPLICIAL)
      simplicial_nodes__.erase(id);

    belong = Belong__::NO_LIST;
  }

  /// indicates whether there exists an almost simplicial node
  bool SimplicialSet::hasAlmostSimplicialNode() {
    // set the limit weight value
    double limit = log_tree_width__ + log_threshold__;

    // update the elements currently in the almost simplicial list that may
    // now be contained in another list
    for (auto iter = changed_status__.beginSafe();   // safe iterator needed here
         iter != changed_status__.endSafe();
         ++iter) {
      if (almost_simplicial_nodes__.contains(*iter)) updateList__(*iter);
    }

    // check the current almost simplicial list
    if (!almost_simplicial_nodes__.empty()
        && ((*log_weights__)[almost_simplicial_nodes__.top()] <= limit))
      return true;

    // if the almost simplicial list does not contain any node that has a low
    // weight, check if a node can enter the almost simplicial list
    for (auto iter = changed_status__.beginSafe();   // safe iterator needed here
         iter != changed_status__.endSafe();
         ++iter) {
      updateList__(*iter);

      if (!almost_simplicial_nodes__.empty()
          && ((*log_weights__)[almost_simplicial_nodes__.top()] <= limit))
        return true;
    }

    return false;
  }

  /// indicates whether there exists an almost simplicial node
  bool SimplicialSet::hasSimplicialNode() {
    // update the elements currently in the simplicial list that may
    // now be contained in another list
    for (auto iter = changed_status__.beginSafe();   // safe iterator needed here
         iter != changed_status__.endSafe();
         ++iter) {
      if (simplicial_nodes__.contains(*iter)) updateList__(*iter);
    }

    // check the current almost simplicial list
    if (!simplicial_nodes__.empty()) return true;

    // if the simplicial list does not contain any node, check if a
    // node can enter the simplicial list
    for (auto iter = changed_status__.beginSafe();   // safe iterator needed here
         iter != changed_status__.endSafe();
         ++iter) {
      updateList__(*iter);

      if (!simplicial_nodes__.empty()) return true;
    }

    return false;
  }

  /// indicates whether there exists a quasi simplicial node
  bool SimplicialSet::hasQuasiSimplicialNode() {
    // set the limit weight value
    double limit = log_tree_width__ + log_threshold__;

    // update the elements currently in the quasi simplicial list that may
    // now be contained in another list
    for (auto iter = changed_status__.beginSafe();   // safe iterator needed here
         iter != changed_status__.endSafe();
         ++iter) {
      if (quasi_simplicial_nodes__.contains(*iter)) updateList__(*iter);
    }

    // check the current quasi simplicial list
    if (!quasi_simplicial_nodes__.empty()
        && ((*log_weights__)[quasi_simplicial_nodes__.top()] <= limit))
      return true;

    // if the quasi simplicial list does not contain any node that has a low
    // weight, check if a node can enter the quasi simplicial list
    for (auto iter = changed_status__.beginSafe();   // safe iterator needed here
         iter != changed_status__.endSafe();
         ++iter) {
      updateList__(*iter);

      if (!quasi_simplicial_nodes__.empty()
          && ((*log_weights__)[quasi_simplicial_nodes__.top()] <= limit))
        return true;
    }

    return false;
  }

  /** @brief initialize: compute nb_triangles__, nb_adjacent_neighbours__, etc
   * when a new graph is set */
  void SimplicialSet::initialize__() {
    // if the graph__ is empty, do nothing
    if (graph__->size() == 0) return;

    // set the weights of the nodes and the initial tree_width (min of the
    // weights)
    log_tree_width__ = std::numeric_limits< double >::max();
    log_weights__->clear();

    for (const auto nodeX: *graph__) {
      double log_weight = (*log_domain_sizes__)[nodeX];
      for (const auto& nei: graph__->neighbours(nodeX))
        log_weight += (*log_domain_sizes__)[nei];

      log_weights__->insert(nodeX, log_weight);
      if (log_tree_width__ > log_weight) log_tree_width__ = log_weight;
    }

    // initialize the nb_triangles__ so that there is no need to check whether
    // nb_triangles__ need new insertions
    nb_triangles__           = graph__->edgesProperty(Size(0));
    nb_adjacent_neighbours__ = graph__->nodesProperty(Size(0));
    containing_list__        = graph__->nodesProperty(Belong__::NO_LIST);
    changed_status__         = graph__->asNodeSet();

    // set the nb_triangles__ and the nb_adjacent_neighbours__: for each
    // triangle, update the nb_triangles__. To count the triangles only once,
    // parse for each node X the set of its neighbors Y,Z that are adjacent to
    // each other and such that the Id of Y and Z are greater than X.
    for (const auto nodeX: *graph__) {
      Size&          nb_adjacent_neighbors_idX = nb_adjacent_neighbours__[nodeX];
      const NodeSet& nei                       = graph__->neighbours(nodeX);

      for (auto iterY = nei.begin(); iterY != nei.end(); ++iterY)
        if (*iterY > nodeX) {
          const NodeId node_idY           = *iterY;
          Size& nb_adjacent_neighbors_idY = nb_adjacent_neighbours__[node_idY];

          auto iterZ = iterY;
          for (++iterZ; iterZ != nei.end(); ++iterZ)
            if ((*iterZ > nodeX) && graph__->existsEdge(node_idY, *iterZ)) {
              const NodeId node_idZ = *iterZ;
              ++nb_adjacent_neighbors_idX;
              ++nb_adjacent_neighbors_idY;
              ++nb_adjacent_neighbours__[node_idZ];
              ++nb_triangles__[Edge(nodeX, node_idY)];
              ++nb_triangles__[Edge(nodeX, node_idZ)];
              ++nb_triangles__[Edge(node_idZ, node_idY)];
            }
        }
    }
  }

  /// initialize the simplicial set w.r.t. a new graph
  void SimplicialSet::setGraph(UndiGraph*                    graph,
                               const NodeProperty< double >* log_domain_sizes,
                               NodeProperty< double >*       log_weights,
                               double                        theRatio,
                               double                        theThreshold) {
    // check that the pointers passed in argument are all different from 0
    if ((graph == nullptr) || (log_domain_sizes == nullptr)
        || (log_weights == nullptr)) {
      GUM_ERROR(OperationNotAllowed, "SimplicialSet requires non-null pointers");
    }

    // clear the structures used for the previous graph and assign the new graph
    graph__            = graph;
    log_weights__      = log_weights;
    log_domain_sizes__ = log_domain_sizes;

    simplicial_nodes__.clear();
    almost_simplicial_nodes__.clear();
    quasi_simplicial_nodes__.clear();
    simplicial_nodes__.resize(graph__->size());
    almost_simplicial_nodes__.resize(graph__->size());
    quasi_simplicial_nodes__.resize(graph__->size());

    containing_list__.clear();
    containing_list__.resize(graph__->size());
    nb_triangles__.clear();
    nb_triangles__.resize(graph__->size() * graph__->size() / 2);
    nb_adjacent_neighbours__.clear();
    nb_adjacent_neighbours__.resize(graph__->size());

    log_tree_width__ = std::numeric_limits< double >::max();
    quasi_ratio__    = theRatio;
    log_threshold__  = std::log(1 + theThreshold);
    changed_status__.clear();
    fill_ins_list__.clear();

    // end of initialization: compute nb_triangles__, nb_adjacent_neighbours__,
    // etc
    initialize__();
  }

  /// reassigns a new set of cliques' log weights (with the same content)
  void SimplicialSet::replaceLogWeights(NodeProperty< double >* old_weights,
                                        NodeProperty< double >* new_weights) {
    // check that the current weights are the old_weights
    if (old_weights != log_weights__)
      GUM_ERROR(InvalidArgument,
                "the old set of weights shall be identical "
                "to the current one");

    log_weights__ = new_weights;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
