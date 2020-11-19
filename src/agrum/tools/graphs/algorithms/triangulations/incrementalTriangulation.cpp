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
 * @brief source implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <limits>
#include <utility>

#include <agrum/agrum.h>
#include <agrum/tools/core/math/math.h>

#include <agrum/tools/core/list.h>
#include <agrum/tools/graphs/algorithms/triangulations/incrementalTriangulation.h>
#include <agrum/tools/graphs/undiGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/triangulations/incrementalTriangulation_inl.h>
#endif   // GUM_NO_INLINE

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// constructor
  IncrementalTriangulation::IncrementalTriangulation(
     const UnconstrainedTriangulation& triang_algo,
     const UndiGraph*                  theGraph,
     const NodeProperty< Size >*       domsizes) :
      Triangulation(domsizes),
      triangulation__(triang_algo.newFactory()) {
    // for debugging purposes
    GUM_CONSTRUCTOR(IncrementalTriangulation);

    // reset the triangulation algorithm => it starts with an empty graph
    triangulation__->clear();

    // copy the graph passed in argument and update the structures
    // containing the informations useful for the triangulation

    for (const auto node: *theGraph)
      addNode(node, (*domsizes)[node]);

    // insert all the edges of the graph into the structure. This will
    // implicitly update the "require_update" field
    for (const auto& edge: theGraph->edges())
      addEdge(edge.first(), edge.second());
  }

  /// default constructor
  IncrementalTriangulation::IncrementalTriangulation(
     const UnconstrainedTriangulation& triang_algo) :
      triangulation__(triang_algo.newFactory()) {
    // for debugging purposes
    GUM_CONSTRUCTOR(IncrementalTriangulation);

    // reset the triangulation algorithm => it starts with an empty graph
    triangulation__->clear();
  }

  /// copy operator
  IncrementalTriangulation::IncrementalTriangulation(
     const IncrementalTriangulation& from) :
      Triangulation(from),
      graph__(from.graph__), junction_tree__(from.junction_tree__),
      T_mpd__(from.T_mpd__), mps_of_node__(from.mps_of_node__),
      cliques_of_mps__(from.cliques_of_mps__),
      mps_of_clique__(from.mps_of_clique__), mps_affected__(from.mps_affected__),
      triangulation__(from.triangulation__->newFactory()),
      require_update__(from.require_update__),
      require_elimination_order__(from.require_elimination_order__),
      elimination_order__(from.elimination_order__),
      reverse_elimination_order__(from.reverse_elimination_order__),
      require_created_JT_cliques__(from.require_created_JT_cliques__),
      created_JT_cliques__(from.created_JT_cliques__) {
    // for debugging purposes
    GUM_CONS_CPY(IncrementalTriangulation);

    domain_sizes__ = from.domain_sizes__;
  }

  /// destructor
  IncrementalTriangulation::~IncrementalTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(IncrementalTriangulation);

    // remove things that were allocated within the current class
    delete triangulation__;
  }

  /// virtual clone constructor
  IncrementalTriangulation* IncrementalTriangulation::newFactory() const {
    return new IncrementalTriangulation(*triangulation__);
  }

  /// virtual copy constructor
  IncrementalTriangulation* IncrementalTriangulation::copyFactory() const {
    return new IncrementalTriangulation(*this);
  }

  /// copy operator
  IncrementalTriangulation&
     IncrementalTriangulation::operator=(const IncrementalTriangulation& from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(IncrementalTriangulation);

      // copy all the structures stored in "from"
      graph__ = from.graph__;
      domain_sizes__ = from.domain_sizes__;
      junction_tree__ = from.junction_tree__;
      T_mpd__ = from.T_mpd__;
      mps_of_node__ = from.mps_of_node__;
      cliques_of_mps__ = from.cliques_of_mps__;
      mps_of_clique__ = from.mps_of_clique__;
      mps_affected__ = from.mps_affected__;
      require_update__ = from.require_update__;
      require_elimination_order__ = from.require_elimination_order__;
      elimination_order__ = from.elimination_order__;
      reverse_elimination_order__ = from.reverse_elimination_order__;
      require_created_JT_cliques__ = from.require_created_JT_cliques__;
      created_JT_cliques__ = from.created_JT_cliques__;

      // just in case we changed the triangulation algorithm, we remove it
      // and create it again
      delete triangulation__;
      triangulation__ = from.triangulation__->newFactory();
    }

    return *this;
  }

  /// adds a new node to the graph (and update the internal structures)
  void IncrementalTriangulation::addNode(const NodeId node, Size modal) {
    // check if the node already exists
    if (graph__.existsNode(node)) return;

    // add the new node to the graph
    graph__.addNodeWithId(node);
    domain_sizes__.insert(node, modal);

    // add a new clique to T_mpd and the junction tree
    NodeSet clique_nodes(2);
    clique_nodes.insert(node);

    NodeId MPS = T_mpd__.addNode(clique_nodes);
    NodeId new_clique = junction_tree__.addNode(clique_nodes);

    // indicate in which MPS node belongs
    List< NodeId >& list_of_mps =
       mps_of_node__.insert(node, List< NodeId >()).second;
    list_of_mps.insert(MPS);

    // indicate in which MPS the clique added to the junction tree belongs
    std::vector< NodeId >& cliquesMPS =
       cliques_of_mps__.insert(MPS, std::vector< NodeId >()).second;

    cliquesMPS.push_back(new_clique);
    mps_of_clique__.insert(new_clique, MPS);

    // indicate that the new MPS should not be affected by a triangulation
    mps_affected__.insert(MPS, false);

    // insert the node into the elimination order sequence
    elimination_order__.push_back(node);

    if (!reverse_elimination_order__.exists(node))
      reverse_elimination_order__.insert(node, Size(elimination_order__.size()));

    if (!created_JT_cliques__.exists(node))
      created_JT_cliques__.insert(node, new_clique);
  }

  /// mark the mps affected by the deletion of a given edge

  void IncrementalTriangulation::markAffectedMPSsByRemoveLink__(const NodeId My,
                                                                const NodeId Mz,
                                                                const Edge& edge) {
    // mark the MPS so that it will be retriangulated
    mps_affected__[My] = true;

    // mark all the neighbour MPS that contain edge
    for (const auto nei: T_mpd__.neighbours(My))
      if (nei != Mz) {
        const NodeSet& Syk = T_mpd__.separator(Edge(nei, My));

        if (Syk.contains(edge.first()) && Syk.contains(edge.second()))
          markAffectedMPSsByRemoveLink__(nei, My, edge);
      }
  }

  /// removes an edge from the graph (the join tree may need a retriangulation)

  void IncrementalTriangulation::eraseEdge(const Edge& edge) {
    // check that the edge exist
    if (!graph__.existsEdge(edge)) return;

    // find a MPS containing the edge (X,Y)
    const NodeId X = edge.first();
    const NodeId Y = edge.second();

    const List< NodeId >& mps1 = mps_of_node__[X];
    const List< NodeId >& mps2 = mps_of_node__[Y];

    NodeId Mx = mps1[0];

    if (mps1.size() <= mps2.size()) {
      for (const auto node: mps1)
        if (T_mpd__.clique(node).contains(Y)) {
          Mx = node;
          break;
        }
    } else {
      for (const auto node: mps2)
        if (T_mpd__.clique(node).contains(X)) {
          Mx = node;
          break;
        }
    }

    // mark the MPS that need be updated
    markAffectedMPSsByRemoveLink__(Mx, Mx, edge);

    require_update__ = true;

    require_elimination_order__ = true;

    require_created_JT_cliques__ = true;

    // remove the edge (X,Y) from the graph
    graph__.eraseEdge(edge);
  }

  /// removes a node from the graph (the join tree may need a triangulation
  /// update)

  void IncrementalTriangulation::eraseNode(const NodeId X) {
    // check if the node exists
    if (!graph__.existsNode(X)) return;

    // remove all the edges adjacent to the node
    {
      const NodeSet& neighbours = graph__.neighbours(X);

      for (auto neighbour_edge =
              neighbours.beginSafe();   // safe iterator needed here
           neighbour_edge != neighbours.endSafe();
           ++neighbour_edge) {
        eraseEdge(Edge(*neighbour_edge, X));
      }
    }

    auto& MPS_of_X = mps_of_node__[X];

    // remove X from the MPS containing X
    for (const auto node: MPS_of_X) {
      T_mpd__.eraseFromClique(node, X);

      // if the intersection between *iter and one of its neighbour is empty,
      // remove the edge linking them
      auto& neighbours = T_mpd__.neighbours(node);

      for (auto it_neighbour = neighbours.beginSafe();
           it_neighbour != neighbours.endSafe();
           ++it_neighbour) {   // safe iterator needed here
        Edge neigh(*it_neighbour, node);

        if (T_mpd__.separator(neigh).size() == 0) T_mpd__.eraseEdge(neigh);
      }
    }

    // remove X from the cliques containing X
    for (const auto clique: MPS_of_X) {
      const std::vector< NodeId >& cliques_of_X = cliques_of_mps__[clique];

      for (unsigned int i = 0; i < cliques_of_X.size(); ++i) {
        junction_tree__.eraseFromClique(cliques_of_X[i], X);

        // if the intersection between clique and one of its neighbour is empty,
        // remove the edge linking them only if, in addition, there is no
        // edge in graph__ between a node of clique and a node in the neighbour
        auto& neighbours = junction_tree__.neighbours(cliques_of_X[i]);

        for (auto it_neighbour = neighbours.beginSafe();
             it_neighbour != neighbours.endSafe();
             ++it_neighbour) {   // safe iterator needed here
          Edge neigh(*it_neighbour, cliques_of_X[i]);

          if (junction_tree__.separator(neigh).size() == 0) {
            // try to see if there is an edge between the nodes of one extremity
            // of *neighbour and those of the other extremity
            bool hasCommonEdge = false;

            for (const auto node1: junction_tree__.clique(neigh.first()))
              for (const auto node2: junction_tree__.clique(neigh.second()))
                if (graph__.existsEdge(node1, node2)) {
                  hasCommonEdge = true;
                  break;
                }

            if (!hasCommonEdge) { junction_tree__.eraseEdge(neigh); }
          }
        }
      }
    }

    // if the MPS containing X is empty, then remove it, as well as the
    // corresponding clique in the junction tree (which also only contains X)
    if ((MPS_of_X.size() == 1) && (T_mpd__.clique(MPS_of_X[0]).size() == 0)) {
      junction_tree__.eraseNode(cliques_of_mps__[MPS_of_X[0]][0]);
      T_mpd__.eraseNode(MPS_of_X[0]);
      mps_of_clique__.erase(cliques_of_mps__[MPS_of_X[0]][0]);
      cliques_of_mps__.erase(MPS_of_X[0]);
      mps_affected__.erase(MPS_of_X[0]);
    }

    mps_of_node__.erase(X);

    // update the elimination orders

    if (!require_update__) {
      for (Idx i = reverse_elimination_order__[X] + 1;
           i < reverse_elimination_order__.size();
           ++i)
        elimination_order__[i - 1] = elimination_order__[i];

      elimination_order__.pop_back();

      reverse_elimination_order__.erase(X);

      created_JT_cliques__.erase(X);
    }

    // remove X completely from the graph
    graph__.eraseNode(X);

    domain_sizes__.erase(X);
  }

  /// add a new edge to the graph

  int IncrementalTriangulation::markAffectedMPSsByAddLink__(const NodeId Mx,
                                                            const NodeId Mz,
                                                            const NodeId X,
                                                            const NodeId Y) {
    // check if Mx contains Y. In this case, mark Mx and return 1 to indicate
    // that
    // Y has been found or 2 to indicate that Y has been found and that the
    // nearest
    // MPS containing X has been marked
    const NodeSet& cliqueMX = T_mpd__.clique(Mx);

    if (cliqueMX.contains(Y)) {
      mps_affected__[Mx] = true;

      if (cliqueMX.contains(X)) return 2;

      return 1;
    }

    // parse Mx's neighbours until we find Y
    for (const auto other_node: T_mpd__.neighbours(Mx))
      if (other_node != Mz) {
        int neighbourStatus = markAffectedMPSsByAddLink__(other_node, Mx, X, Y);

        if (neighbourStatus == 2)
          return 2;
        else if (neighbourStatus == 1) {
          mps_affected__[Mx] = true;

          if (cliqueMX.contains(X)) return 2;

          return 1;
        }
      }

    // indicate that X was not found
    return 0;
  }

  /// adds a new edge to the graph (the join tree may need a triangulation
  /// update)
  void IncrementalTriangulation::addEdge(const NodeId X, const NodeId Y) {
    // check that the edge exist
    if ((X == Y) || !graph__.existsNode(X) || !graph__.existsNode(Y)
        || graph__.existsEdge(Edge(X, Y)))
      return;

    // add the edge to the graph
    graph__.addEdge(X, Y);

    // take any MPS containing X and search its tree to find Y
    NodeId& mps_X = mps_of_node__[X][0];

    int found = markAffectedMPSsByAddLink__(mps_X, mps_X, X, Y);

    if (found == 0) {
      // the mps containing X do not belong to the same tree as those containing
      // Y
      NodeId& mps_Y = mps_of_node__[Y][0];

      // find a clique in mps_X containing X and another in mps_Y containing Y
      // and add a clique XY to the junction tree linked to the cliques found
      // in mps_X and mps_Y
      const std::vector< NodeId >& cliques_X = cliques_of_mps__[mps_X];
      const std::vector< NodeId >& cliques_Y = cliques_of_mps__[mps_Y];
      NodeId                       c_X = 0, c_Y = 0;

      for (unsigned int i = 0; i < cliques_X.size(); ++i) {
        if (junction_tree__.clique(cliques_X[i]).contains(X)) {
          c_X = cliques_X[i];
          break;
        }
      }

      for (unsigned int i = 0; i < cliques_Y.size(); ++i) {
        if (junction_tree__.clique(cliques_Y[i]).contains(Y)) {
          c_Y = cliques_Y[i];
          break;
        }
      }

      // link c_Y and c_X through a new node containing XY
      NodeSet nodes(2);

      nodes.insert(X);

      nodes.insert(Y);

      NodeId newNode = junction_tree__.addNode(nodes);

      junction_tree__.addEdge(newNode, c_X);
      junction_tree__.addEdge(newNode, c_Y);

      NodeId newMPS = T_mpd__.addNode(nodes);

      T_mpd__.addEdge(newMPS, mps_X);
      T_mpd__.addEdge(newMPS, mps_Y);

      // check that the maximal prime subgraph containing X is not X alone
      // in this case, remove this max prime subgraph, as well as the
      // corresponding
      // clique in the junction tree
      if (T_mpd__.clique(mps_X).size() == 1) {
        junction_tree__.eraseNode(c_X);
        T_mpd__.eraseNode(mps_X);
        mps_affected__.erase(mps_X);
        mps_of_clique__.erase(c_X);
        cliques_of_mps__.erase(mps_X);
        created_JT_cliques__[X] = newNode;
        mps_X = newMPS;
      } else
        mps_of_node__[X].insert(newMPS);

      // do the same thing as above for node Y
      if (T_mpd__.clique(mps_Y).size() == 1) {
        junction_tree__.eraseNode(c_Y);
        T_mpd__.eraseNode(mps_Y);
        mps_affected__.erase(mps_Y);
        mps_of_clique__.erase(c_Y);
        cliques_of_mps__.erase(mps_Y);
        created_JT_cliques__[Y] = newNode;
        mps_Y = newMPS;
      } else
        mps_of_node__[Y].insert(newMPS);

      std::vector< NodeId >& cl =
         cliques_of_mps__.insert(newMPS, std::vector< NodeId >()).second;

      cl.push_back(newNode);

      mps_of_clique__.insert(newNode, newMPS);

      mps_affected__.insert(newMPS, false);
    } else {
      require_update__ = true;
      require_created_JT_cliques__ = true;
    }

    // in all cases, recompute the elimination ordering
    require_elimination_order__ = true;
  }

  /// checks that the incremental triangulation works properly

  bool IncrementalTriangulation::check__() {
    // just in case, update everything
    updateTriangulation();

    bool OK = true;

    // check that all the nodes of the graph belong to the junction tree
    {
      NodeProperty< bool > nodesProp = graph__.nodesProperty< bool >(false);

      for (const auto cliq: junction_tree__.nodes())
        for (const auto node: junction_tree__.clique(cliq))
          nodesProp[node] = true;

      for (const auto& elt: nodesProp)
        if (!elt.second) {
          std::cerr << "check nodes" << std::endl
                    << graph__ << std::endl
                    << junction_tree__ << std::endl;
          OK = false;
        }

      if (!OK) return false;
    }

    // check that the edgs belong to the junction tree
    {
      std::pair< NodeId, NodeId > thePair;
      EdgeProperty< bool >        edgesProp = graph__.edgesProperty(false);

      for (const auto cliq: junction_tree__.nodes()) {
        const NodeSet& clique = junction_tree__.clique(cliq);

        for (auto iter2 = clique.begin(); iter2 != clique.end(); ++iter2) {
          auto iter3 = iter2;

          for (++iter3; iter3 != clique.end(); ++iter3) {
            thePair.first = std::min(*iter2, *iter3);
            thePair.second = std::max(*iter2, *iter3);

            if (graph__.existsEdge(thePair.first, thePair.second))
              edgesProp[Edge(thePair.first, thePair.second)] = true;
          }
        }
      }

      for (const auto& elt: edgesProp)
        if (!elt.second) {
          std::cerr << "check edges" << std::endl
                    << graph__ << std::endl
                    << junction_tree__ << std::endl;
          OK = false;
        }

      if (!OK) return false;
    }

    // check that all the nodes of the graph belong to the MPS tree
    {
      NodeProperty< bool > nodesProp = graph__.nodesProperty< bool >(false);

      for (const auto cliq: T_mpd__.nodes())
        for (const auto node: T_mpd__.clique(cliq))
          nodesProp[node] = true;

      for (const auto& elt: nodesProp)
        if (!elt.second) {
          std::cerr << "check nodes" << std::endl
                    << graph__ << std::endl
                    << T_mpd__ << std::endl;
          OK = false;
        }

      if (!OK) return false;
    }

    // check that the arcs of the graph belong to the MPS tree
    {
      std::pair< NodeId, NodeId > thePair;
      EdgeProperty< bool >        edgesProp = graph__.edgesProperty(false);

      for (const auto cliq: T_mpd__.nodes()) {
        const NodeSet& clique = T_mpd__.clique(cliq);

        for (auto iter2 = clique.begin(); iter2 != clique.end(); ++iter2) {
          auto iter3 = iter2;

          for (++iter3; iter3 != clique.end(); ++iter3) {
            thePair.first = std::min(*iter2, *iter3);
            thePair.second = std::max(*iter2, *iter3);

            if (graph__.existsEdge(thePair.first, thePair.second))
              edgesProp[Edge(thePair.first, thePair.second)] = true;
          }
        }
      }

      for (const auto& elt: edgesProp)
        if (!elt.second) {
          std::cerr << "check edges" << std::endl
                    << graph__ << std::endl
                    << T_mpd__ << std::endl;
          OK = false;
        }

      if (!OK) return false;
    }

    // check the MPS of node
    {
      NodeProperty< NodeProperty< bool > > chk;

      for (const auto node: graph__.nodes())
        chk.insert(node, HashTable< NodeId, bool >());

      for (const auto cliq: T_mpd__.nodes())
        for (auto node: T_mpd__.clique(cliq))
          chk[node].insert(cliq, false);

      for (const auto& elt: mps_of_node__) {
        HashTable< NodeId, bool >& hash = chk[elt.first];

        for (const auto cell: elt.second) {
          if (!hash.exists(cell)) {
            std::cerr << "check mps of nodes" << std::endl
                      << T_mpd__ << std::endl
                      << mps_of_node__ << std::endl;
            OK = false;
          }

          hash[cell] = true;
        }
      }

      for (const auto& elt: chk)
        for (const auto& elt2: elt.second)
          if (!elt2.second) {
            std::cerr << "check mps of nodes2" << std::endl
                      << T_mpd__ << std::endl
                      << mps_of_node__ << std::endl;
            OK = false;
          }

      if (!OK) return false;
    }

    // check that the junction tree and the T_mpd are junction trees
    {
      if (!junction_tree__.isJoinTree()) {
        std::cerr << "check join tree junction_tree__" << std::endl
                  << junction_tree__ << std::endl;
        return false;
      }

      if (!T_mpd__.isJoinTree()) {
        std::cerr << "check join tree  T_mpd__" << std::endl
                  << T_mpd__ << std::endl;
        return false;
      }
    }

    // check elimination sequences
    {
      eliminationOrder();

      if (elimination_order__.size() != graph__.size()) {
        std::cerr << "check elimination order" << std::endl
                  << elimination_order__ << std::endl;
        return false;
      }

      NodeSet nodes;

      for (const auto node: graph__.nodes()) {
        if (nodes.exists(node)) {
          std::cerr << "check elimination order" << std::endl
                    << elimination_order__ << std::endl;
          return false;
        } else
          nodes.insert(node);
      }

      if (nodes.size() != graph__.size()) {
        std::cerr << "check elimination order" << std::endl
                  << elimination_order__ << std::endl;
        return false;
      }

      if (reverse_elimination_order__.size() != graph__.size()) {
        std::cerr << "check reverse elimination order" << std::endl
                  << reverse_elimination_order__ << std::endl;
        return false;
      }

      for (const auto node: graph__.nodes()) {
        if (!reverse_elimination_order__.exists(node)) {
          std::cerr << "check reverse elimination order" << std::endl
                    << reverse_elimination_order__ << std::endl;
          return false;
        }
      }
    }

    // check created junction tree cliques
    {
      createdJunctionTreeCliques();

      if (created_JT_cliques__.size() != graph__.size()) {
        std::cerr << "check creating JT cliques" << std::endl
                  << created_JT_cliques__ << std::endl;
        return false;
      }

      for (const auto node: graph__.nodes()) {
        if (!created_JT_cliques__.exists(node)
            || !junction_tree__.existsNode(created_JT_cliques__[node])) {
          std::cerr << "check created JT cliques" << std::endl
                    << created_JT_cliques__ << std::endl;
          return false;
        }
      }
    }

    return true;
  }

  /// set up a connected subgraph to be triangulated

  void IncrementalTriangulation::setUpConnectedTriangulation__(
     NodeId                     Mx,
     NodeId                     Mfrom,
     UndiGraph&                 theGraph,
     std::vector< Edge >&       notAffectedneighbourCliques,
     HashTable< NodeId, bool >& cliques_affected) {
    // mark the clique so that we won't try to update it several times
    cliques_affected[Mx] = false;

    // get the nodes that are concerned by the triangulation update
    for (const auto node: junction_tree__.clique(Mx))
      if (!theGraph.exists(node)) theGraph.addNodeWithId(node);

    // go on with the neighbour cliques in the junction tree
    for (const auto othernode: junction_tree__.neighbours(Mx))
      if (othernode != Mfrom) {
        if (cliques_affected.exists(othernode)) {
          setUpConnectedTriangulation__(othernode,
                                        Mx,
                                        theGraph,
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

  void IncrementalTriangulation::updateJunctionTree__(
     NodeProperty< bool >& all_cliques_affected,
     NodeSet&              new_nodes_in_junction_tree) {
    // a temporary subgraph in which we actually perform the triangulation
    UndiGraph tmp_graph;

    // for each triangulation, we will keep track of the cliques of the
    // junction tree that are not affected by the triangulation but that are
    // adjacent to cliques affected. This will enable us to connect easily the
    // newly created cliques with the old ones.
    std::vector< Edge > notAffectedneighbourCliques;

    // parse all the affected MPS and get the corresponding cliques

    for (const auto& elt: mps_affected__)
      if (elt.second) {
        // get the cliques contained in this MPS
        const std::vector< NodeId >& cliques = cliques_of_mps__[elt.first];

        for (unsigned int i = 0; i < cliques.size(); ++i)
          all_cliques_affected.insert(cliques[i], true);
      }

    // for each connected set of cliques involved in the triangulations
    // perform a new triangulation and update the max prime subgraph tree
    for (const auto& elt: all_cliques_affected) {
      if (elt.second) {
        // set up the connected subgraph that need be retriangulated and the
        // cliques that are affected by this triangulation
        tmp_graph.clear();
        notAffectedneighbourCliques.clear();
        setUpConnectedTriangulation__(elt.first,
                                      elt.first,
                                      tmp_graph,
                                      notAffectedneighbourCliques,
                                      all_cliques_affected);

        // insert the edges in tmp_graph
        for (auto edge: graph__.edges()) {
          try {
            tmp_graph.addEdge(edge.first(), edge.second());
          } catch (Exception&) {}   // both extremities must be in tmp_graph
        }

        // remove from the mps_of_node table the affected mps containing the
        // node
        // for ( UndiGraph::NodeIterator iter_node =
        // tmp_graph.beginNodes();iter_node
        // != tmp_graph.endNodes(); ++iter_node ) {
        for (const auto node: tmp_graph.nodes()) {
          List< NodeId >& mps = mps_of_node__[node];

          for (HashTableConstIteratorSafe< NodeId, bool > iter_mps =
                  mps_affected__.beginSafe();   // safe iterator needed here
               iter_mps != mps_affected__.endSafe();
               ++iter_mps) {
            if (iter_mps.val()) mps.eraseByVal(iter_mps.key());
          }
        }

        // now tmp_graph contains the graph that should be triangulated.
        // so triangulate it and get its junction tree
        triangulation__->setGraph(&tmp_graph, &domain_sizes__);

        const CliqueGraph& tmp_junction_tree = triangulation__->junctionTree();

        // now, update the global junction tree
        // first add the nodes of tmp_junction_tree to junction_tree__
        // to do so, store the translations of the node ids of tmp_junction_tree
        // into the node ids of junction_tree__
        NodeProperty< NodeId > tmp2global_junction_tree(tmp_junction_tree.size());

        for (const auto cliq: tmp_junction_tree.nodes()) {
          // get new ids for the nodes of tmp_junction_tree. These should be
          // greater than or equal to junction_tree__.bound () so that we can
          // use the max_old_id defined at the beginning of the method.
          NodeId new_id = junction_tree__.addNode(tmp_junction_tree.clique(cliq));
          // translate the id of the temprary JT into an id of the global JT
          tmp2global_junction_tree.insert(cliq, new_id);
          new_nodes_in_junction_tree.insert(new_id);
        }

        // and add the edges of tmp_junction_tree to junction_tree__
        for (const auto& edge: tmp_junction_tree.edges())
          junction_tree__.addEdge(tmp2global_junction_tree[edge.first()],
                                  tmp2global_junction_tree[edge.second()]);

        // second get the edges in junction_tree__ that have an extremal clique
        // R
        // in the affected clique set and the other one S not in the affected
        // set
        // and see which new node V in the junction_tree__ should be connected
        // to S. The running intersection property guarrantees that the clique
        // in
        // the tmp_junction_tree that results from the elimination (during the
        // triangulation process) of the first eliminated node in the separator
        // between R and S is an admissible candidate
        for (unsigned int i = 0; i < notAffectedneighbourCliques.size(); ++i) {
          // check that the separator is not empty. If this is the case, do not
          // link the new junction tree to the old one
          const NodeSet& sep =
             junction_tree__.separator(notAffectedneighbourCliques[i]);

          if (sep.size() != 0) {
            // now find the first eliminated node in the separator
            Size   elim_order__ = tmp_graph.bound() + 1;
            NodeId elim_node = 0;

            for (const auto id: sep) {
              Size new_order = triangulation__->eliminationOrder(id);

              if (new_order < elim_order__) {
                elim_order__ = new_order;
                elim_node = id;
              }
            }

            // find the junction_tree__ clique corresponding to the elimination
            // of
            // elim_node and insert an edge between this clique and that which
            // was
            // not affected
            NodeId& to_connect =
               tmp2global_junction_tree[triangulation__->createdJunctionTreeClique(
                  elim_node)];

            NodeId not_affected =
               all_cliques_affected.exists(notAffectedneighbourCliques[i].first())
                  ? notAffectedneighbourCliques[i].second()
                  : notAffectedneighbourCliques[i].first();

            junction_tree__.addEdge(not_affected, to_connect);

            if (!new_nodes_in_junction_tree.contains(to_connect)) {
              T_mpd__.addEdge(mps_of_clique__[to_connect],
                              mps_of_clique__[not_affected]);
            }

            // check that the separator created by the new edge is not equal to
            // to_connect. If this is the case, then to_connect is included in
            // not_affected and, hence, should be removed from the graph
            if (junction_tree__.separator(not_affected, to_connect).size()
                == junction_tree__.clique(to_connect).size()) {
              junction_tree__.eraseEdge(Edge(not_affected, to_connect));

              for (const auto neighbour: junction_tree__.neighbours(to_connect)) {
                junction_tree__.addEdge(neighbour, not_affected);

                if (!new_nodes_in_junction_tree.contains(neighbour))
                  T_mpd__.addEdge(mps_of_clique__[neighbour],
                                  mps_of_clique__[not_affected]);
              }

              junction_tree__.eraseNode(to_connect);

              to_connect = not_affected;
            }
          }
        }
      }
    }

    // remove the mps that were affected and update the cliques_of_mps table
    for (const auto& elt: all_cliques_affected) {
      mps_of_clique__.erase(elt.first);
      junction_tree__.eraseNode(elt.first);
    }

    for (const auto& elt: mps_affected__)
      if (elt.second) {
        cliques_of_mps__.erase(elt.first);
        T_mpd__.eraseNode(elt.first);
      }
  }

  /// used for computing the junction tree of the maximal prime subgraphs

  void IncrementalTriangulation::computeMaxPrimeMergings__(
     const NodeId                                node,
     const NodeId                                from,
     std::vector< std::pair< NodeId, NodeId > >& merged_cliques,
     HashTable< NodeId, bool >&                  mark,
     const NodeSet& new_nodes_in_junction_tree) const {
    mark[node] = true;

    // check the separators on all the adjacent edges of Mx
    for (const auto other_node: junction_tree__.neighbours(node))
      if (other_node != from) {
        const NodeSet& separator =
           junction_tree__.separator(Edge(other_node, node));

        // check that the separator between node and other_node is complete
        bool complete = true;

        for (auto iter_sep1 = separator.begin();
             iter_sep1 != separator.end() && complete;
             ++iter_sep1) {
          auto iter_sep2 = iter_sep1;

          for (++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2) {
            if (!graph__.existsEdge(*iter_sep1, *iter_sep2)) {
              complete = false;
              break;
            }
          }
        }

        // here complete indicates whether the separator is complete or not
        if (!complete)
          merged_cliques.push_back(std::pair< NodeId, NodeId >(other_node, node));

        if (new_nodes_in_junction_tree.contains(other_node))
          computeMaxPrimeMergings__(
             other_node, node, merged_cliques, mark, new_nodes_in_junction_tree);
      }
  }

  /// update the max prime subgraph

  void IncrementalTriangulation::updateMaxPrimeSubgraph__(
     NodeProperty< bool >& all_cliques_affected,
     const NodeSet&        new_nodes_in_junction_tree) {
    // the maximal prime subgraph join tree is created by aggregation of some
    // cliques. More precisely, when the separator between 2 cliques is not
    // complete in the original graph, then the two cliques must be merged.

    // Create a hashtable indicating which clique has been absorbed by some
    // other
    // clique. Keys are the cliques absorbed, and values are the cliques that
    // absorb them.
    HashTable< NodeId, NodeId > T_mpd_cliques(all_cliques_affected.size());

    for (const auto clik: junction_tree__.nodes())
      if (new_nodes_in_junction_tree.contains(clik))
        T_mpd_cliques.insert(clik, clik);

    // parse all the separators of the junction tree and test those that are not
    // complete in the original graph
    std::vector< std::pair< NodeId, NodeId > > merged_cliques;

    HashTable< NodeId, bool > mark = T_mpd_cliques.map(false);

    for (const auto& elt: mark)
      if (!elt.second)
        computeMaxPrimeMergings__(
           elt.first, elt.first, merged_cliques, mark, new_nodes_in_junction_tree);

    // compute the transitive closure of merged_cliques. This one will contain
    // pairs (X,Y) indicating that clique X must be merged with clique Y.
    // Actually clique X will be inserted into clique Y.
    for (unsigned int i = 0; i < merged_cliques.size(); ++i) {
      if (T_mpd_cliques.exists(merged_cliques[i].second))
        T_mpd_cliques[merged_cliques[i].first] =
           T_mpd_cliques[merged_cliques[i].second];
      else
        T_mpd_cliques[merged_cliques[i].first] =
           mps_of_clique__[merged_cliques[i].second];
    }

    // now we can create the max prime junction tree.

    // create a map translating the cliques' ids in the junction tree into
    // cliques' id in the T_mpd_ tree
    NodeProperty< NodeId > clique2MPS(T_mpd_cliques.size());

    // First, create the new cliques and create the corresponding
    // cliques_of_mps entries
    for (const auto& elt: T_mpd_cliques)
      if (elt.first == elt.second) {
        NodeId newId = T_mpd__.addNode(junction_tree__.clique(elt.second));
        clique2MPS.insert(elt.second, newId);
        std::vector< NodeId >& vect_of_cliques =
           cliques_of_mps__.insert(newId, std::vector< NodeId >()).second;
        vect_of_cliques.push_back(elt.second);
      }

    // add to the cliques previously created the nodes of the cliques that were
    // merged into them and update the cliques_of_mps
    for (const auto& elt: T_mpd_cliques)
      if ((elt.first != elt.second)
          && (new_nodes_in_junction_tree.contains(elt.second))) {
        const NodeId idMPS = clique2MPS[elt.second];

        for (const auto node: junction_tree__.clique(elt.first)) {
          try {
            T_mpd__.addToClique(idMPS, node);
          } catch (DuplicateElement&) {}
        }

        cliques_of_mps__[idMPS].push_back(elt.first);
      }

    // update the mps_of_node and the mps_of_clique
    for (const auto& elt: T_mpd_cliques) {
      const NodeId idMPS = clique2MPS[elt.second];
      mps_of_clique__.insert(elt.first, idMPS);

      if (elt.first == elt.second)
        for (const auto node: T_mpd__.clique(idMPS))
          mps_of_node__[node].insert(idMPS);
    }

    // add the edges to the max prime subgraph tree
    for (const auto& elt: T_mpd_cliques) {
      NodeId clique = clique2MPS[elt.second];

      for (const auto othernode: junction_tree__.neighbours(elt.first))
        if (T_mpd_cliques.exists(othernode)) {
          // here iter is linked to another node that has been created during
          // the triangulation
          NodeId otherClique = clique2MPS[T_mpd_cliques[othernode]];

          // avoid adding the same edge several times
          if (clique > otherClique) { T_mpd__.addEdge(clique, otherClique); }
        } else {
          T_mpd__.addEdge(clique, mps_of_clique__[othernode]);
        }
    }
  }

  /// updates the triangulated graph using the modif list

  void IncrementalTriangulation::updateTriangulation() {
    if (!require_update__) return;
    // the set of all the cliques that should be affected by the different
    // triangulations we will perform (one by connected component)
    NodeProperty< bool > all_cliques_affected(junction_tree__.size());

    // we need to keep track of the new node ids that will be inserted
    // into junction_tree__. A priori, these should be equal to the ids
    // inserted into tmp2global_junction_tree. But, sometimes, some new nodes
    // are included into old nodes and, in this case, the translation in
    // tmp2global_junction_tree indicates the the new node inserted corresponds
    // to an old node. Here we wish to know this additional information
    NodeSet new_nodes_in_junction_tree;

    updateJunctionTree__(all_cliques_affected, new_nodes_in_junction_tree);

    // now update the T_mpd so that it be coherent with the junction tree
    updateMaxPrimeSubgraph__(all_cliques_affected, new_nodes_in_junction_tree);

    // reset the MPS that are affected
    mps_affected__.clear();

    for (const auto node: T_mpd__.nodes())
      mps_affected__.insert(node, false);

    // remove all the structures used by the triangulation algorithm
    triangulation__->clear();

    require_update__ = false;
  }

  /// sets the graph to the empty graph

  void IncrementalTriangulation::clear() {
    graph__.clear();
    domain_sizes__.clear();
    junction_tree__.clear();
    T_mpd__.clear();
    mps_of_node__.clear();
    cliques_of_mps__.clear();
    mps_of_clique__.clear();
    mps_affected__.clear();
    triangulation__->clear();
    require_update__ = false;
    require_elimination_order__ = false;
    elimination_order__.clear();
    reverse_elimination_order__.clear();
    require_created_JT_cliques__ = false;
    created_JT_cliques__.clear();
  }

  /// a collect algorithm to compute, for each node, one container JT's clique

  void IncrementalTriangulation::collectJTCliques__(
     const NodeId clique, const NodeId from, NodeProperty< bool >& examined) {
    // apply collect to all the neighbours except from
    for (const auto otherclique: junction_tree__.neighbours(clique))
      if (otherclique != from) collectJTCliques__(otherclique, clique, examined);

    // get the nodes that belong to clique and not to from
    examined[clique] = true;

    const NodeSet& cliquenodes = junction_tree__.clique(clique);

    if (from != clique) {
      const NodeSet& separator = junction_tree__.separator(clique, from);

      for (const auto cli: cliquenodes)
        if (!separator.contains(cli)) created_JT_cliques__.insert(cli, clique);
    } else {
      for (const auto cli: cliquenodes)
        created_JT_cliques__.insert(cli, clique);
    }
  }

  /** @brief returns the Ids of the cliques of the junction tree created by the
   * elimination of the nodes */

  const NodeProperty< NodeId >&
     IncrementalTriangulation::createdJunctionTreeCliques() {
    // check if we already computed the containing cliques
    if (!require_created_JT_cliques__) return created_JT_cliques__;

    // we first we compute the junction tree
    updateTriangulation();

    created_JT_cliques__.clear();

    require_created_JT_cliques__ = false;

    if (junction_tree__.size() == 0) { return created_JT_cliques__; }

    // now we can use a collect algorithm to get the containing cliques
    NodeProperty< bool > examined = junction_tree__.nodesProperty< bool >(false);

    for (const auto& elt: examined)
      if (!elt.second) collectJTCliques__(elt.first, elt.first, examined);

    return created_JT_cliques__;
  }

  /// returns a clique containing a given node of the triangulated graph

  NodeId IncrementalTriangulation::createdJunctionTreeClique(NodeId id) {
    createdJunctionTreeCliques();
    return created_JT_cliques__[id];
  }

  /** @brief returns the Id of the maximal prime subgraph created by the
   * elimination of a given node during the triangulation process */

  NodeId IncrementalTriangulation::createdMaxPrimeSubgraph(const NodeId id) {
    // get the created junction tree clique and get its MPS
    return mps_of_clique__[createdJunctionTreeClique(id)];
  }

  /// changes the current graph

  void IncrementalTriangulation::setGraph(const UndiGraph*            graph,
                                          const NodeProperty< Size >* dom_sizes) {
    // check that both the graph and the domain sizes are different from nullptr
    // or else that both are equal to nullptr
    if (((graph != nullptr) && (dom_sizes == nullptr))
        || ((graph == nullptr) && (dom_sizes != nullptr))) {
      GUM_ERROR(GraphError,
                "one of the graph or the set of domain sizes "
                "is a null pointer.");
    }

    // remove the current graph
    clear();

    // copy the graph passed in arent and update the structures
    // containing the informations useful for the triangulation
    if (graph != nullptr) {
      for (const auto node: *graph)
        addNode(node, (*dom_sizes)[node]);

      for (const auto& edge: graph->edges())
        addEdge(edge.first(), edge.second());
    }
  }

  /// a collect algorithm to compute elimination orderings

  void IncrementalTriangulation::collectEliminationOrder__(
     const NodeId          node,
     const NodeId          from,
     NodeProperty< bool >& examined,
     Idx&                  index) {
    // apply collect to all the neighbours except from
    for (const auto othernode: junction_tree__.neighbours(node))
      if (othernode != from)
        collectEliminationOrder__(othernode, node, examined, index);

    // get the nodes that belong to node and not to from
    examined[node] = true;

    const NodeSet& clique = junction_tree__.clique(node);

    if (from != node) {
      const NodeSet& separator = junction_tree__.separator(node, from);

      for (const auto cli: clique) {
        if (!separator.contains(cli)) {
          elimination_order__[index] = cli;
          reverse_elimination_order__.insert(cli, index);
          ++index;
        }
      }
    } else {
      for (const auto cli: clique) {
        elimination_order__[index] = cli;
        reverse_elimination_order__.insert(cli, index);
        ++index;
      }
    }
  }

  /// get a possible elimination ordering

  const std::vector< NodeId >& IncrementalTriangulation::eliminationOrder() {
    // check if we already computed the elimination order
    if (!require_elimination_order__) return elimination_order__;

    // to compute the elimination order, we first we compute the junction tree
    updateTriangulation();

    elimination_order__.resize(graph__.size());

    reverse_elimination_order__.clear();

    require_elimination_order__ = false;

    if (junction_tree__.size() == Size(0)) { return elimination_order__; }

    // now we can use a collect algorithm to get the elimination order
    Idx index = Idx(0);

    NodeProperty< bool > examined = junction_tree__.nodesProperty< bool >(false);

    for (const auto& elt: examined)
      if (!elt.second)
        collectEliminationOrder__(elt.first, elt.first, examined, index);

    return elimination_order__;
  }

  /** @brief returns the number of a given node in the elimination order
   * (0 = first node eliminated) */

  Idx IncrementalTriangulation::eliminationOrder(const NodeId node) {
    if (!graph__.existsNode(node)) {
      GUM_ERROR(NotFound, "the node " << node << " does not exist");
    }

    // compute the elimination order
    eliminationOrder();

    return reverse_elimination_order__[node];
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
