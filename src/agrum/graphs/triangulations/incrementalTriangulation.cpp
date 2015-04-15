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
 * @brief source implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <utility>
#include <limits>
#include <cmath>

#include <agrum/config.h>

#include <agrum/core/list.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/triangulations/incrementalTriangulation.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/triangulations/incrementalTriangulation.inl>
#endif // GUM_NO_INLINE

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===            CLASS FOR COMPUTING INCREMENTAL TRIANGULATIONS           === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// constructor

  IncrementalTriangulation::IncrementalTriangulation(
      const UnconstrainedTriangulation &triang_algo, const UndiGraph &theGraph,
      const NodeProperty<Size> &modal)
      : __triangulation(triang_algo.newFactory()), __require_update(false),
        __require_elimination_order(false), __require_created_JT_cliques(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(IncrementalTriangulation);

    // reset the triangulation algorithm => it starts with an empty graph
    __triangulation->clear();

    // copy the graph passed in argument and update the structures
    // containing the informations useful for the triangulation

    for (const auto node : theGraph.nodes())
      addNode(node, modal[node]);

    // insert all the edges of the graph into the structure. This will
    // implicitly update the "require_update" field
    for (const auto edge : theGraph.edges())
      addEdge(edge.first(), edge.second());
  }

  /// default constructor

  IncrementalTriangulation::IncrementalTriangulation(
      const UnconstrainedTriangulation &triang_algo)
      : __triangulation(triang_algo.newFactory()), __require_update(false),
        __require_elimination_order(false), __require_created_JT_cliques(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(IncrementalTriangulation);

    // reset the triangulation algorithm => it starts with an empty graph
    __triangulation->clear();
  }

  /// copy operator

  IncrementalTriangulation::IncrementalTriangulation(
      const IncrementalTriangulation &from)
      : __graph(from.__graph), __junction_tree(from.__junction_tree),
        __T_mpd(from.__T_mpd), __mps_of_node(from.__mps_of_node),
        __cliques_of_mps(from.__cliques_of_mps),
        __mps_of_clique(from.__mps_of_clique), __mps_affected(from.__mps_affected),
        __triangulation(from.__triangulation->newFactory()),
        __require_update(from.__require_update),
        __require_elimination_order(from.__require_elimination_order),
        __elimination_order(from.__elimination_order),
        __reverse_elimination_order(from.__reverse_elimination_order),
        __require_created_JT_cliques(from.__require_created_JT_cliques),
        __created_JT_cliques(from.__created_JT_cliques) {
    // for debugging purposes
    GUM_CONS_CPY(IncrementalTriangulation);

    _modalities = from._modalities;
  }

  /// destructor

  IncrementalTriangulation::~IncrementalTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(IncrementalTriangulation);

    // remove things that were allocated within the current class
    delete __triangulation;
  }

  /// virtual copy constructor

  IncrementalTriangulation *IncrementalTriangulation::newFactory() const {
    return new IncrementalTriangulation(*__triangulation);
  }

  /// copy operator

  IncrementalTriangulation &IncrementalTriangulation::
  operator=(const IncrementalTriangulation &from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(IncrementalTriangulation);

      // copy all the structures stored in "from"
      __graph = from.__graph;
      _modalities = from._modalities;
      __junction_tree = from.__junction_tree;
      __T_mpd = from.__T_mpd;
      __mps_of_node = from.__mps_of_node;
      __cliques_of_mps = from.__cliques_of_mps;
      __mps_of_clique = from.__mps_of_clique;
      __mps_affected = from.__mps_affected;
      __require_update = from.__require_update;
      __require_elimination_order = from.__require_elimination_order;
      __elimination_order = from.__elimination_order;
      __reverse_elimination_order = from.__reverse_elimination_order;
      __require_created_JT_cliques = from.__require_created_JT_cliques;
      __created_JT_cliques = from.__created_JT_cliques;

      // just in case we changed the triangulation algorithm, we remove it
      // and create it again
      delete __triangulation;
      __triangulation = from.__triangulation->newFactory();
    }

    return *this;
  }

  /// adds a new node to the graph (and update the internal structures)

  void IncrementalTriangulation::insertNode(const NodeId node, Size modal) {
    addNode(node, modal);
  }

  void IncrementalTriangulation::addNode(const NodeId node, Size modal) {
    // check if the node already exists
    if (__graph.existsNode(node))
      return;

    // add the new node to the graph
    __graph.addNode(node);
    _modalities.insert(node, modal);

    // add a new clique to T_mpd and the junction tree
    NodeSet clique_nodes(2);
    clique_nodes.insert(node);

    NodeId MPS = __T_mpd.addNode(clique_nodes);
    NodeId new_clique = __junction_tree.addNode(clique_nodes);

    // indicate in which MPS node belongs
    List<NodeId> &list_of_mps = __mps_of_node.insert(node, List<NodeId>()).second;
    list_of_mps.insert(MPS);

    // indicate in which MPS the clique added to the junction tree belongs
    std::vector<NodeId> &cliquesMPS =
        __cliques_of_mps.insert(MPS, std::vector<NodeId>()).second;

    cliquesMPS.push_back(new_clique);
    __mps_of_clique.insert(new_clique, MPS);

    // indicate that the new MPS should not be affected by a triangulation
    __mps_affected.insert(MPS, false);

    // insert the node into the elimination order sequence
    __elimination_order.push_back(node);

    if (!__reverse_elimination_order.exists(node))
      __reverse_elimination_order.insert(node, __elimination_order.size());

    if (!__created_JT_cliques.exists(node))
      __created_JT_cliques.insert(node, new_clique);
  }

  /// mark the mps affected by the deletion of a given edge

  void IncrementalTriangulation::__markAffectedMPSsByRemoveLink(const NodeId My,
                                                                const NodeId Mz,
                                                                const Edge &edge) {
    // mark the MPS so that it will be retriangulated
    __mps_affected[My] = true;

    // mark all the neighbour MPS that contain edge
    for (const auto nei : __T_mpd.neighbours(My))
      if (nei != Mz) {
        const NodeSet &Syk = __T_mpd.separator(Edge(nei, My));

        if (Syk.contains(edge.first()) && Syk.contains(edge.second()))
          __markAffectedMPSsByRemoveLink(nei, My, edge);
      }
  }

  /// removes an edge from the graph (the join tree may need a retriangulation)

  void IncrementalTriangulation::eraseEdge(const Edge &edge) {
    // check that the edge exist
    if (!__graph.existsEdge(edge))
      return;

    // find a MPS containing the edge (X,Y)
    const NodeId X = edge.first();
    const NodeId Y = edge.second();

    const List<NodeId> &mps1 = __mps_of_node[X];
    const List<NodeId> &mps2 = __mps_of_node[Y];

    NodeId Mx = mps1[0];

    if (mps1.size() <= mps2.size()) {
      for (const auto node : mps1)
        if (__T_mpd.clique(node).contains(Y)) {
          Mx = node;
          break;
        }
    } else {
      for (const auto node : mps2)
        if (__T_mpd.clique(node).contains(X)) {
          Mx = node;
          break;
        }
    }

    // mark the MPS that need be updated
    __markAffectedMPSsByRemoveLink(Mx, Mx, edge);

    __require_update = true;

    __require_elimination_order = true;

    __require_created_JT_cliques = true;

    // remove the edge (X,Y) from the graph
    __graph.eraseEdge(edge);
  }

  /// removes a node from the graph (the join tree may need a triangulation update)

  void IncrementalTriangulation::eraseNode(const NodeId X) {
    // check if the node exists
    if (!__graph.existsNode(X))
      return;

    // remove all the edges adjacent to the node
    {
      const NodeSet &neighbours = __graph.neighbours(X);

      for (auto neighbour_edge = neighbours.beginSafe(); // safe iterator needed here
           neighbour_edge != neighbours.endSafe(); ++neighbour_edge) {
        eraseEdge(Edge(*neighbour_edge, X));
      }
    }

    auto &MPS_of_X = __mps_of_node[X];

    // remove X from the MPS containing X
    for (const auto node : MPS_of_X) {
      __T_mpd.eraseFromClique(node, X);

      // if the intersection between *iter and one of its neighbour is empty,
      // remove the edge linking them
      auto &neighbours = __T_mpd.neighbours(node);

      for (auto it_neighbour = neighbours.beginSafe();
           it_neighbour != neighbours.endSafe();
           ++it_neighbour) { // safe iterator needed here
        Edge neigh(*it_neighbour, node);

        if (__T_mpd.separator(neigh).size() == 0)
          __T_mpd.eraseEdge(neigh);
      }
    }

    // remove X from the cliques containing X
    for (const auto clique : MPS_of_X) {
      const std::vector<NodeId> &cliques_of_X = __cliques_of_mps[clique];

      for (unsigned int i = 0; i < cliques_of_X.size(); ++i) {
        __junction_tree.eraseFromClique(cliques_of_X[i], X);

        // if the intersection between clique and one of its neighbour is empty,
        // remove the edge linking them only if, in addition, there is no
        // edge in __graph between a node of clique and a node in the neighbour
        auto &neighbours = __junction_tree.neighbours(cliques_of_X[i]);

        for (auto it_neighbour = neighbours.beginSafe();
             it_neighbour != neighbours.endSafe();
             ++it_neighbour) { // safe iterator needed here
          Edge neigh(*it_neighbour, cliques_of_X[i]);

          if (__junction_tree.separator(neigh).size() == 0) {
            // try to see if there is an edge between the nodes of one extremity
            // of *neighbour and those of the other extremity
            bool hasCommonEdge = false;

            for (const auto node1 : __junction_tree.clique(neigh.first()))
              for (const auto node2 : __junction_tree.clique(neigh.second()))
                if (__graph.existsEdge(node1, node2)) {
                  hasCommonEdge = true;
                  break;
                }

            if (!hasCommonEdge) {
              __junction_tree.eraseEdge(neigh);
            }
          }
        }
      }
    }

    // if the MPS containing X is empty, then remove it, as well as the
    // corresponding clique in the junction tree (which also only contains X)
    if ((MPS_of_X.size() == 1) && (__T_mpd.clique(MPS_of_X[0]).size() == 0)) {
      __junction_tree.eraseNode(__cliques_of_mps[MPS_of_X[0]][0]);
      __T_mpd.eraseNode(MPS_of_X[0]);
      __mps_of_clique.erase(__cliques_of_mps[MPS_of_X[0]][0]);
      __cliques_of_mps.erase(MPS_of_X[0]);
      __mps_affected.erase(MPS_of_X[0]);
    }

    __mps_of_node.erase(X);

    // update the elimination orders

    if (!__require_update) {
      for (unsigned int i = __reverse_elimination_order[X] + 1;
           i < __reverse_elimination_order.size(); ++i)
        __elimination_order[i - 1] = __elimination_order[i];

      __elimination_order.pop_back();

      __reverse_elimination_order.erase(X);

      __created_JT_cliques.erase(X);
    }

    // remove X completely from the graph
    __graph.eraseNode(X);

    _modalities.erase(X);
  }

  /// add a new edge to the graph

  int IncrementalTriangulation::__markAffectedMPSsByAddLink(const NodeId Mx,
                                                            const NodeId Mz,
                                                            const NodeId X,
                                                            const NodeId Y) {
    // check if Mx contains Y. In this case, mark Mx and return 1 to indicate that
    // Y has been found or 2 to indicate that Y has been found and that the nearest
    // MPS containing X has been marked
    const NodeSet &cliqueMX = __T_mpd.clique(Mx);

    if (cliqueMX.contains(Y)) {
      __mps_affected[Mx] = true;

      if (cliqueMX.contains(X))
        return 2;

      return 1;
    }

    // parse Mx's neighbours until we find Y
    for (const auto other_node : __T_mpd.neighbours(Mx))
      if (other_node != Mz) {
        int neighbourStatus = __markAffectedMPSsByAddLink(other_node, Mx, X, Y);

        if (neighbourStatus == 2)
          return 2;
        else if (neighbourStatus == 1) {
          __mps_affected[Mx] = true;

          if (cliqueMX.contains(X))
            return 2;

          return 1;
        }
      }

    // indicate that X was not found
    return 0;
  }

  /// adds a new edge to the graph (the join tree may need a triangulation update)
  void IncrementalTriangulation::insertEdge(const NodeId X, const NodeId Y) {
    addEdge(X, Y);
  }

  void IncrementalTriangulation::addEdge(const NodeId X, const NodeId Y) {
    // check that the edge exist
    if ((X == Y) || !__graph.existsNode(X) || !__graph.existsNode(Y) ||
        __graph.existsEdge(Edge(X, Y)))
      return;

    // add the edge to the graph
    __graph.addEdge(X, Y);

    // take any MPS containing X and search its tree to find Y
    NodeId &mps_X = __mps_of_node[X][0];

    int found = __markAffectedMPSsByAddLink(mps_X, mps_X, X, Y);

    if (found == 0) {
      // the mps containing X do not belong to the same tree as those containing Y
      NodeId &mps_Y = __mps_of_node[Y][0];

      // find a clique in mps_X containing X and another in mps_Y containing Y
      // and add a clique XY to the junction tree linked to the cliques found
      // in mps_X and mps_Y
      const std::vector<NodeId> &cliques_X = __cliques_of_mps[mps_X];
      const std::vector<NodeId> &cliques_Y = __cliques_of_mps[mps_Y];
      NodeId c_X = 0, c_Y = 0;

      for (unsigned int i = 0; i < cliques_X.size(); ++i) {
        if (__junction_tree.clique(cliques_X[i]).contains(X)) {
          c_X = cliques_X[i];
          break;
        }
      }

      for (unsigned int i = 0; i < cliques_Y.size(); ++i) {
        if (__junction_tree.clique(cliques_Y[i]).contains(Y)) {
          c_Y = cliques_Y[i];
          break;
        }
      }

      // link c_Y and c_X through a new node containing XY
      NodeSet nodes(2);

      nodes.insert(X);

      nodes.insert(Y);

      NodeId newNode = __junction_tree.addNode(nodes);

      __junction_tree.addEdge(newNode, c_X);
      __junction_tree.addEdge(newNode, c_Y);

      NodeId newMPS = __T_mpd.addNode(nodes);

      __T_mpd.addEdge(newMPS, mps_X);
      __T_mpd.addEdge(newMPS, mps_Y);

      // check that the maximal prime subgraph containing X is not X alone
      // in this case, remove this max prime subgraph, as well as the corresponding
      // clique in the junction tree
      if (__T_mpd.clique(mps_X).size() == 1) {
        __junction_tree.eraseNode(c_X);
        __T_mpd.eraseNode(mps_X);
        __mps_affected.erase(mps_X);
        __mps_of_clique.erase(c_X);
        __cliques_of_mps.erase(mps_X);
        __created_JT_cliques[X] = newNode;
        mps_X = newMPS;
      } else
        __mps_of_node[X].insert(newMPS);

      // do the same thing as above for node Y
      if (__T_mpd.clique(mps_Y).size() == 1) {
        __junction_tree.eraseNode(c_Y);
        __T_mpd.eraseNode(mps_Y);
        __mps_affected.erase(mps_Y);
        __mps_of_clique.erase(c_Y);
        __cliques_of_mps.erase(mps_Y);
        __created_JT_cliques[Y] = newNode;
        mps_Y = newMPS;
      } else
        __mps_of_node[Y].insert(newMPS);

      std::vector<NodeId> &cl =
          __cliques_of_mps.insert(newMPS, std::vector<NodeId>()).second;

      cl.push_back(newNode);

      __mps_of_clique.insert(newNode, newMPS);

      __mps_affected.insert(newMPS, false);
    } else {
      __require_update = true;
      __require_created_JT_cliques = true;
    }

    // in all cases, recompute the elimination ordering
    __require_elimination_order = true;
  }

  /// checks that the incremental triangulation works properly

  bool IncrementalTriangulation::__check() {
    // just in case, update everything
    updateTriangulation();

    bool OK = true;

    // check that all the nodes of the graph belong to the junction tree
    {
      NodeProperty<bool> nodesProp = __graph.nodesProperty<bool>(false);

      for (const auto cliq : __junction_tree.nodes())
        for (const auto node : __junction_tree.clique(cliq))
          nodesProp[node] = true;

      for (const auto &elt : nodesProp)
        if (!elt.second) {
          std::cerr << "check nodes" << std::endl << __graph << std::endl
                    << __junction_tree << std::endl;
          OK = false;
        }

      if (!OK)
        return false;
    }

    // check that the edgs belong to the junction tree
    {
      std::pair<NodeId, NodeId> thePair;
      EdgeProperty<bool> edgesProp = __graph.edgesProperty(false);

      for (const auto cliq : __junction_tree.nodes()) {
        const NodeSet &clique = __junction_tree.clique(cliq);

        for (auto iter2 = clique.begin(); iter2 != clique.end(); ++iter2) {
          auto iter3 = iter2;

          for (++iter3; iter3 != clique.end(); ++iter3) {
            thePair.first = std::min(*iter2, *iter3);
            thePair.second = std::max(*iter2, *iter3);

            if (__graph.existsEdge(thePair.first, thePair.second))
              edgesProp[Edge(thePair.first, thePair.second)] = true;
          }
        }
      }

      for (const auto &elt : edgesProp)
        if (!elt.second) {
          std::cerr << "check edges" << std::endl << __graph << std::endl
                    << __junction_tree << std::endl;
          OK = false;
        }

      if (!OK)
        return false;
    }

    // check that all the nodes of the graph belong to the MPS tree
    {
      NodeProperty<bool> nodesProp = __graph.nodesProperty<bool>(false);

      for (const auto cliq : __T_mpd.nodes())
        for (const auto node : __T_mpd.clique(cliq))
          nodesProp[node] = true;

      for (const auto &elt : nodesProp)
        if (!elt.second) {
          std::cerr << "check nodes" << std::endl << __graph << std::endl << __T_mpd
                    << std::endl;
          OK = false;
        }

      if (!OK)
        return false;
    }

    // check that the arcs of the graph belong to the MPS tree
    {
      std::pair<NodeId, NodeId> thePair;
      EdgeProperty<bool> edgesProp = __graph.edgesProperty(false);

      for (const auto cliq : __T_mpd.nodes()) {
        const NodeSet &clique = __T_mpd.clique(cliq);

        for (auto iter2 = clique.begin(); iter2 != clique.end(); ++iter2) {
          auto iter3 = iter2;

          for (++iter3; iter3 != clique.end(); ++iter3) {
            thePair.first = std::min(*iter2, *iter3);
            thePair.second = std::max(*iter2, *iter3);

            if (__graph.existsEdge(thePair.first, thePair.second))
              edgesProp[Edge(thePair.first, thePair.second)] = true;
          }
        }
      }

      for (const auto &elt : edgesProp)
        if (!elt.second) {
          std::cerr << "check edges" << std::endl << __graph << std::endl << __T_mpd
                    << std::endl;
          OK = false;
        }

      if (!OK)
        return false;
    }

    // check the MPS of node
    {
      NodeProperty<NodeProperty<bool>> chk;

      for (const auto node : __graph.nodes())
        chk.insert(node, HashTable<NodeId, bool>());

      for (const auto cliq : __T_mpd.nodes())
        for (auto node : __T_mpd.clique(cliq))
          chk[node].insert(cliq, false);

      for (const auto &elt : __mps_of_node) {
        HashTable<NodeId, bool> &hash = chk[elt.first];

        for (const auto cell : elt.second) {
          if (!hash.exists(cell)) {
            std::cerr << "check mps of nodes" << std::endl << __T_mpd << std::endl
                      << __mps_of_node << std::endl;
            OK = false;
          }

          hash[cell] = true;
        }
      }

      for (const auto &elt : chk)
        for (const auto &elt2 : elt.second)
          if (!elt2.second) {
            std::cerr << "check mps of nodes2" << std::endl << __T_mpd << std::endl
                      << __mps_of_node << std::endl;
            OK = false;
          }

      if (!OK)
        return false;
    }

    // check that the junction tree and the T_mpd are junction trees
    {
      if (!__junction_tree.isJoinTree()) {
        std::cerr << "check join tree __junction_tree" << std::endl
                  << __junction_tree << std::endl;
        return false;
      }

      if (!__T_mpd.isJoinTree()) {
        std::cerr << "check join tree  __T_mpd" << std::endl << __T_mpd << std::endl;
        return false;
      }
    }

    // check elimination sequences
    {
      eliminationOrder();

      if (__elimination_order.size() != __graph.size()) {
        std::cerr << "check elimination order" << std::endl << __elimination_order
                  << std::endl;
        return false;
      }

      NodeSet nodes;

      for (const auto node : __graph.nodes()) {
        if (nodes.exists(node)) {
          std::cerr << "check elimination order" << std::endl << __elimination_order
                    << std::endl;
          return false;
        } else
          nodes.insert(node);
      }

      if (nodes.size() != __graph.size()) {
        std::cerr << "check elimination order" << std::endl << __elimination_order
                  << std::endl;
        return false;
      }

      if (__reverse_elimination_order.size() != __graph.size()) {
        std::cerr << "check reverse elimination order" << std::endl
                  << __reverse_elimination_order << std::endl;
        return false;
      }

      for (const auto node : __graph.nodes()) {
        if (!__reverse_elimination_order.exists(node)) {
          std::cerr << "check reverse elimination order" << std::endl
                    << __reverse_elimination_order << std::endl;
          return false;
        }
      }
    }

    // check created junction tree cliques
    {
      createdJunctionTreeCliques();

      if (__created_JT_cliques.size() != __graph.size()) {
        std::cerr << "check creating JT cliques" << std::endl << __created_JT_cliques
                  << std::endl;
        return false;
      }

      for (const auto node : __graph.nodes()) {
        if (!__created_JT_cliques.exists(node) ||
            !__junction_tree.existsNode(__created_JT_cliques[node])) {
          std::cerr << "check created JT cliques" << std::endl
                    << __created_JT_cliques << std::endl;
          return false;
        }
      }
    }

    return true;
  }

  /// set up a connected subgraph to be triangulated

  void IncrementalTriangulation::__setUpConnectedTriangulation(
      NodeId Mx, NodeId Mfrom, UndiGraph &theGraph,
      std::vector<Edge> &notAffectedneighbourCliques,
      HashTable<NodeId, bool> &cliques_affected) {
    // mark the clique so that we won't try to update it several times
    cliques_affected[Mx] = false;

    // get the nodes that are concerned by the triangulation update
    for (const auto node : __junction_tree.clique(Mx))
      if (!theGraph.exists(node))
        theGraph.addNode(node);

    // go on with the neighbour cliques in the junction tree
    for (const auto othernode : __junction_tree.neighbours(Mx))
      if (othernode != Mfrom) {
        if (cliques_affected.exists(othernode)) {
          __setUpConnectedTriangulation(othernode, Mx, theGraph,
                                        notAffectedneighbourCliques,
                                        cliques_affected);
        } else {
          // indicate that we have a clique not affected that is adjacent
          // to an affected one
          notAffectedneighbourCliques.push_back(Edge(othernode, Mx));
        }
      }
  }

  /// update the junction tree

  void IncrementalTriangulation::__updateJunctionTree(
      NodeProperty<bool> &all_cliques_affected,
      NodeSet &new_nodes_in_junction_tree) {
    // a temporary subgraph in which we actually perform the triangulation
    UndiGraph tmp_graph;

    // for each triangulation, we will keep track of the cliques of the
    // junction tree that are not affected by the triangulation but that are
    // adjacent to cliques affected. This will enable us to connect easily the
    // newly created cliques with the old ones.
    std::vector<Edge> notAffectedneighbourCliques;

    // parse all the affected MPS and get the corresponding cliques

    for (const auto &elt : __mps_affected)
      if (elt.second) {
        // get the cliques contained in this MPS
        const std::vector<NodeId> &cliques = __cliques_of_mps[elt.first];

        for (unsigned int i = 0; i < cliques.size(); ++i)
          all_cliques_affected.insert(cliques[i], true);
      }

    // for each connected set of cliques involved in the triangulations
    // perform a new triangulation and update the max prime subgraph tree
    for (const auto &elt : all_cliques_affected) {
      if (elt.second) {
        // set up the connected subgraph that need be retriangulated and the
        // cliques that are affected by this triangulation
        tmp_graph.clear();
        notAffectedneighbourCliques.clear();
        __setUpConnectedTriangulation(elt.first, elt.first, tmp_graph,
                                      notAffectedneighbourCliques,
                                      all_cliques_affected);

        // insert the edges in tmp_graph
        for (auto edge : __graph.edges()) {
          try {
            tmp_graph.addEdge(edge.first(), edge.second());
          } catch (Exception &) {
          } // both extremities must be in tmp_graph
        }

        // remove from the mps_of_node table the affected mps containing the node
        // for ( UndiGraph::NodeIterator iter_node = tmp_graph.beginNodes();iter_node
        // != tmp_graph.endNodes(); ++iter_node ) {
        for (const auto node : tmp_graph.nodes()) {
          List<NodeId> &mps = __mps_of_node[node];

          for (HashTableConstIteratorSafe<NodeId, bool> iter_mps =
                   __mps_affected.beginSafe(); // safe iterator needed here
               iter_mps != __mps_affected.endSafe();
               ++iter_mps) {
            if (iter_mps.val())
              mps.eraseByVal(iter_mps.key());
          }
        }

        // now tmp_graph contains the graph that should be triangulated.
        // so triangulate it and get its junction tree
        __triangulation->setGraph(&tmp_graph, &_modalities);

        const CliqueGraph &tmp_junction_tree = __triangulation->junctionTree();

        // now, update the global junction tree
        // first add the nodes of tmp_junction_tree to __junction_tree
        // to do so, store the translations of the node ids of tmp_junction_tree
        // into the node ids of __junction_tree
        NodeProperty<NodeId> tmp2global_junction_tree(tmp_junction_tree.size());

        for (const auto cliq : tmp_junction_tree.nodes()) {
          // get new ids for the nodes of tmp_junction_tree. These should be
          // greater than or equal to __junction_tree.bound () so that we can
          // use the max_old_id defined at the beginning of the method.
          NodeId new_id = __junction_tree.addNode(tmp_junction_tree.clique(cliq));
          // translate the id of the temprary JT into an id of the global JT
          tmp2global_junction_tree.insert(cliq, new_id);
          new_nodes_in_junction_tree.insert(new_id);
        }

        // and add the edges of tmp_junction_tree to __junction_tree
        for (const auto edge : tmp_junction_tree.edges())
          __junction_tree.addEdge(tmp2global_junction_tree[edge.first()],
                                  tmp2global_junction_tree[edge.second()]);

        // second get the edges in __junction_tree that have an extremal clique R
        // in the affected clique set and the other one S not in the affected set
        // and see which new node V in the __junction_tree should be connected
        // to S. The running intersection property guarrantees that the clique in
        // the tmp_junction_tree that results from the elimination (during the
        // triangulation process) of the first eliminated node in the separator
        // between R and S is an admissible candidate
        for (unsigned int i = 0; i < notAffectedneighbourCliques.size(); ++i) {
          // check that the separator is not empty. If this is the case, do not
          // link the new junction tree to the old one
          const NodeSet &sep =
              __junction_tree.separator(notAffectedneighbourCliques[i]);

          if (sep.size() != 0) {
            // now find the first eliminated node in the separator
            unsigned int __elim_order = tmp_graph.bound() + 1;
            NodeId elim_node = 0;

            for (const auto id : sep) {
              unsigned int new_order = __triangulation->eliminationOrder(id);

              if (new_order < __elim_order) {
                __elim_order = new_order;
                elim_node = id;
              }
            }

            // find the __junction_tree clique corresponding to the elimination of
            // elim_node and insert an edge between this clique and that which was
            // not affected
            NodeId &to_connect =
                tmp2global_junction_tree[__triangulation->createdJunctionTreeClique(
                    elim_node)];

            NodeId not_affected =
                all_cliques_affected.exists(notAffectedneighbourCliques[i].first())
                    ? notAffectedneighbourCliques[i].second()
                    : notAffectedneighbourCliques[i].first();

            __junction_tree.addEdge(not_affected, to_connect);

            if (!new_nodes_in_junction_tree.contains(to_connect)) {
              __T_mpd.addEdge(__mps_of_clique[to_connect],
                              __mps_of_clique[not_affected]);
            }

            // check that the separator created by the new edge is not equal to
            // to_connect. If this is the case, then to_connect is included in
            // not_affected and, hence, should be removed from the graph
            if (__junction_tree.separator(not_affected, to_connect).size() ==
                __junction_tree.clique(to_connect).size()) {
              __junction_tree.eraseEdge(Edge(not_affected, to_connect));

              for (const auto neighbour : __junction_tree.neighbours(to_connect)) {
                __junction_tree.addEdge(neighbour, not_affected);

                if (!new_nodes_in_junction_tree.contains(neighbour))
                  __T_mpd.addEdge(__mps_of_clique[neighbour],
                                  __mps_of_clique[not_affected]);
              }

              __junction_tree.eraseNode(to_connect);

              to_connect = not_affected;
            }
          }
        }
      }
    }

    // remove the mps that were affected and update the cliques_of_mps table
    for (const auto &elt : all_cliques_affected) {
      __mps_of_clique.erase(elt.first);
      __junction_tree.eraseNode(elt.first);
    }

    for (const auto &elt : __mps_affected)
      if (elt.second) {
        __cliques_of_mps.erase(elt.first);
        __T_mpd.eraseNode(elt.first);
      }
  }

  /// used for computing the junction tree of the maximal prime subgraphs

  void IncrementalTriangulation::__computeMaxPrimeMergings(
      const NodeId node, const NodeId from,
      std::vector<std::pair<NodeId, NodeId>> &merged_cliques,
      HashTable<NodeId, bool> &mark,
      const NodeSet &new_nodes_in_junction_tree) const {
    mark[node] = true;

    // check the separators on all the adjacent edges of Mx
    for (const auto other_node : __junction_tree.neighbours(node))
      if (other_node != from) {
        const NodeSet &separator = __junction_tree.separator(Edge(other_node, node));

        // check that the separator between node and other_node is complete
        bool complete = true;

        for (auto iter_sep1 = separator.begin();
             iter_sep1 != separator.end() && complete; ++iter_sep1) {
          auto iter_sep2 = iter_sep1;

          for (++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2) {
            if (!__graph.existsEdge(*iter_sep1, *iter_sep2)) {
              complete = false;
              break;
            }
          }
        }

        // here complete indicates whether the separator is complete or not
        if (!complete)
          merged_cliques.push_back(std::pair<NodeId, NodeId>(other_node, node));

        if (new_nodes_in_junction_tree.contains(other_node))
          __computeMaxPrimeMergings(other_node, node, merged_cliques, mark,
                                    new_nodes_in_junction_tree);
      }
  }

  /// update the max prime subgraph

  void IncrementalTriangulation::__updateMaxPrimeSubgraph(
      NodeProperty<bool> &all_cliques_affected,
      const NodeSet &new_nodes_in_junction_tree) {
    // the maximal prime subgraph join tree is created by aggregation of some
    // cliques. More precisely, when the separator between 2 cliques is not
    // complete in the original graph, then the two cliques must be merged.

    // Create a hashtable indicating which clique has been absorbed by some other
    // clique. Keys are the cliques absorbed, and values are the cliques that
    // absorb them.
    HashTable<NodeId, NodeId> T_mpd_cliques(all_cliques_affected.size());

    for (const auto clik : __junction_tree.nodes())
      if (new_nodes_in_junction_tree.contains(clik))
        T_mpd_cliques.insert(clik, clik);

    // parse all the separators of the junction tree and test those that are not
    // complete in the original graph
    std::vector<std::pair<NodeId, NodeId>> merged_cliques;

    HashTable<NodeId, bool> mark = T_mpd_cliques.map(false);

    for (const auto &elt : mark)
      if (!elt.second)
        __computeMaxPrimeMergings(elt.first, elt.first, merged_cliques, mark,
                                  new_nodes_in_junction_tree);

    // compute the transitive closure of merged_cliques. This one will contain
    // pairs (X,Y) indicating that clique X must be merged with clique Y.
    // Actually clique X will be inserted into clique Y.
    for (unsigned int i = 0; i < merged_cliques.size(); ++i) {
      if (T_mpd_cliques.exists(merged_cliques[i].second))
        T_mpd_cliques[merged_cliques[i].first] =
            T_mpd_cliques[merged_cliques[i].second];
      else
        T_mpd_cliques[merged_cliques[i].first] =
            __mps_of_clique[merged_cliques[i].second];
    }

    // now we can create the max prime junction tree.

    // create a map translating the cliques' ids in the junction tree into
    // cliques' id in the _T_mpd tree
    NodeProperty<NodeId> clique2MPS(T_mpd_cliques.size());

    // First, create the new cliques and create the corresponding
    // cliques_of_mps entries
    for (const auto &elt : T_mpd_cliques)
      if (elt.first == elt.second) {
        NodeId newId = __T_mpd.addNode(__junction_tree.clique(elt.second));
        clique2MPS.insert(elt.second, newId);
        std::vector<NodeId> &vect_of_cliques =
            __cliques_of_mps.insert(newId, std::vector<NodeId>()).second;
        vect_of_cliques.push_back(elt.second);
      }

    // add to the cliques previously created the nodes of the cliques that were
    // merged into them and update the cliques_of_mps
    for (const auto &elt : T_mpd_cliques)
      if ((elt.first != elt.second) &&
          (new_nodes_in_junction_tree.contains(elt.second))) {
        const NodeId idMPS = clique2MPS[elt.second];

        for (const auto node : __junction_tree.clique(elt.first)) {
          try {
            __T_mpd.addToClique(idMPS, node);
          } catch (DuplicateElement &) {
          }
        }

        __cliques_of_mps[idMPS].push_back(elt.first);
      }

    // update the mps_of_node and the mps_of_clique
    for (const auto &elt : T_mpd_cliques) {
      const NodeId idMPS = clique2MPS[elt.second];
      __mps_of_clique.insert(elt.first, idMPS);

      if (elt.first == elt.second)
        for (const auto node : __T_mpd.clique(idMPS))
          __mps_of_node[node].insert(idMPS);
    }

    // add the edges to the max prime subgraph tree
    for (const auto &elt : T_mpd_cliques) {
      NodeId clique = clique2MPS[elt.second];

      for (const auto othernode : __junction_tree.neighbours(elt.first))
        if (T_mpd_cliques.exists(othernode)) {
          // here iter is linked to another node that has been created during
          // the triangulation
          NodeId otherClique = clique2MPS[T_mpd_cliques[othernode]];

          // avoid adding the same edge several times
          if (clique > otherClique) {
            __T_mpd.addEdge(clique, otherClique);
          }
        } else {
          __T_mpd.addEdge(clique, __mps_of_clique[othernode]);
        }
    }
  }

  /// updates the triangulated graph using the modif list

  void IncrementalTriangulation::updateTriangulation() {
    if (!__require_update)
      return;

    // the set of all the cliques that should be affected by the different
    // triangulations we will perform (one by connected component)
    NodeProperty<bool> all_cliques_affected(__junction_tree.size());

    // we need to keep track of the new node ids that will be inserted
    // into __junction_tree. A priori, these should be equal to the ids
    // inserted into tmp2global_junction_tree. But, sometimes, some new nodes
    // are included into old nodes and, in this case, the translation in
    // tmp2global_junction_tree indicates the the new node inserted corresponds
    // to an old node. Here we wish to know this additional information
    NodeSet new_nodes_in_junction_tree;

    __updateJunctionTree(all_cliques_affected, new_nodes_in_junction_tree);

    // now update the T_mpd so that it be coherent with the junction tree
    __updateMaxPrimeSubgraph(all_cliques_affected, new_nodes_in_junction_tree);

    // reset the MPS that are affected
    __mps_affected.clear();

    for (const auto node : __T_mpd.nodes())
      __mps_affected.insert(node, false);

    // remove all the structures used by the triangulation algorithm
    __triangulation->clear();

    __require_update = false;
  }

  /// sets the graph to the empty graph

  void IncrementalTriangulation::clear() {
    __graph.clear();
    _modalities.clear();
    __junction_tree.clear();
    __T_mpd.clear();
    __mps_of_node.clear();
    __cliques_of_mps.clear();
    __mps_of_clique.clear();
    __mps_affected.clear();
    __triangulation->clear();
    __require_update = false;
    __require_elimination_order = false;
    __elimination_order.clear();
    __reverse_elimination_order.clear();
    __require_created_JT_cliques = false;
    __created_JT_cliques.clear();
  }

  /// a collect algorithm to compute, for each node, one container JT's clique

  void IncrementalTriangulation::__collectJTCliques(const NodeId clique,
                                                    const NodeId from,
                                                    NodeProperty<bool> &examined) {
    // apply collect to all the neighbours except from
    for (const auto otherclique : __junction_tree.neighbours(clique))
      if (otherclique != from)
        __collectJTCliques(otherclique, clique, examined);

    // get the nodes that belong to clique and not to from
    examined[clique] = true;

    const NodeSet &cliquenodes = __junction_tree.clique(clique);

    if (from != clique) {
      const NodeSet &separator = __junction_tree.separator(clique, from);

      for (const auto cli : cliquenodes)
        if (!separator.contains(cli))
          __created_JT_cliques.insert(cli, clique);
    } else {
      for (const auto cli : cliquenodes)
        __created_JT_cliques.insert(cli, clique);
    }
  }

  /** @brief returns the Ids of the cliques of the junction tree created by the
   * elimination of the nodes */

  const NodeProperty<NodeId> &
  IncrementalTriangulation::createdJunctionTreeCliques() {
    // check if we already computed the containing cliques
    if (!__require_created_JT_cliques)
      return __created_JT_cliques;

    // we first we compute the junction tree
    updateTriangulation();

    __created_JT_cliques.clear();

    __require_created_JT_cliques = false;

    if (__junction_tree.size() == 0) {
      return __created_JT_cliques;
    }

    // now we can use a collect algorithm to get the containing cliques
    NodeProperty<bool> examined = __junction_tree.nodesProperty<bool>(false);

    for (const auto &elt : examined)
      if (!elt.second)
        __collectJTCliques(elt.first, elt.first, examined);

    return __created_JT_cliques;
  }

  /// returns a clique containing a given node of the triangulated graph

  NodeId IncrementalTriangulation::createdJunctionTreeClique(NodeId id) {
    createdJunctionTreeCliques();
    return __created_JT_cliques[id];
  }

  /** @brief returns the Id of the maximal prime subgraph created by the
   * elimination of a given node during the triangulation process */

  NodeId IncrementalTriangulation::createdMaxPrimeSubgraph(const NodeId id) {
    // get the created junction tree clique and get its MPS
    return __mps_of_clique[createdJunctionTreeClique(id)];
  }

  /// changes the current graph

  void IncrementalTriangulation::setGraph(const UndiGraph &theGraph,
                                          const NodeProperty<Size> &modal) {
    // remove the current graph
    clear();

    // copy the graph passed in arent and update the structures
    // containing the informations useful for the triangulation

    for (const auto node : theGraph.nodes())
      addNode(node, modal[node]);

    for (const auto edge : theGraph.edges())
      addEdge(edge.first(), edge.second());
  }

  /// a collect algorithm to compute elimination orderings

  void IncrementalTriangulation::__collectEliminationOrder(
      const NodeId node, const NodeId from, NodeProperty<bool> &examined,
      unsigned int &index) {
    // apply collect to all the neighbours except from
    for (const auto othernode : __junction_tree.neighbours(node))
      if (othernode != from)
        __collectEliminationOrder(othernode, node, examined, index);

    // get the nodes that belong to node and not to from
    examined[node] = true;

    const NodeSet &clique = __junction_tree.clique(node);

    if (from != node) {
      const NodeSet &separator = __junction_tree.separator(node, from);

      for (const auto cli : clique) {
        if (!separator.contains(cli)) {
          __elimination_order[index] = cli;
          __reverse_elimination_order.insert(cli, index);
          ++index;
        }
      }
    } else {
      for (const auto cli : clique) {
        __elimination_order[index] = cli;
        __reverse_elimination_order.insert(cli, index);
        ++index;
      }
    }
  }

  /// get a possible elimination ordering

  const std::vector<NodeId> &IncrementalTriangulation::eliminationOrder() {
    // check if we already computed the elimination order
    if (!__require_elimination_order)
      return __elimination_order;

    // to compute the elimination order, we first we compute the junction tree
    updateTriangulation();

    __elimination_order.resize(__graph.size());

    __reverse_elimination_order.clear();

    __require_elimination_order = false;

    if (__junction_tree.size() == 0) {
      return __elimination_order;
    }

    // now we can use a collect algorithm to get the elimination order
    unsigned int index = 0;

    NodeProperty<bool> examined = __junction_tree.nodesProperty<bool>(false);

    for (const auto &elt : examined)
      if (!elt.second)
        __collectEliminationOrder(elt.first, elt.first, examined, index);

    return __elimination_order;
  }

  /** @brief returns the number of a given node in the elimination order
   * (0 = first node eliminated) */

  unsigned int IncrementalTriangulation::eliminationOrder(const NodeId node) {
    if (!__graph.existsNode(node)) {
      GUM_ERROR(NotFound, "the node " << node << " does not exist");
    }

    // compute the elimination order
    eliminationOrder();

    return __reverse_elimination_order[node];
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
