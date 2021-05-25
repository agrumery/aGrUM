/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
#include <agrum/tools/core/math/math_utils.h>

#include <agrum/tools/core/list.h>
#include <agrum/tools/graphs/algorithms/triangulations/incrementalTriangulation.h>
#include <agrum/tools/graphs/undiGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/triangulations/incrementalTriangulation_inl.h>
#endif   // GUM_NO_INLINE

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// constructor
  IncrementalTriangulation::IncrementalTriangulation(const UnconstrainedTriangulation& triang_algo,
                                                     const UndiGraph*                  theGraph,
                                                     const NodeProperty< Size >*       domsizes) :
      Triangulation(domsizes),
      _triangulation_(triang_algo.newFactory()) {
    GUM_CONSTRUCTOR(IncrementalTriangulation);

    // reset the triangulation algorithm => it starts with an empty graph
    _triangulation_->clear();

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
      _triangulation_(triang_algo.newFactory()) {
    GUM_CONSTRUCTOR(IncrementalTriangulation);

    // reset the triangulation algorithm => it starts with an empty graph
    _triangulation_->clear();
  }

  /// copy operator
  IncrementalTriangulation::IncrementalTriangulation(const IncrementalTriangulation& from) :
      Triangulation(from), _graph_(from._graph_), _junction_tree_(from._junction_tree_),
      _T_mpd_(from._T_mpd_), _mps_of_node_(from._mps_of_node_),
      _cliques_of_mps_(from._cliques_of_mps_), _mps_of_clique_(from._mps_of_clique_),
      _mps_affected_(from._mps_affected_), _triangulation_(from._triangulation_->newFactory()),
      _require_update_(from._require_update_),
      _require_elimination_order_(from._require_elimination_order_),
      _elimination_order_(from._elimination_order_),
      _reverse_elimination_order_(from._reverse_elimination_order_),
      _require_created_JT_cliques_(from._require_created_JT_cliques_),
      _created_JT_cliques_(from._created_JT_cliques_) {
    GUM_CONS_CPY(IncrementalTriangulation);

    _domain_sizes_ = from._domain_sizes_;
  }

  /// destructor
  IncrementalTriangulation::~IncrementalTriangulation() {
    GUM_DESTRUCTOR(IncrementalTriangulation);

    // remove things that were allocated within the current class
    delete _triangulation_;
  }

  /// virtual clone constructor
  IncrementalTriangulation* IncrementalTriangulation::newFactory() const {
    return new IncrementalTriangulation(*_triangulation_);
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
      GUM_OP_CPY(IncrementalTriangulation)

      // copy all the structures stored in "from"
      _graph_                      = from._graph_;
      _domain_sizes_               = from._domain_sizes_;
      _junction_tree_              = from._junction_tree_;
      _T_mpd_                      = from._T_mpd_;
      _mps_of_node_                = from._mps_of_node_;
      _cliques_of_mps_             = from._cliques_of_mps_;
      _mps_of_clique_              = from._mps_of_clique_;
      _mps_affected_               = from._mps_affected_;
      _require_update_             = from._require_update_;
      _require_elimination_order_  = from._require_elimination_order_;
      _elimination_order_          = from._elimination_order_;
      _reverse_elimination_order_  = from._reverse_elimination_order_;
      _require_created_JT_cliques_ = from._require_created_JT_cliques_;
      _created_JT_cliques_         = from._created_JT_cliques_;

      // just in case we changed the triangulation algorithm, we remove it
      // and create it again
      delete _triangulation_;
      _triangulation_ = from._triangulation_->newFactory();
    }

    return *this;
  }

  /// adds a new node to the graph (and update the internal structures)
  void IncrementalTriangulation::addNode(const NodeId node, Size modal) {
    // check if the node already exists
    if (_graph_.existsNode(node)) return;

    // add the new node to the graph
    _graph_.addNodeWithId(node);
    _domain_sizes_.insert(node, modal);

    // add a new clique to T_mpd and the junction tree
    NodeSet clique_nodes(2);
    clique_nodes.insert(node);

    NodeId MPS        = _T_mpd_.addNode(clique_nodes);
    NodeId new_clique = _junction_tree_.addNode(clique_nodes);

    // indicate in which MPS node belongs
    List< NodeId >& list_of_mps = _mps_of_node_.insert(node, List< NodeId >()).second;
    list_of_mps.insert(MPS);

    // indicate in which MPS the clique added to the junction tree belongs
    std::vector< NodeId >& cliquesMPS
       = _cliques_of_mps_.insert(MPS, std::vector< NodeId >()).second;

    cliquesMPS.push_back(new_clique);
    _mps_of_clique_.insert(new_clique, MPS);

    // indicate that the new MPS should not be affected by a triangulation
    _mps_affected_.insert(MPS, false);

    // insert the node into the elimination order sequence
    _elimination_order_.push_back(node);

    if (!_reverse_elimination_order_.exists(node))
      _reverse_elimination_order_.insert(node, Size(_elimination_order_.size()));

    if (!_created_JT_cliques_.exists(node)) _created_JT_cliques_.insert(node, new_clique);
  }

  /// mark the mps affected by the deletion of a given edge

  void IncrementalTriangulation::_markAffectedMPSsByRemoveLink_(const NodeId My,
                                                                const NodeId Mz,
                                                                const Edge&  edge) {
    // mark the MPS so that it will be retriangulated
    _mps_affected_[My] = true;

    // mark all the neighbour MPS that contain edge
    for (const auto nei: _T_mpd_.neighbours(My))
      if (nei != Mz) {
        const NodeSet& Syk = _T_mpd_.separator(Edge(nei, My));

        if (Syk.contains(edge.first()) && Syk.contains(edge.second()))
          _markAffectedMPSsByRemoveLink_(nei, My, edge);
      }
  }

  /// removes an edge from the graph (the join tree may need a retriangulation)

  void IncrementalTriangulation::eraseEdge(const Edge& edge) {
    // check that the edge exist
    if (!_graph_.existsEdge(edge)) return;

    // find a MPS containing the edge (X,Y)
    const NodeId X = edge.first();
    const NodeId Y = edge.second();

    const List< NodeId >& mps1 = _mps_of_node_[X];
    const List< NodeId >& mps2 = _mps_of_node_[Y];

    NodeId Mx = mps1[0];

    if (mps1.size() <= mps2.size()) {
      for (const auto node: mps1)
        if (_T_mpd_.clique(node).contains(Y)) {
          Mx = node;
          break;
        }
    } else {
      for (const auto node: mps2)
        if (_T_mpd_.clique(node).contains(X)) {
          Mx = node;
          break;
        }
    }

    // mark the MPS that need be updated
    _markAffectedMPSsByRemoveLink_(Mx, Mx, edge);

    _require_update_ = true;

    _require_elimination_order_ = true;

    _require_created_JT_cliques_ = true;

    // remove the edge (X,Y) from the graph
    _graph_.eraseEdge(edge);
  }

  /// removes a node from the graph (the join tree may need a triangulation
  /// update)

  void IncrementalTriangulation::eraseNode(const NodeId X) {
    // check if the node exists
    if (!_graph_.existsNode(X)) return;

    // remove all the edges adjacent to the node
    {
      const NodeSet& neighbours = _graph_.neighbours(X);

      for (auto neighbour_edge = neighbours.beginSafe();   // safe iterator needed here
           neighbour_edge != neighbours.endSafe();
           ++neighbour_edge) {
        eraseEdge(Edge(*neighbour_edge, X));
      }
    }

    auto& MPS_of_X = _mps_of_node_[X];

    // remove X from the MPS containing X
    for (const auto node: MPS_of_X) {
      _T_mpd_.eraseFromClique(node, X);

      // if the intersection between *iter and one of its neighbour is empty,
      // remove the edge linking them
      auto& neighbours = _T_mpd_.neighbours(node);

      for (auto it_neighbour = neighbours.beginSafe(); it_neighbour != neighbours.endSafe();
           ++it_neighbour) {   // safe iterator needed here
        Edge neigh(*it_neighbour, node);

        if (_T_mpd_.separator(neigh).size() == 0) _T_mpd_.eraseEdge(neigh);
      }
    }

    // remove X from the cliques containing X
    for (const auto clique: MPS_of_X) {
      const std::vector< NodeId >& cliques_of_X = _cliques_of_mps_[clique];

      for (unsigned int i = 0; i < cliques_of_X.size(); ++i) {
        _junction_tree_.eraseFromClique(cliques_of_X[i], X);

        // if the intersection between clique and one of its neighbour is empty,
        // remove the edge linking them only if, in addition, there is no
        // edge in  _graph_ between a node of clique and a node in the neighbour
        auto& neighbours = _junction_tree_.neighbours(cliques_of_X[i]);

        for (auto it_neighbour = neighbours.beginSafe(); it_neighbour != neighbours.endSafe();
             ++it_neighbour) {   // safe iterator needed here
          Edge neigh(*it_neighbour, cliques_of_X[i]);

          if (_junction_tree_.separator(neigh).size() == 0) {
            // try to see if there is an edge between the nodes of one extremity
            // of *neighbour and those of the other extremity
            bool hasCommonEdge = false;

            for (const auto node1: _junction_tree_.clique(neigh.first()))
              for (const auto node2: _junction_tree_.clique(neigh.second()))
                if (_graph_.existsEdge(node1, node2)) {
                  hasCommonEdge = true;
                  break;
                }

            if (!hasCommonEdge) { _junction_tree_.eraseEdge(neigh); }
          }
        }
      }
    }

    // if the MPS containing X is empty, then remove it, as well as the
    // corresponding clique in the junction tree (which also only contains X)
    if ((MPS_of_X.size() == 1) && (_T_mpd_.clique(MPS_of_X[0]).size() == 0)) {
      _junction_tree_.eraseNode(_cliques_of_mps_[MPS_of_X[0]][0]);
      _T_mpd_.eraseNode(MPS_of_X[0]);
      _mps_of_clique_.erase(_cliques_of_mps_[MPS_of_X[0]][0]);
      _cliques_of_mps_.erase(MPS_of_X[0]);
      _mps_affected_.erase(MPS_of_X[0]);
    }

    _mps_of_node_.erase(X);

    // update the elimination orders

    if (!_require_update_) {
      for (Idx i = _reverse_elimination_order_[X] + 1; i < _reverse_elimination_order_.size(); ++i)
        _elimination_order_[i - 1] = _elimination_order_[i];

      _elimination_order_.pop_back();

      _reverse_elimination_order_.erase(X);

      _created_JT_cliques_.erase(X);
    }

    // remove X completely from the graph
    _graph_.eraseNode(X);

    _domain_sizes_.erase(X);
  }

  /// add a new edge to the graph

  int IncrementalTriangulation::_markAffectedMPSsByAddLink_(const NodeId Mx,
                                                            const NodeId Mz,
                                                            const NodeId X,
                                                            const NodeId Y) {
    // check if Mx contains Y. In this case, mark Mx and return 1 to indicate
    // that
    // Y has been found or 2 to indicate that Y has been found and that the
    // nearest
    // MPS containing X has been marked
    const NodeSet& cliqueMX = _T_mpd_.clique(Mx);

    if (cliqueMX.contains(Y)) {
      _mps_affected_[Mx] = true;

      if (cliqueMX.contains(X)) return 2;

      return 1;
    }

    // parse Mx's neighbours until we find Y
    for (const auto other_node: _T_mpd_.neighbours(Mx))
      if (other_node != Mz) {
        int neighbourStatus = _markAffectedMPSsByAddLink_(other_node, Mx, X, Y);

        if (neighbourStatus == 2)
          return 2;
        else if (neighbourStatus == 1) {
          _mps_affected_[Mx] = true;

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
    if ((X == Y) || !_graph_.existsNode(X) || !_graph_.existsNode(Y)
        || _graph_.existsEdge(Edge(X, Y)))
      return;

    // add the edge to the graph
    _graph_.addEdge(X, Y);

    // take any MPS containing X and search its tree to find Y
    NodeId& mps_X = _mps_of_node_[X][0];

    int found = _markAffectedMPSsByAddLink_(mps_X, mps_X, X, Y);

    if (found == 0) {
      // the mps containing X do not belong to the same tree as those containing
      // Y
      NodeId& mps_Y = _mps_of_node_[Y][0];

      // find a clique in mps_X containing X and another in mps_Y containing Y
      // and add a clique XY to the junction tree linked to the cliques found
      // in mps_X and mps_Y
      const std::vector< NodeId >& cliques_X = _cliques_of_mps_[mps_X];
      const std::vector< NodeId >& cliques_Y = _cliques_of_mps_[mps_Y];
      NodeId                       c_X = 0, c_Y = 0;

      for (unsigned int i = 0; i < cliques_X.size(); ++i) {
        if (_junction_tree_.clique(cliques_X[i]).contains(X)) {
          c_X = cliques_X[i];
          break;
        }
      }

      for (unsigned int i = 0; i < cliques_Y.size(); ++i) {
        if (_junction_tree_.clique(cliques_Y[i]).contains(Y)) {
          c_Y = cliques_Y[i];
          break;
        }
      }

      // link c_Y and c_X through a new node containing XY
      NodeSet nodes(2);

      nodes.insert(X);

      nodes.insert(Y);

      NodeId newNode = _junction_tree_.addNode(nodes);

      _junction_tree_.addEdge(newNode, c_X);
      _junction_tree_.addEdge(newNode, c_Y);

      NodeId newMPS = _T_mpd_.addNode(nodes);

      _T_mpd_.addEdge(newMPS, mps_X);
      _T_mpd_.addEdge(newMPS, mps_Y);

      // check that the maximal prime subgraph containing X is not X alone
      // in this case, remove this max prime subgraph, as well as the
      // corresponding
      // clique in the junction tree
      if (_T_mpd_.clique(mps_X).size() == 1) {
        _junction_tree_.eraseNode(c_X);
        _T_mpd_.eraseNode(mps_X);
        _mps_affected_.erase(mps_X);
        _mps_of_clique_.erase(c_X);
        _cliques_of_mps_.erase(mps_X);
        _created_JT_cliques_[X] = newNode;
        mps_X                   = newMPS;
      } else
        _mps_of_node_[X].insert(newMPS);

      // do the same thing as above for node Y
      if (_T_mpd_.clique(mps_Y).size() == 1) {
        _junction_tree_.eraseNode(c_Y);
        _T_mpd_.eraseNode(mps_Y);
        _mps_affected_.erase(mps_Y);
        _mps_of_clique_.erase(c_Y);
        _cliques_of_mps_.erase(mps_Y);
        _created_JT_cliques_[Y] = newNode;
        mps_Y                   = newMPS;
      } else
        _mps_of_node_[Y].insert(newMPS);

      std::vector< NodeId >& cl = _cliques_of_mps_.insert(newMPS, std::vector< NodeId >()).second;

      cl.push_back(newNode);

      _mps_of_clique_.insert(newNode, newMPS);

      _mps_affected_.insert(newMPS, false);
    } else {
      _require_update_             = true;
      _require_created_JT_cliques_ = true;
    }

    // in all cases, recompute the elimination ordering
    _require_elimination_order_ = true;
  }

  /// checks that the incremental triangulation works properly

  bool IncrementalTriangulation::_check_() {
    // just in case, update everything
    updateTriangulation();

    bool OK = true;

    // check that all the nodes of the graph belong to the junction tree
    {
      NodeProperty< bool > nodesProp = _graph_.nodesProperty< bool >(false);

      for (const auto cliq: _junction_tree_.nodes())
        for (const auto node: _junction_tree_.clique(cliq))
          nodesProp[node] = true;

      for (const auto& elt: nodesProp)
        if (!elt.second) {
          std::cerr << "check nodes" << std::endl
                    << _graph_ << std::endl
                    << _junction_tree_ << std::endl;
          OK = false;
        }

      if (!OK) return false;
    }

    // check that the edgs belong to the junction tree
    {
      std::pair< NodeId, NodeId > thePair;
      EdgeProperty< bool >        edgesProp = _graph_.edgesProperty(false);

      for (const auto cliq: _junction_tree_.nodes()) {
        const NodeSet& clique = _junction_tree_.clique(cliq);

        for (auto iter2 = clique.begin(); iter2 != clique.end(); ++iter2) {
          auto iter3 = iter2;

          for (++iter3; iter3 != clique.end(); ++iter3) {
            thePair.first  = std::min(*iter2, *iter3);
            thePair.second = std::max(*iter2, *iter3);

            if (_graph_.existsEdge(thePair.first, thePair.second))
              edgesProp[Edge(thePair.first, thePair.second)] = true;
          }
        }
      }

      for (const auto& elt: edgesProp)
        if (!elt.second) {
          std::cerr << "check edges" << std::endl
                    << _graph_ << std::endl
                    << _junction_tree_ << std::endl;
          OK = false;
        }

      if (!OK) return false;
    }

    // check that all the nodes of the graph belong to the MPS tree
    {
      NodeProperty< bool > nodesProp = _graph_.nodesProperty< bool >(false);

      for (const auto cliq: _T_mpd_.nodes())
        for (const auto node: _T_mpd_.clique(cliq))
          nodesProp[node] = true;

      for (const auto& elt: nodesProp)
        if (!elt.second) {
          std::cerr << "check nodes" << std::endl << _graph_ << std::endl << _T_mpd_ << std::endl;
          OK = false;
        }

      if (!OK) return false;
    }

    // check that the arcs of the graph belong to the MPS tree
    {
      std::pair< NodeId, NodeId > thePair;
      EdgeProperty< bool >        edgesProp = _graph_.edgesProperty(false);

      for (const auto cliq: _T_mpd_.nodes()) {
        const NodeSet& clique = _T_mpd_.clique(cliq);

        for (auto iter2 = clique.begin(); iter2 != clique.end(); ++iter2) {
          auto iter3 = iter2;

          for (++iter3; iter3 != clique.end(); ++iter3) {
            thePair.first  = std::min(*iter2, *iter3);
            thePair.second = std::max(*iter2, *iter3);

            if (_graph_.existsEdge(thePair.first, thePair.second))
              edgesProp[Edge(thePair.first, thePair.second)] = true;
          }
        }
      }

      for (const auto& elt: edgesProp)
        if (!elt.second) {
          std::cerr << "check edges" << std::endl << _graph_ << std::endl << _T_mpd_ << std::endl;
          OK = false;
        }

      if (!OK) return false;
    }

    // check the MPS of node
    {
      NodeProperty< NodeProperty< bool > > chk;

      for (const auto node: _graph_.nodes())
        chk.insert(node, HashTable< NodeId, bool >());

      for (const auto cliq: _T_mpd_.nodes())
        for (auto node: _T_mpd_.clique(cliq))
          chk[node].insert(cliq, false);

      for (const auto& elt: _mps_of_node_) {
        HashTable< NodeId, bool >& hash = chk[elt.first];

        for (const auto cell: elt.second) {
          if (!hash.exists(cell)) {
            std::cerr << "check mps of nodes" << std::endl
                      << _T_mpd_ << std::endl
                      << _mps_of_node_ << std::endl;
            OK = false;
          }

          hash[cell] = true;
        }
      }

      for (const auto& elt: chk)
        for (const auto& elt2: elt.second)
          if (!elt2.second) {
            std::cerr << "check mps of nodes2" << std::endl
                      << _T_mpd_ << std::endl
                      << _mps_of_node_ << std::endl;
            OK = false;
          }

      if (!OK) return false;
    }

    // check that the junction tree and the T_mpd are junction trees
    {
      if (!_junction_tree_.isJoinTree()) {
        std::cerr << "check join tree  _junction_tree_" << std::endl
                  << _junction_tree_ << std::endl;
        return false;
      }

      if (!_T_mpd_.isJoinTree()) {
        std::cerr << "check join tree   _T_mpd_" << std::endl << _T_mpd_ << std::endl;
        return false;
      }
    }

    // check elimination sequences
    {
      eliminationOrder();

      if (_elimination_order_.size() != _graph_.size()) {
        std::cerr << "check elimination order" << std::endl << _elimination_order_ << std::endl;
        return false;
      }

      NodeSet nodes;

      for (const auto node: _graph_.nodes()) {
        if (nodes.exists(node)) {
          std::cerr << "check elimination order" << std::endl << _elimination_order_ << std::endl;
          return false;
        } else
          nodes.insert(node);
      }

      if (nodes.size() != _graph_.size()) {
        std::cerr << "check elimination order" << std::endl << _elimination_order_ << std::endl;
        return false;
      }

      if (_reverse_elimination_order_.size() != _graph_.size()) {
        std::cerr << "check reverse elimination order" << std::endl
                  << _reverse_elimination_order_ << std::endl;
        return false;
      }

      for (const auto node: _graph_.nodes()) {
        if (!_reverse_elimination_order_.exists(node)) {
          std::cerr << "check reverse elimination order" << std::endl
                    << _reverse_elimination_order_ << std::endl;
          return false;
        }
      }
    }

    // check created junction tree cliques
    {
      createdJunctionTreeCliques();

      if (_created_JT_cliques_.size() != _graph_.size()) {
        std::cerr << "check creating JT cliques" << std::endl << _created_JT_cliques_ << std::endl;
        return false;
      }

      for (const auto node: _graph_.nodes()) {
        if (!_created_JT_cliques_.exists(node)
            || !_junction_tree_.existsNode(_created_JT_cliques_[node])) {
          std::cerr << "check created JT cliques" << std::endl << _created_JT_cliques_ << std::endl;
          return false;
        }
      }
    }

    return true;
  }

  /// set up a connected subgraph to be triangulated

  void IncrementalTriangulation::_setUpConnectedTriangulation_(
     NodeId                     Mx,
     NodeId                     Mfrom,
     UndiGraph&                 theGraph,
     std::vector< Edge >&       notAffectedneighbourCliques,
     HashTable< NodeId, bool >& cliques_affected) {
    // mark the clique so that we won't try to update it several times
    cliques_affected[Mx] = false;

    // get the nodes that are concerned by the triangulation update
    for (const auto node: _junction_tree_.clique(Mx))
      if (!theGraph.exists(node)) theGraph.addNodeWithId(node);

    // go on with the neighbour cliques in the junction tree
    for (const auto othernode: _junction_tree_.neighbours(Mx))
      if (othernode != Mfrom) {
        if (cliques_affected.exists(othernode)) {
          _setUpConnectedTriangulation_(othernode,
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

  void IncrementalTriangulation::_updateJunctionTree_(NodeProperty< bool >& all_cliques_affected,
                                                      NodeSet& new_nodes_in_junction_tree) {
    // a temporary subgraph in which we actually perform the triangulation
    UndiGraph tmp_graph;

    // for each triangulation, we will keep track of the cliques of the
    // junction tree that are not affected by the triangulation but that are
    // adjacent to cliques affected. This will enable us to connect easily the
    // newly created cliques with the old ones.
    std::vector< Edge > notAffectedneighbourCliques;

    // parse all the affected MPS and get the corresponding cliques

    for (const auto& elt: _mps_affected_)
      if (elt.second) {
        // get the cliques contained in this MPS
        const std::vector< NodeId >& cliques = _cliques_of_mps_[elt.first];

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
        _setUpConnectedTriangulation_(elt.first,
                                      elt.first,
                                      tmp_graph,
                                      notAffectedneighbourCliques,
                                      all_cliques_affected);

        // insert the edges in tmp_graph
        for (auto edge: _graph_.edges()) {
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
          List< NodeId >& mps = _mps_of_node_[node];

          for (HashTableConstIteratorSafe< NodeId, bool > iter_mps
               = _mps_affected_.beginSafe();   // safe iterator needed here
               iter_mps != _mps_affected_.endSafe();
               ++iter_mps) {
            if (iter_mps.val()) mps.eraseByVal(iter_mps.key());
          }
        }

        // now tmp_graph contains the graph that should be triangulated.
        // so triangulate it and get its junction tree
        _triangulation_->setGraph(&tmp_graph, &_domain_sizes_);

        const CliqueGraph& tmp_junction_tree = _triangulation_->junctionTree();

        // now, update the global junction tree
        // first add the nodes of tmp_junction_tree to  _junction_tree_
        // to do so, store the translations of the node ids of tmp_junction_tree
        // into the node ids of  _junction_tree_
        NodeProperty< NodeId > tmp2global_junction_tree(tmp_junction_tree.size());

        for (const auto cliq: tmp_junction_tree.nodes()) {
          // get new ids for the nodes of tmp_junction_tree. These should be
          // greater than or equal to  _junction_tree_.bound () so that we can
          // use the max_old_id defined at the beginning of the method.
          NodeId new_id = _junction_tree_.addNode(tmp_junction_tree.clique(cliq));
          // translate the id of the temprary JT into an id of the global JT
          tmp2global_junction_tree.insert(cliq, new_id);
          new_nodes_in_junction_tree.insert(new_id);
        }

        // and add the edges of tmp_junction_tree to  _junction_tree_
        for (const auto& edge: tmp_junction_tree.edges())
          _junction_tree_.addEdge(tmp2global_junction_tree[edge.first()],
                                  tmp2global_junction_tree[edge.second()]);

        // second get the edges in  _junction_tree_ that have an extremal clique
        // R
        // in the affected clique set and the other one S not in the affected
        // set
        // and see which new node V in the  _junction_tree_ should be connected
        // to S. The running intersection property guarrantees that the clique
        // in
        // the tmp_junction_tree that results from the elimination (during the
        // triangulation process) of the first eliminated node in the separator
        // between R and S is an admissible candidate
        for (unsigned int i = 0; i < notAffectedneighbourCliques.size(); ++i) {
          // check that the separator is not empty. If this is the case, do not
          // link the new junction tree to the old one
          const NodeSet& sep = _junction_tree_.separator(notAffectedneighbourCliques[i]);

          if (sep.size() != 0) {
            // now find the first eliminated node in the separator
            Size   _elim_order_ = tmp_graph.bound() + 1;
            NodeId elim_node    = 0;

            for (const auto id: sep) {
              Size new_order = _triangulation_->eliminationOrder(id);

              if (new_order < _elim_order_) {
                _elim_order_ = new_order;
                elim_node    = id;
              }
            }

            // find the  _junction_tree_ clique corresponding to the elimination
            // of
            // elim_node and insert an edge between this clique and that which
            // was
            // not affected
            NodeId& to_connect
               = tmp2global_junction_tree[_triangulation_->createdJunctionTreeClique(elim_node)];

            NodeId not_affected
               = all_cliques_affected.exists(notAffectedneighbourCliques[i].first())
                  ? notAffectedneighbourCliques[i].second()
                  : notAffectedneighbourCliques[i].first();

            _junction_tree_.addEdge(not_affected, to_connect);

            if (!new_nodes_in_junction_tree.contains(to_connect)) {
              _T_mpd_.addEdge(_mps_of_clique_[to_connect], _mps_of_clique_[not_affected]);
            }

            // check that the separator created by the new edge is not equal to
            // to_connect. If this is the case, then to_connect is included in
            // not_affected and, hence, should be removed from the graph
            if (_junction_tree_.separator(not_affected, to_connect).size()
                == _junction_tree_.clique(to_connect).size()) {
              _junction_tree_.eraseEdge(Edge(not_affected, to_connect));

              for (const auto neighbour: _junction_tree_.neighbours(to_connect)) {
                _junction_tree_.addEdge(neighbour, not_affected);

                if (!new_nodes_in_junction_tree.contains(neighbour))
                  _T_mpd_.addEdge(_mps_of_clique_[neighbour], _mps_of_clique_[not_affected]);
              }

              _junction_tree_.eraseNode(to_connect);

              to_connect = not_affected;
            }
          }
        }
      }
    }

    // remove the mps that were affected and update the cliques_of_mps table
    for (const auto& elt: all_cliques_affected) {
      _mps_of_clique_.erase(elt.first);
      _junction_tree_.eraseNode(elt.first);
    }

    for (const auto& elt: _mps_affected_)
      if (elt.second) {
        _cliques_of_mps_.erase(elt.first);
        _T_mpd_.eraseNode(elt.first);
      }
  }

  /// used for computing the junction tree of the maximal prime subgraphs

  void IncrementalTriangulation::_computeMaxPrimeMergings_(
     const NodeId                                node,
     const NodeId                                from,
     std::vector< std::pair< NodeId, NodeId > >& merged_cliques,
     HashTable< NodeId, bool >&                  mark,
     const NodeSet&                              new_nodes_in_junction_tree) const {
    mark[node] = true;

    // check the separators on all the adjacent edges of Mx
    for (const auto other_node: _junction_tree_.neighbours(node))
      if (other_node != from) {
        const NodeSet& separator = _junction_tree_.separator(Edge(other_node, node));

        // check that the separator between node and other_node is complete
        bool complete = true;

        for (auto iter_sep1 = separator.begin(); iter_sep1 != separator.end() && complete;
             ++iter_sep1) {
          auto iter_sep2 = iter_sep1;

          for (++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2) {
            if (!_graph_.existsEdge(*iter_sep1, *iter_sep2)) {
              complete = false;
              break;
            }
          }
        }

        // here complete indicates whether the separator is complete or not
        if (!complete) merged_cliques.push_back(std::pair< NodeId, NodeId >(other_node, node));

        if (new_nodes_in_junction_tree.contains(other_node))
          _computeMaxPrimeMergings_(other_node,
                                    node,
                                    merged_cliques,
                                    mark,
                                    new_nodes_in_junction_tree);
      }
  }

  /// update the max prime subgraph

  void
     IncrementalTriangulation::_updateMaxPrimeSubgraph_(NodeProperty< bool >& all_cliques_affected,
                                                        const NodeSet& new_nodes_in_junction_tree) {
    // the maximal prime subgraph join tree is created by aggregation of some
    // cliques. More precisely, when the separator between 2 cliques is not
    // complete in the original graph, then the two cliques must be merged.

    // Create a hashtable indicating which clique has been absorbed by some
    // other
    // clique. Keys are the cliques absorbed, and values are the cliques that
    // absorb them.
    HashTable< NodeId, NodeId > T_mpd_cliques(all_cliques_affected.size());

    for (const auto clik: _junction_tree_.nodes())
      if (new_nodes_in_junction_tree.contains(clik)) T_mpd_cliques.insert(clik, clik);

    // parse all the separators of the junction tree and test those that are not
    // complete in the original graph
    std::vector< std::pair< NodeId, NodeId > > merged_cliques;

    HashTable< NodeId, bool > mark = T_mpd_cliques.map(false);

    for (const auto& elt: mark)
      if (!elt.second)
        _computeMaxPrimeMergings_(elt.first,
                                  elt.first,
                                  merged_cliques,
                                  mark,
                                  new_nodes_in_junction_tree);

    // compute the transitive closure of merged_cliques. This one will contain
    // pairs (X,Y) indicating that clique X must be merged with clique Y.
    // Actually clique X will be inserted into clique Y.
    for (unsigned int i = 0; i < merged_cliques.size(); ++i) {
      if (T_mpd_cliques.exists(merged_cliques[i].second))
        T_mpd_cliques[merged_cliques[i].first] = T_mpd_cliques[merged_cliques[i].second];
      else
        T_mpd_cliques[merged_cliques[i].first] = _mps_of_clique_[merged_cliques[i].second];
    }

    // now we can create the max prime junction tree.

    // create a map translating the cliques' ids in the junction tree into
    // cliques' id in the T_mpd_ tree
    NodeProperty< NodeId > clique2MPS(T_mpd_cliques.size());

    // First, create the new cliques and create the corresponding
    // cliques_of_mps entries
    for (const auto& elt: T_mpd_cliques)
      if (elt.first == elt.second) {
        NodeId newId = _T_mpd_.addNode(_junction_tree_.clique(elt.second));
        clique2MPS.insert(elt.second, newId);
        std::vector< NodeId >& vect_of_cliques
           = _cliques_of_mps_.insert(newId, std::vector< NodeId >()).second;
        vect_of_cliques.push_back(elt.second);
      }

    // add to the cliques previously created the nodes of the cliques that were
    // merged into them and update the cliques_of_mps
    for (const auto& elt: T_mpd_cliques)
      if ((elt.first != elt.second) && (new_nodes_in_junction_tree.contains(elt.second))) {
        const NodeId idMPS = clique2MPS[elt.second];

        for (const auto node: _junction_tree_.clique(elt.first)) {
          try {
            _T_mpd_.addToClique(idMPS, node);
          } catch (DuplicateElement&) {}
        }

        _cliques_of_mps_[idMPS].push_back(elt.first);
      }

    // update the mps_of_node and the mps_of_clique
    for (const auto& elt: T_mpd_cliques) {
      const NodeId idMPS = clique2MPS[elt.second];
      _mps_of_clique_.insert(elt.first, idMPS);

      if (elt.first == elt.second)
        for (const auto node: _T_mpd_.clique(idMPS))
          _mps_of_node_[node].insert(idMPS);
    }

    // add the edges to the max prime subgraph tree
    for (const auto& elt: T_mpd_cliques) {
      NodeId clique = clique2MPS[elt.second];

      for (const auto othernode: _junction_tree_.neighbours(elt.first))
        if (T_mpd_cliques.exists(othernode)) {
          // here iter is linked to another node that has been created during
          // the triangulation
          NodeId otherClique = clique2MPS[T_mpd_cliques[othernode]];

          // avoid adding the same edge several times
          if (clique > otherClique) { _T_mpd_.addEdge(clique, otherClique); }
        } else {
          _T_mpd_.addEdge(clique, _mps_of_clique_[othernode]);
        }
    }
  }

  /// updates the triangulated graph using the modif list

  void IncrementalTriangulation::updateTriangulation() {
    if (!_require_update_) return;
    // the set of all the cliques that should be affected by the different
    // triangulations we will perform (one by connected component)
    NodeProperty< bool > all_cliques_affected(_junction_tree_.size());

    // we need to keep track of the new node ids that will be inserted
    // into  _junction_tree_. A priori, these should be equal to the ids
    // inserted into tmp2global_junction_tree. But, sometimes, some new nodes
    // are included into old nodes and, in this case, the translation in
    // tmp2global_junction_tree indicates the the new node inserted corresponds
    // to an old node. Here we wish to know this additional information
    NodeSet new_nodes_in_junction_tree;

    _updateJunctionTree_(all_cliques_affected, new_nodes_in_junction_tree);

    // now update the T_mpd so that it be coherent with the junction tree
    _updateMaxPrimeSubgraph_(all_cliques_affected, new_nodes_in_junction_tree);

    // reset the MPS that are affected
    _mps_affected_.clear();

    for (const auto node: _T_mpd_.nodes())
      _mps_affected_.insert(node, false);

    // remove all the structures used by the triangulation algorithm
    _triangulation_->clear();

    _require_update_ = false;
  }

  /// sets the graph to the empty graph

  void IncrementalTriangulation::clear() {
    _graph_.clear();
    _domain_sizes_.clear();
    _junction_tree_.clear();
    _T_mpd_.clear();
    _mps_of_node_.clear();
    _cliques_of_mps_.clear();
    _mps_of_clique_.clear();
    _mps_affected_.clear();
    _triangulation_->clear();
    _require_update_            = false;
    _require_elimination_order_ = false;
    _elimination_order_.clear();
    _reverse_elimination_order_.clear();
    _require_created_JT_cliques_ = false;
    _created_JT_cliques_.clear();
  }

  /// a collect algorithm to compute, for each node, one container JT's clique

  void IncrementalTriangulation::_collectJTCliques_(const NodeId          clique,
                                                    const NodeId          from,
                                                    NodeProperty< bool >& examined) {
    // apply collect to all the neighbours except from
    for (const auto otherclique: _junction_tree_.neighbours(clique))
      if (otherclique != from) _collectJTCliques_(otherclique, clique, examined);

    // get the nodes that belong to clique and not to from
    examined[clique] = true;

    const NodeSet& cliquenodes = _junction_tree_.clique(clique);

    if (from != clique) {
      const NodeSet& separator = _junction_tree_.separator(clique, from);

      for (const auto cli: cliquenodes)
        if (!separator.contains(cli)) _created_JT_cliques_.insert(cli, clique);
    } else {
      for (const auto cli: cliquenodes)
        _created_JT_cliques_.insert(cli, clique);
    }
  }

  /** @brief returns the Ids of the cliques of the junction tree created by the
   * elimination of the nodes */

  const NodeProperty< NodeId >& IncrementalTriangulation::createdJunctionTreeCliques() {
    // check if we already computed the containing cliques
    if (!_require_created_JT_cliques_) return _created_JT_cliques_;

    // we first we compute the junction tree
    updateTriangulation();

    _created_JT_cliques_.clear();

    _require_created_JT_cliques_ = false;

    if (_junction_tree_.size() == 0) { return _created_JT_cliques_; }

    // now we can use a collect algorithm to get the containing cliques
    NodeProperty< bool > examined = _junction_tree_.nodesProperty< bool >(false);

    for (const auto& elt: examined)
      if (!elt.second) _collectJTCliques_(elt.first, elt.first, examined);

    return _created_JT_cliques_;
  }

  /// returns a clique containing a given node of the triangulated graph

  NodeId IncrementalTriangulation::createdJunctionTreeClique(NodeId id) {
    createdJunctionTreeCliques();
    return _created_JT_cliques_[id];
  }

  /** @brief returns the Id of the maximal prime subgraph created by the
   * elimination of a given node during the triangulation process */

  NodeId IncrementalTriangulation::createdMaxPrimeSubgraph(const NodeId id) {
    // get the created junction tree clique and get its MPS
    return _mps_of_clique_[createdJunctionTreeClique(id)];
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

  void IncrementalTriangulation::_collectEliminationOrder_(const NodeId          node,
                                                           const NodeId          from,
                                                           NodeProperty< bool >& examined,
                                                           Idx&                  index) {
    // apply collect to all the neighbours except from
    for (const auto othernode: _junction_tree_.neighbours(node))
      if (othernode != from) _collectEliminationOrder_(othernode, node, examined, index);

    // get the nodes that belong to node and not to from
    examined[node] = true;

    const NodeSet& clique = _junction_tree_.clique(node);

    if (from != node) {
      const NodeSet& separator = _junction_tree_.separator(node, from);

      for (const auto cli: clique) {
        if (!separator.contains(cli)) {
          _elimination_order_[index] = cli;
          _reverse_elimination_order_.insert(cli, index);
          ++index;
        }
      }
    } else {
      for (const auto cli: clique) {
        _elimination_order_[index] = cli;
        _reverse_elimination_order_.insert(cli, index);
        ++index;
      }
    }
  }

  /// get a possible elimination ordering

  const std::vector< NodeId >& IncrementalTriangulation::eliminationOrder() {
    // check if we already computed the elimination order
    if (!_require_elimination_order_) return _elimination_order_;

    // to compute the elimination order, we first we compute the junction tree
    updateTriangulation();

    _elimination_order_.resize(_graph_.size());

    _reverse_elimination_order_.clear();

    _require_elimination_order_ = false;

    if (_junction_tree_.size() == Size(0)) { return _elimination_order_; }

    // now we can use a collect algorithm to get the elimination order
    Idx index = Idx(0);

    NodeProperty< bool > examined = _junction_tree_.nodesProperty< bool >(false);

    for (const auto& elt: examined)
      if (!elt.second) _collectEliminationOrder_(elt.first, elt.first, examined, index);

    return _elimination_order_;
  }

  /** @brief returns the number of a given node in the elimination order
   * (0 = first node eliminated) */

  Idx IncrementalTriangulation::eliminationOrder(const NodeId node) {
    if (!_graph_.existsNode(node)) { GUM_ERROR(NotFound, "the node " << node << " does not exist") }

    // compute the elimination order
    eliminationOrder();

    return _reverse_elimination_order_[node];
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
