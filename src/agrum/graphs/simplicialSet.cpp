/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
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
 * @brief source implementations of simplicial set
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <sstream>
#include <limits>
#include <cmath>
#include "graphElements.h"
#include <agrum/graphs/simplicialSet.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/simplicialSet.inl>
#endif // GUM_NO_INLINE

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===  CLASS FOR RETRIEVING SIMPLICIAL, ALMOST AND QUASI SIMPLICIAL NODES === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// constructor. initialize the simplicial set w.r.t. a given __graph

  SimplicialSet::SimplicialSet(UndiGraph *theGraph,
                               const NodeProperty<float> *theModalities,
                               NodeProperty<float> *theLogWeights, float theRatio,
                               float theThreshold)
      : __graph(theGraph ? theGraph : GUM_ERROR_IN_EXPR(
                                          OperationNotAllowed,
                                          "SimplicialSet requires a valid graph")),
        __log_weights(
            theLogWeights
                ? theLogWeights
                : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                    "SimplicialSet requires valid log weights")),
        __log_modalities(
            theModalities
                ? theModalities
                : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                    "SimplicialSet requires valid modalities")),
        __simplicial_nodes(std::less<float>(), __graph->size()),
        __almost_simplicial_nodes(std::less<float>(), __graph->size()),
        __quasi_simplicial_nodes(std::less<float>(), __graph->size()),
        __containing_list(__graph->size()),
        __nb_triangles(__graph->size() * __graph->size() / 2),
        __nb_adjacent_neighbours(__graph->size()), __quasi_ratio(theRatio),
        __log_threshold(std::log(1 + theThreshold)), __we_want_fill_ins(false) {
    if (!theGraph)
      // for debugging purposes
      GUM_CONSTRUCTOR(SimplicialSet);

    // end of initialization: compute __nb_triangles, __nb_adjacent_neighbours, etc
    __initialize();
  }

  /// copy constructor

  SimplicialSet::SimplicialSet(const SimplicialSet &from, UndiGraph *theGraph,
                               const NodeProperty<float> *theModalities,
                               NodeProperty<float> *theLogWeights, bool avoid_check)
      : __graph(theGraph ? theGraph : GUM_ERROR_IN_EXPR(
                                          OperationNotAllowed,
                                          "SimplicialSet requires a valid graph")),
        __log_weights(
            theLogWeights
                ? theLogWeights
                : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                    "SimplicialSet requires valid log weights")),
        __log_modalities(
            theModalities
                ? theModalities
                : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                    "SimplicialSet requires valid modalities")) {
    if (!avoid_check) {
      // check whether the graph, the log weights and the log_modalities
      // are similar to those of from
      if ((__graph == from.__graph) || (__log_weights == from.__log_weights) ||
          (*__graph != *from.__graph) ||
          (*__log_modalities != *from.__log_modalities)) {
        GUM_ERROR(OperationNotAllowed, "SimplicialSet requires fresh copies of "
                                       "graph, log weights and log modalities");
      }
    }

    // copy the current content of from
    *__log_weights = *from.__log_weights;
    __simplicial_nodes = from.__simplicial_nodes;
    __almost_simplicial_nodes = from.__almost_simplicial_nodes;
    __quasi_simplicial_nodes = from.__quasi_simplicial_nodes;
    __containing_list = from.__containing_list;
    __nb_triangles = from.__nb_triangles;
    __nb_adjacent_neighbours = from.__nb_adjacent_neighbours;
    __log_tree_width = from.__log_tree_width;
    __quasi_ratio = from.__quasi_ratio;
    __log_threshold = from.__log_threshold;
    __changed_status = from.__changed_status;
    __we_want_fill_ins = from.__we_want_fill_ins;
    __fill_ins_list = from.__fill_ins_list;
  }

  /// destructor

  SimplicialSet::~SimplicialSet() {
    // for debugging purposes
    GUM_DESTRUCTOR(SimplicialSet);
  }

  /// adds the necessary edges so that node 'id' and its neighbours form a clique

  void SimplicialSet::makeClique(const NodeId id) {
    // first, update the list to which belongs the node
    if (__changed_status.contains(id))
      __updateList(id);

    // to make id be a clique, we may have to add edges. Hence, this will create
    // new triangles and we should update the number of triangles passing through
    // the new edges. Moreover, we should also update the number of adjacent
    // neighbours for each node involved in these new triangles as well as the new
    // weights of the nodes. Finally, we should update the simplicial, almost and
    // quasi simplicial lists. However, to optimize the code, we use a lazy list
    // update: we just update a hashtable indicating whether a list update should
    // be performed for a given node. This enables performing the updates only
    // when necessary. if the node is known to be simplicial, there is nothing
    // to do
    if (__simplicial_nodes.contains(id)) {
      return;
    } else if (__almost_simplicial_nodes.contains(id)) {
      // get the neighbour that does not form a clique with the other neighbours
      // recall that id is an almost simplicial node if there exists a node,
      // say Y, such that, after deleting Y, id and its adjacent nodes form a
      // clique.
      const NodeSet &nei = __graph->neighbours(id);
      unsigned int nb_adj = nei.size();
      unsigned int nb = __nb_adjacent_neighbours[id];

      // nb_almost = the number of edges that should link the neighbours of
      // node id, after node Y mentioned above has been removed. Recall that
      // these neighbours and id form a clique. Hence this corresponds to the
      // number of triangles involving id and 2 of its neighbours, after node
      // Y has been removed.
      unsigned int nb_almost = ((nb_adj - 1) * (nb_adj - 2)) / 2;
      NodeId node1 = 0;

      for (const auto current_node : nei) {
        if (nb_almost == nb - __nb_triangles[Edge(current_node, id)]) {
          // we found the neighbour we were looking for: nb = the number of pairs
          // of neighbours of id that are adjacent. In other words, this is the
          // number of triangles involving node id. Now remove from it the
          // triangles involving edge (id,node1), and you get the number of
          // triangles involving id, but not node1. If id is almost simplicial,
          // then this number should be equal to the set of combinations of
          // all the possible pairs of neighbours of id except node1, hence
          // to nb_almost.
          node1 = current_node;
          break;
        }
      }

      float log_modal_node1 = (*__log_modalities)[node1];
      float &__log_weights_node1 = (*__log_weights)[node1];

      // now, to make a clique between id and its neighbours, there just remains
      // to add the missing edges between node1 and the other neighbours of id.

      // nb_n1 will contain the number of pairs of neighbours of node1 that
      // will be adjacent after the clique is constructed but that
      // are not yet adjacent
      unsigned int nb_n1 = 0;

      // update the number of triangles of the edges and keep track of the
      // nodes involved.
      for (const auto node2 : nei) {
        if ((node2 != node1) && !__graph->existsEdge(node1, node2)) {
          // add the edge
          Edge e1_2(node1, node2);
          __graph->addEdge(node1, node2);

          if (__we_want_fill_ins)
            __fill_ins_list.insert(e1_2);

          if (!__changed_status.contains(node2))
            __changed_status.insert(node2);

          __log_weights_node1 += (*__log_modalities)[node2];
          (*__log_weights)[node2] += log_modal_node1;
          __nb_triangles.insert(e1_2, 0);

          // nb_n2 will contain the number of pairs of neighbours of node2 that
          // will be adjacent after the clique is constructed but that
          // are not yet adjacent
          unsigned int nb_n2 = 0;

          // for all common neighbours of node1 and node2, update the number of
          // triangles as well as the number of adjacent neighbours
          if (__graph->neighbours(node1).size() <=
              __graph->neighbours(node2).size()) {
            for (const auto neighbour : __graph->neighbours(node1))
              if (__graph->existsEdge(neighbour, node2)) {
                // here iter->other (node1) is a neighbour of node1 and node2
                // hence there is a new triangle to be taken into account:
                // that between node1, node2 and iterEdge->other( node1 )
                ++nb_n1;
                ++nb_n2;
                ++__nb_adjacent_neighbours[neighbour];
                ++__nb_triangles[Edge(node1, neighbour)];
                ++__nb_triangles[Edge(node2, neighbour)];

                if (!__changed_status.contains(neighbour))
                  __changed_status.insert(neighbour);
              }
          } else {
            for (const auto neighbour : __graph->neighbours(node2))
              if (__graph->existsEdge(neighbour, node1)) {
                // here iter->other (node2) is a neighbour of node1 and node2
                // hence there is a new triangle to be taken into account:
                // that between node1, node2 and iterEdge->other( node2 )
                ++nb_n1;
                ++nb_n2;
                ++__nb_adjacent_neighbours[neighbour];
                ++__nb_triangles[Edge(node2, neighbour)];
                ++__nb_triangles[Edge(node1, neighbour)];

                if (!__changed_status.contains(neighbour))
                  __changed_status.insert(neighbour);
              }
          }

          __nb_adjacent_neighbours[node2] += nb_n2;
          __nb_triangles[e1_2] += nb_n2;
        }
      }

      if (!__changed_status.contains(node1))
        __changed_status.insert(node1);

      __nb_adjacent_neighbours[node1] += nb_n1;
    } else {
      // here, id is neither a simplicial node nor an almost simplicial node

      // update the number of triangles of the edges and keep track of the
      // nodes involved.
      const NodeSet &nei = __graph->neighbours(id);

      for (auto iter1 = nei.begin(); iter1 != nei.end(); ++iter1) {
        NodeId node1 = *iter1;
        float log_modal_node1 = (*__log_modalities)[node1];
        float &__log_weights_node1 = (*__log_weights)[node1];
        bool node1_status = false;
        unsigned int nb_n1 = 0;

        auto iterEdge2 = iter1;

        for (++iterEdge2; iterEdge2 != nei.end(); ++iterEdge2) {
          NodeId node2 = *iterEdge2;
          Edge e1_2(node1, node2);

          if (!__graph->existsEdge(e1_2)) {
            // add the edge
            __graph->addEdge(node1, node2);

            if (__we_want_fill_ins)
              __fill_ins_list.insert(e1_2);

            if (!__changed_status.contains(node2))
              __changed_status.insert(node2);

            node1_status = true;
            __log_weights_node1 += (*__log_modalities)[node2];
            (*__log_weights)[node2] += log_modal_node1;
            __nb_triangles.insert(e1_2, 0);

            // for all common neighbours of node1 and node2, update the number of
            // triangles as well as the number of adjacent neighbours
            unsigned int nb_n2 = 0;

            if (__graph->neighbours(node1).size() <=
                __graph->neighbours(node2).size()) {
              for (const auto neighbour : __graph->neighbours(node1))
                if (__graph->existsEdge(neighbour, node2)) {
                  // here iterEdge->other (node1) is a neighbour of
                  // both node1 and node2
                  ++nb_n1;
                  ++nb_n2;
                  ++__nb_adjacent_neighbours[neighbour];
                  ++__nb_triangles[Edge(node1, neighbour)];
                  ++__nb_triangles[Edge(node2, neighbour)];

                  if (!__changed_status.contains(neighbour))
                    __changed_status.insert(neighbour);
                }
            } else {
              for (const auto neighbour : __graph->neighbours(node2)) {
                if (__graph->existsEdge(neighbour, node1)) {
                  // here iterEdge->other (node2) is a neighbour of
                  // both node1 and node2
                  ++nb_n1;
                  ++nb_n2;
                  ++__nb_adjacent_neighbours[neighbour];
                  ++__nb_triangles[Edge(node2, neighbour)];
                  ++__nb_triangles[Edge(node1, neighbour)];

                  if (!__changed_status.contains(neighbour))
                    __changed_status.insert(neighbour);
                }
              }
            }

            __nb_adjacent_neighbours[node2] += nb_n2;
            __nb_triangles[e1_2] += nb_n2;
          }
        }

        __nb_adjacent_neighbours[node1] += nb_n1;

        if (node1_status && !__changed_status.contains(node1))
          __changed_status.insert(node1);
      }
    }

    // update the __changed_status of node id as well as its containing list
    if (!__simplicial_nodes.contains(id)) {
      if (__changed_status.contains(id))
        __changed_status.erase(id);

      switch (__containing_list[id]) {
        case __Belong::ALMOST_SIMPLICIAL:
          __almost_simplicial_nodes.erase(id);
          break;

        case __Belong::QUASI_SIMPLICIAL:
          __quasi_simplicial_nodes.erase(id);
          break;

        default:
          break;
      }

      __simplicial_nodes.insert(id, (*__log_weights)[id]);
      __containing_list[id] = __Belong::SIMPLICIAL;
    } else {
      if (__changed_status.contains(id)) {
        __changed_status.erase(id);
      }
    }
  }

  /// removes a node and its adjacent edges from the underlying __graph

  void SimplicialSet::eraseClique(const NodeId id) {
    // check if the node we wish to remove actually belongs to the __graph
    if (!__graph->exists(id)) {
      GUM_ERROR(NotFound, "the node " << id << " does not belong to the graph");
    }

    const NodeSet nei = __graph->neighbours(id);

    // check that node id is actually a clique
    unsigned int nb_adj = nei.size();

    if (__nb_adjacent_neighbours[id] != (nb_adj * (nb_adj - 1)) / 2) {
      GUM_ERROR(NotFound, "the node " << id << " is not a clique");
    }

    // remove the node and its adjacent edges
    float log_modal_id = (*__log_modalities)[id];

    for (auto iter1 = nei.begin(); iter1 != nei.end(); ++iter1) {
      NodeId node1 = *iter1;
      __nb_adjacent_neighbours[node1] -= nb_adj - 1;
      (*__log_weights)[node1] -= log_modal_id;

      if (!__changed_status.contains(node1))
        __changed_status.insert(node1);

      __nb_triangles.erase(Edge(node1, id));

      auto iter2 = iter1;

      for (++iter2; iter2 != nei.end(); ++iter2)
        --__nb_triangles[Edge(node1, *iter2)];
    }

    __log_tree_width = std::max(__log_tree_width, (*__log_weights)[id]);

    switch (__containing_list[id]) {
      case __Belong::SIMPLICIAL:
        __simplicial_nodes.erase(id);
        break;

      case __Belong::ALMOST_SIMPLICIAL:
        __almost_simplicial_nodes.erase(id);
        break;

      case __Belong::QUASI_SIMPLICIAL:
        __quasi_simplicial_nodes.erase(id);
        break;

      default:
        break;
    }

    __nb_adjacent_neighbours.erase(id);
    __containing_list.erase(id);
    __changed_status.erase(id);
    __graph->eraseNode(id);
    __log_weights->erase(id);
  }

  /// removes a node and its adjacent edges from the underlying __graph

  void SimplicialSet::eraseNode(const NodeId id) {
    // check if the node we wish to remove actually belongs to the __graph
    if (!__graph->exists(id)) {
      GUM_ERROR(NotFound, "the node " << id << " does not belong to the graph");
    }

    // remove the node and its adjacent edges
    const NodeSet &nei = __graph->neighbours(id);

    for (auto iter = nei.beginSafe(); iter != nei.endSafe();
         ++iter) // safe iterator needed here
      eraseEdge(Edge(*iter, id));

    switch (__containing_list[id]) {
      case __Belong::SIMPLICIAL:
        __simplicial_nodes.erase(id);
        break;

      case __Belong::ALMOST_SIMPLICIAL:
        __almost_simplicial_nodes.erase(id);
        break;

      case __Belong::QUASI_SIMPLICIAL:
        __quasi_simplicial_nodes.erase(id);
        break;

      default:
        break;
    }

    __nb_adjacent_neighbours.erase(id);
    __containing_list.erase(id);
    __changed_status.erase(id);
    __graph->eraseNode(id);
    __log_weights->erase(id);
  }

  /// removes a node and its adjacent edges from the underlying __graph

  void SimplicialSet::eraseEdge(const Edge &edge) {
    // check if the edge we wish to remove actually belongs to the __graph
    if (!__graph->existsEdge(edge)) {
      GUM_ERROR(NotFound, "the edge does not belong to the graph");
    }

    // get the extremal nodes of the edge
    NodeId node1 = edge.first();
    NodeId node2 = edge.second();

    // remove the edge
    __graph->eraseEdge(edge);
    __nb_triangles.erase(edge);

    // update the __log_weights of both nodes
    (*__log_weights)[node1] -= (*__log_modalities)[node2];
    (*__log_weights)[node2] -= (*__log_modalities)[node1];

    // update the number of triangles and the adjacent neighbours
    unsigned int nb_neigh_n1_n2 = 0;

    for (const auto othernode : __graph->neighbours(node1)) {
      if (__graph->existsEdge(node2, othernode)) {
        // udate the number of triangles passing through the egdes of the __graph
        --__nb_triangles[Edge(node1, othernode)];
        --__nb_triangles[Edge(node2, othernode)];
        // update the neighbours
        ++nb_neigh_n1_n2;
        --__nb_adjacent_neighbours[othernode];

        if (!__changed_status.contains(othernode))
          __changed_status.insert(othernode);
      }
    }

    __nb_adjacent_neighbours[node1] -= nb_neigh_n1_n2;
    __nb_adjacent_neighbours[node2] -= nb_neigh_n1_n2;

    if (!__changed_status.contains(node1))
      __changed_status.insert(node1);

    if (!__changed_status.contains(node2))
      __changed_status.insert(node2);
  }

  /// adds a new edge to the __graph and recomputes the simplicial set

  void SimplicialSet::insertEdge(NodeId node1, NodeId node2) {
    addEdge(node1, node2);
  }
  void SimplicialSet::addEdge(NodeId node1, NodeId node2) {
    // if the edge already exists, do nothing
    Edge edge(node1, node2);

    if (__graph->existsEdge(edge))
      return;

    // update the __log_weights of both nodes
    (*__log_weights)[node1] += (*__log_modalities)[node2];
    (*__log_weights)[node2] += (*__log_modalities)[node1];

    unsigned int nb_triangle_in_new_edge = 0;
    unsigned int nb_neigh_n1_n2 = 0;

    for (const auto othernode : __graph->neighbours(node1)) {
      if (__graph->existsEdge(node2, othernode)) {
        // udate the number of triangles passing through the egdes of the __graph
        ++__nb_triangles[Edge(node1, othernode)];
        ++__nb_triangles[Edge(node2, othernode)];
        ++nb_triangle_in_new_edge;

        // update the neighbours
        ++nb_neigh_n1_n2;
        ++__nb_adjacent_neighbours[othernode];

        if (!__changed_status.contains(othernode))
          __changed_status.insert(othernode);
      }
    }

    __nb_adjacent_neighbours[node1] += nb_neigh_n1_n2;
    __nb_adjacent_neighbours[node2] += nb_neigh_n1_n2;

    // add the edge
    __graph->addEdge(node1, node2);
    __nb_triangles.insert(edge, nb_triangle_in_new_edge);

    if (!__changed_status.contains(node1))
      __changed_status.insert(node1);

    if (!__changed_status.contains(node2))
      __changed_status.insert(node2);
  }

  /// put node id in the correct simplicial/almost simplicial/quasi simplicial list

  void SimplicialSet::__updateList(const NodeId id) {
    // check if the node belongs to the __graph
    if (!__graph->exists(id)) {
      GUM_ERROR(NotFound, "the node could not be found");
    }

    // check if the status of the node has changed
    if (!__changed_status.contains(id))
      return;

    __changed_status.erase(id);

    __Belong &belong = __containing_list[id];
    const NodeSet &nei = __graph->neighbours(id);
    unsigned int nb_adj = nei.size();

    // check if the node should belong to the simplicial set
    if (__nb_adjacent_neighbours[id] == (nb_adj * (nb_adj - 1)) / 2) {
      if (belong != __Belong::SIMPLICIAL) {
        if (belong == __Belong::ALMOST_SIMPLICIAL)
          __almost_simplicial_nodes.erase(id);
        else if (belong == __Belong::QUASI_SIMPLICIAL)
          __quasi_simplicial_nodes.erase(id);

        __simplicial_nodes.insert(id, (*__log_weights)[id]);
        belong = __Belong::SIMPLICIAL;
      }

      return;
    }

    // check if the node should belong to the almost simplicial set
    unsigned int nb_almost = ((nb_adj - 1) * (nb_adj - 2)) / 2;
    unsigned int nb = __nb_adjacent_neighbours[id];

    for (const auto cur : nei) {
      if (nb_almost == nb - __nb_triangles[Edge(cur, id)]) {
        // the node is an almost simplicial node
        if (belong != __Belong::ALMOST_SIMPLICIAL) {
          if (belong == __Belong::SIMPLICIAL)
            __simplicial_nodes.erase(id);
          else if (belong == __Belong::QUASI_SIMPLICIAL)
            __quasi_simplicial_nodes.erase(id);

          __almost_simplicial_nodes.insert(id, (*__log_weights)[id]);
          belong = __Belong::ALMOST_SIMPLICIAL;
        } else
          __almost_simplicial_nodes.setPriority(id, (*__log_weights)[id]);

        return;
      }
    }

    // check if the node should belong to the quasi simplicial nodes
    if (__nb_adjacent_neighbours[id] / ((nb_adj * (nb_adj - 1)) / 2) >=
        __quasi_ratio) {
      if (belong != __Belong::QUASI_SIMPLICIAL) {
        if (belong == __Belong::SIMPLICIAL)
          __simplicial_nodes.erase(id);
        else if (belong == __Belong::ALMOST_SIMPLICIAL)
          __almost_simplicial_nodes.erase(id);

        __quasi_simplicial_nodes.insert(id, (*__log_weights)[id]);
        belong = __Belong::QUASI_SIMPLICIAL;
      } else
        __quasi_simplicial_nodes.setPriority(id, (*__log_weights)[id]);

      return;
    }

    // the node does not belong to any list, so remove the node from
    // its current list
    if (belong == __Belong::QUASI_SIMPLICIAL)
      __quasi_simplicial_nodes.erase(id);
    else if (belong == __Belong::ALMOST_SIMPLICIAL)
      __almost_simplicial_nodes.erase(id);
    else if (belong == __Belong::SIMPLICIAL)
      __simplicial_nodes.erase(id);

    belong = __Belong::NO_LIST;
  }

  /// indicates whether there exists an almost simplicial node

  bool SimplicialSet::hasAlmostSimplicialNode() {
    // set the limit weight value
    double limit = __log_tree_width + __log_threshold;

    // update the elements currently in the almost simplicial list that may
    // now be contained in another list
    for (auto iter = __changed_status.beginSafe(); // safe iterator needed here
         iter != __changed_status.endSafe(); ++iter) {
      if (__almost_simplicial_nodes.contains(*iter))
        __updateList(*iter);
    }

    // check the current almost simplicial list
    if (!__almost_simplicial_nodes.empty() &&
        ((*__log_weights)[__almost_simplicial_nodes.top()] <= limit))
      return true;

    // if the almost simplicial list does not contain any node that has a low
    // weight, check if a node can enter the almost simplicial list
    for (auto iter = __changed_status.beginSafe(); // safe iterator needed here
         iter != __changed_status.endSafe(); ++iter) {
      __updateList(*iter);

      if (!__almost_simplicial_nodes.empty() &&
          ((*__log_weights)[__almost_simplicial_nodes.top()] <= limit))
        return true;
    }

    return false;
  }

  /// indicates whether there exists an almost simplicial node

  bool SimplicialSet::hasSimplicialNode() {
    // update the elements currently in the simplicial list that may
    // now be contained in another list
    for (auto iter = __changed_status.beginSafe(); // safe iterator needed here
         iter != __changed_status.endSafe(); ++iter) {
      if (__simplicial_nodes.contains(*iter))
        __updateList(*iter);
    }

    // check the current almost simplicial list
    if (!__simplicial_nodes.empty())
      return true;

    // if the simplicial list does not contain any node, check if a
    // node can enter the simplicial list
    for (auto iter = __changed_status.beginSafe(); // safe iterator needed here
         iter != __changed_status.endSafe(); ++iter) {
      __updateList(*iter);

      if (!__simplicial_nodes.empty())
        return true;
    }

    return false;
  }

  /// indicates whether there exists a quasi simplicial node

  bool SimplicialSet::hasQuasiSimplicialNode() {
    // set the limit weight value
    double limit = __log_tree_width + __log_threshold;

    // update the elements currently in the quasi simplicial list that may
    // now be contained in another list
    for (auto iter = __changed_status.beginSafe(); // safe iterator needed here
         iter != __changed_status.endSafe(); ++iter) {
      if (__quasi_simplicial_nodes.contains(*iter))
        __updateList(*iter);
    }

    // check the current quasi simplicial list
    if (!__quasi_simplicial_nodes.empty() &&
        ((*__log_weights)[__quasi_simplicial_nodes.top()] <= limit))
      return true;

    // if the quasi simplicial list does not contain any node that has a low
    // weight, check if a node can enter the quasi simplicial list
    for (auto iter = __changed_status.beginSafe(); // safe iterator needed here
         iter != __changed_status.endSafe(); ++iter) {
      __updateList(*iter);

      if (!__quasi_simplicial_nodes.empty() &&
          ((*__log_weights)[__quasi_simplicial_nodes.top()] <= limit))
        return true;
    }

    return false;
  }

  /** @brief initialize: compute __nb_triangles, __nb_adjacent_neighbours, etc
   * when a new graph is set */

  void SimplicialSet::__initialize() {
    // if the __graph is empty, do nothing
    if (__graph->size() == 0)
      return;

    // set the weights of the nodes and the initial tree_width (min of the weights)
    __log_tree_width = std::numeric_limits<float>::max();
    __log_weights->clear();

    for (const auto nodeX : __graph->nodes()) {
      float log_weight = (*__log_modalities)[nodeX];

      for (const auto &nei : __graph->neighbours(nodeX))
        log_weight += (*__log_modalities)[nei];

      __log_weights->insert(nodeX, log_weight);

      if (__log_tree_width > log_weight)
        __log_tree_width = log_weight;
    }

    // initialize the __nb_triangles so that there is no need to check whether
    // __nb_triangles need new insertions
    __nb_triangles = __graph->edgesProperty(0U);
    __nb_adjacent_neighbours = __graph->nodesProperty(0U);
    __containing_list = __graph->nodesProperty(__Belong::NO_LIST);
    __changed_status = __graph->asNodeSet();

    // set the __nb_triangles and the __nb_adjacent_neighbours: for each triangle,
    // update the __nb_triangles. To count the triangles only once, parse for each
    // node X the set of its neighbours Y,Z that are adjacent to each other and
    // such that the Id of Y and Z are greater than X.
    for (const auto nodeX : __graph->nodes()) {
      unsigned int &nb_adjacent_neighbours_idX = __nb_adjacent_neighbours[nodeX];

      const NodeSet &nei = __graph->neighbours(nodeX);

      for (auto iterY = nei.begin(); iterY != nei.end(); ++iterY)
        if (*iterY > nodeX) {
          NodeId node_idY = *iterY;
          unsigned int &nb_adjacent_neighbours_idY =
              __nb_adjacent_neighbours[node_idY];

          auto iterZ = iterY;

          for (++iterZ; iterZ != nei.end(); ++iterZ)
            if ((*iterZ > nodeX) && __graph->existsEdge(node_idY, *iterZ)) {
              NodeId node_idZ = *iterZ;
              ++nb_adjacent_neighbours_idX;
              ++nb_adjacent_neighbours_idY;
              ++__nb_adjacent_neighbours[node_idZ];
              ++__nb_triangles[Edge(nodeX, node_idY)];
              ++__nb_triangles[Edge(nodeX, node_idZ)];
              ++__nb_triangles[Edge(node_idZ, node_idY)];
            }
        }
    }
  }

  /// initialize the simplicial set w.r.t. a new graph

  void SimplicialSet::setGraph(UndiGraph *theGraph,
                               const NodeProperty<float> *theModalities,
                               NodeProperty<float> *theLogWeights, float theRatio,
                               float theThreshold) {
    // check that the pointers passed in argument are all different from 0
    if (!theGraph || !theModalities || !theLogWeights) {
      GUM_ERROR(OperationNotAllowed, "SimplicialSet requires non-zero pointers");
    }

    // clear the structures used for the previous graph and assign the new graph
    __graph = theGraph;
    __log_weights = theLogWeights;
    __log_modalities = theModalities;

    __simplicial_nodes.clear();
    __almost_simplicial_nodes.clear();
    __quasi_simplicial_nodes.clear();
    __simplicial_nodes.resize(__graph->size());
    __almost_simplicial_nodes.resize(__graph->size());
    __quasi_simplicial_nodes.resize(__graph->size());

    __containing_list.clear();
    __containing_list.resize(__graph->size());
    __nb_triangles.clear();
    __nb_triangles.resize(__graph->size() * __graph->size() / 2);
    __nb_adjacent_neighbours.clear();
    __nb_adjacent_neighbours.resize(__graph->size());

    __log_tree_width = std::numeric_limits<float>::max();
    __quasi_ratio = theRatio;
    __log_threshold = std::log(1 + theThreshold);
    __changed_status.clear();
    __fill_ins_list.clear();

    // end of initialization: compute __nb_triangles, __nb_adjacent_neighbours, etc
    __initialize();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
