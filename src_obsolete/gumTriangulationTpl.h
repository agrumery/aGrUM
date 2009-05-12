/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
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
 * @brief Class for computing triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_TRIANGULATION_H
#define GUM_TRIANGULATION_H

#include <iostream>
#include <utility>

#include <agrum/gum_utils.h>
#include <agrum/gumDebug.h>
#include <agrum/gumExceptions.h>
#include <agrum/gumNode.h>
#include <agrum/gumGraphElements.h>
#include <agrum/gumGraph.h>
#include <agrum/gumCliqueGraphTpl.h>
#include <agrum/utils/gumHeap.h>
#include <agrum/gumList.h>


#define GUM_QUASI_RATIO      0.95
#define GUM_WEIGHT_THRESHOLD 0.0



/* ============================================================================== */
/* ===   CLASS FOR RETRIEVING SIMPLICIAL, ALMOST AND QUASI SIMPLICIAL NODES   === */
/* ============================================================================== */
/** @class gumSimplicialSet
 * @brief Class enabling fast retrieval of (quasi) simplicial nodes
 */
/* ============================================================================== */
template <typename Node> class gumSimplicialSet {
public:
  // ##############################################################################
  /// @name Constructors / Destructors
  // ##############################################################################
  /// @{

  // ==============================================================================
  /// constructor. initialize the simplicial set w.r.t. a given graph
  // ==============================================================================
  explicit gumSimplicialSet (gumUndiGraphTpl<Node,gumEdge>& graph,
                             const gumHashTable<gumId,float>& log_modalities,
                             gumHashTable<gumId,float>& log_weights,
                             float theRatio = GUM_QUASI_RATIO,
                             float theThreshold = GUM_WEIGHT_THRESHOLD);

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  gumSimplicialSet (const gumSimplicialSet<Node>& from);

  // ==============================================================================
  /// destructor
  // ==============================================================================
  ~gumSimplicialSet ();

  /// @}



  // ##############################################################################
  /// @name Accessors / Modifiers
  // ##############################################################################
  /// @{

  // ==============================================================================
  /// adds the necessary edges so that node 'id' and its neighbours form a clique
  // ==============================================================================
  void makeClique (const gumId& id);

  // ==============================================================================
  /// removes a node and its adjacent edges from the underlying graph
  /** The node should form a clique with its neighbours.
   * @throw gumNotFound exception is thrown if the node cannot be found
   * in the graph or if it is not a clique. */
  // ==============================================================================
  void eraseClique (const gumId& id);

  // ==============================================================================
  /// removes a node and its adjacent edges from the underlying graph
  /** @throw gumNotFound exception is thrown if the node cannot be found
   * in the graph. */
  // ==============================================================================
  void eraseNode (const gumId& id);

  // ==============================================================================
  /// removes an edge from the graph and recomputes the simplicial set
  // ==============================================================================
  void eraseEdge (const gumEdge& edge);

  // ==============================================================================
  /// adds a new edge to the graph and recomputes the simplicial set
  // ==============================================================================
  void insertEdge (const gumEdge& edge);

  // ==============================================================================
  /// indicates whether a given node is a simplicial node
  /** A simplicial node is a node such that the latter and its neighbours form
   * a clique. */
  // ==============================================================================
  const bool isSimplicial (const gumId& id);

  // ==============================================================================
  /// indicates whether there exists a simplicial node
  /** A simplicial node is a node such that the latter and its neighbours form
   * a clique. */
  // ==============================================================================
  const bool hasSimplicialNode ();

  // ==============================================================================
  /// indicates whether there exists an almost simplicial node
  // ==============================================================================
  const bool hasAlmostSimplicialNode ();

  // ==============================================================================
  /// indicates whether there exists a quasi simplicial node
  // ==============================================================================
  const bool hasQuasiSimplicialNode ();

  // ==============================================================================
  /// indicates whether there exists a simplicial node
  // ==============================================================================
  const bool hasAllSimplicialNodes ();

  // ==============================================================================
  /// indicates whether there exists a simplicial node
  /** A simplicial node is a node such that the latter and its neighbours form
   * a clique. */
  // ==============================================================================
  const gumId& getBestSimplicialNode ();

  // ==============================================================================
  /// returns all the simplicial nodes
  /** @warning the booleans of the hashtable are meaningless. All the elements in
   * the hashtable are simplicial. The hashtable structure is used to determine
   * quickly whether a node is simplicial or not. */
  // ==============================================================================
  const gumHashTable<gumId,bool>& getAllSimplicialNodes ();

  // ==============================================================================
  /// gets an almost simplicial node
  // ==============================================================================
  const gumId& getBestAlmostSimplicialNode ();

  // ==============================================================================
  /// returns all the almost simplicial nodes
  // ==============================================================================
  const gumPriorityQueue<gumId,float>& getAllAlmostSimplicialNodes ();

  // ==============================================================================
  /// gets a quasi simplicial node
  // ==============================================================================
  const gumId& getBestQuasiSimplicialNode ();

  // ==============================================================================
  /// returns all the quasi simplicial nodes
  // ==============================================================================
  const gumPriorityQueue<gumId,float>& getAllQuasiSimplicialNodes ();

  // ==============================================================================
  /// sets/unset the fill-ins storage in the standard triangulation procedure
  // ==============================================================================
  void setFillIns(bool);

  // ==============================================================================
  /// gets a quasi simplicial node
  // ==============================================================================
  const gumEdgeList<gumEdge>& fillIns() const;

  /// @}



private:
  /// the graph on which we perform the simplicial computations
  gumUndiGraphTpl<Node,gumEdge>& graph;

  /// the weights of the nodes (i.e., weight of their clique)
  gumHashTable<gumId,float>& log_weights;

  /// the log of the modalities of the nodes
  const gumHashTable<gumId,float>& log_modalities;

  /// the list of the simplicial nodes
  gumHashTable<gumId,bool> simplicial_nodes;

  /// a queue of the almost simplicial nodes ordered by increasing node weight
  gumPriorityQueue<gumId,float> almost_simplicial_nodes;

  /// a queue of the quasi simplicial nodes ordered by increasing node weight
  gumPriorityQueue<gumId,float> quasi_simplicial_nodes;

  /** @brief indicates for each node to which list (simplicial, almost simplicial,
   * quasi simplicial) it belongs */
  enum gumBelong { GUM_SIMPLICIAL,
                   GUM_ALMOST_SIMPLICIAL,
                   GUM_QUASI_SIMPLICIAL,
                   GUM_NO_LIST };
  gumHashTable<gumId,gumBelong> containing_list;

  /// for each edge, keep track of the number of triangles passing through this egde
  gumHashTable<std::pair<gumId,gumId>, unsigned int> nb_triangles;

  /// for each node, the number of pairs of adjacent neighbours
  gumHashTable<gumId, unsigned int> nb_adjacent_neighbours;

  /// the current tree width
  float log_tree_width;

  /** @brief for a given node, if the number of pairs of neighbours that are
   * adjacent / the number of adjacent neighbours in a clique is greater than the
   * quasi ratio, then the node should belong the quasi simplicial list */
  float quasi_ratio;

  /** @brief quasi and almost simplicial nodes may not be elminated unless their
   * weight is lower than (1 + threshold) * tree_width */
  float log_threshold;

  /// the set of nodes that have potentially changed of status
  gumHashTable<gumId,bool> changed_status;


  // ==============================================================================
  /// put node id in the correct simplicial/almost simplicial/quasi simplicial list
  // ==============================================================================
  void updateList (gumId id);

  /// a boolean indicating if we want fill-ins list with the standard triangulation method
  bool we_want_fill_ins;

  /// fill-ins list
  gumEdgeList<gumEdge> fill_ins_list;
};






/* ============================================================================== */
/* ===                BASIC INTERFACE TO PERFORM TRIANGULATIONS               === */
/* ============================================================================== */
/** @class gumTriangulation
 * @brief Interface for all triangulation methods
 */
/* ============================================================================== */
template <typename Node> struct gumTriangulation {
  // ==============================================================================
  /// returns the fill-ins added by the triangulation algorithm
  // ==============================================================================
  virtual const gumEdgeList<gumEdge>& getFillIns () = 0;

  // ==============================================================================
  /// returns an elimination ordering compatible with the triangulated graph
  // ==============================================================================
  virtual const std::vector<gumId>& eliminationOrder () = 0;

  // ==============================================================================
  /** @brief returns the number of a given node in the elimination order
   * (0 = first node eliminated) */
  // ==============================================================================
  virtual unsigned int eliminationOrder (const gumId&) = 0;

  // ==============================================================================
  /// returns the triangulated graph
  // ==============================================================================
  virtual const gumUndiGraphTpl<Node,gumEdge>& triangulatedGraph () = 0;

  // ==============================================================================
  /// returns the elimination tree of a compatible ordering
  // ==============================================================================
  virtual const gumCliqueGraphTpl<gumNode,gumEdge,Node>& eliminationTree () = 0;

  // ==============================================================================
  /// returns a compatible junction tree
  // ==============================================================================
  virtual const gumCliqueGraphTpl<gumNode,gumEdge,Node>& junctionTree () = 0;

  // ==============================================================================
  /** @brief returns the Id of the clique created by the
   * elimination of a given node during the triangulation process */
  // ==============================================================================
  virtual const gumId& createdClique (const gumId& id) = 0;

  // ==============================================================================
  /// returns a junction tree of maximal prime subgraphs
  /** @warning Actually, the cliques of the junction tree are guarranteed to be
   * maximal prime subgraph of the original graph that was triangulated only if
   * the triangulation performed is minimal (in the sense that removing any edge in
   * the triangulated graph results in a nontriangulated graph). This can be
   * ensured by requiring minimality of the triangulation. */
  // ==============================================================================
  virtual const gumCliqueGraphTpl<gumNode,gumEdge,Node>& maxPrimeSubgraphTree() = 0;

  // ==============================================================================
  /** @brief returns the Id of the maximal prime subgraph created by the
   * elimination of a given node during the triangulation process */
  // ==============================================================================
  virtual const gumId& createdMaxPrimeSubgraph (const gumId& id) = 0;

  // ==============================================================================
  /// reinitialize the graph to be triangulated to an empty graph
  // ==============================================================================
  virtual void clear () = 0;

  // ==============================================================================
  /// initialize the triangulation algorithm for a new graph
  // ==============================================================================
  template <typename Edge>
  void setGraph (const gumUndiGraphTpl<Node,Edge>& theGraph,
                 const gumHashTable<gumId,unsigned int>& modal);

  // ==============================================================================
  /// destructor
  // ==============================================================================
  virtual ~gumTriangulation () { }


};






/* ============================================================================== */
/* ===              DEFAULT TRIANGULATION ALGORITHM USED BY AGRUM             === */
/* ============================================================================== */
/** @class gumDefaultTriangulation
 * @brief The default triangulation algorithm used by aGrUM
 *
 * By default, this is the very class used by aGrUM for performing triangulations.
 * The algorithm used is the following:
 * # the graph passed in argument is completed by fill-ins until it becomes
 *   triangulated
 * # then an elimination tree is computed from this triangulated graph
 * # finally, a junction tree is dervied from the elimination tree
 *
 * The triangulation step first tries to remove simplicial nodes, that is, nodes
 * that belong to only one clique. Then almost simplicial nodes of low width are
 * removed (almost simplicial nodes are nodes such that all but one of their
 * neighbours form a clique. Then quasi simplicial nodes are removed, that is, nodes
 * such that the ratio of the number of fill-ins to add to form a clique by the
 * number of edges in a clique is small. Then nodes that create cliques of small
 * weight are removed.
 *
 * The transformation from the elimination tree to the join tree is performed
 * bottom-up. Each time a node of the elimination tree is identified to be a
 * sub-clique, it is removed and all of its parents but one are linked to the
 * latter. The identification of sub-cliques is very fast (comparison of 2 ints).
 */
/* ============================================================================== */
template <typename Node> class gumDefaultTriangulation :
  public gumTriangulation<Node> {
public:
  // ##############################################################################
  /// @name Constructors / Destructors
  // ##############################################################################
  /// @{

  // ==============================================================================
  /// basic constructor. initialize the triangulation
  /** Note that, in the graph passed in argument, the type of the edges may differ
   * from gumEdge. However, the junction trees and triangulated graphs produced
   * by the triangulation algorithm will all have edges of type gumEdge. */
  // ==============================================================================
  template <typename Edge>
  explicit gumDefaultTriangulation (const gumUndiGraphTpl<Node,Edge>& graph,
                                    const gumHashTable<gumId,unsigned int>& modal,
                                    bool minimality = false,
                                    double theRatio = GUM_QUASI_RATIO,
                                    double theThreshold = GUM_WEIGHT_THRESHOLD);

  // ==============================================================================
  /// basic constructor. initialize the triangulation
  /** Note that, in the graph passed in argument, the type of the edges differ
   * from gumEdge. However, the junction trees and triangulated graphs produced
   * by the triangulation algorithm will all have edges of type gumEdge. */
  // ==============================================================================
  template <typename Arc>
  explicit gumDefaultTriangulation (const gumDiGraphTpl<Node,Arc>& graph,
                                    const gumHashTable<gumId,unsigned int>& modal,
                                    bool minimality = false,
                                    double theRatio = GUM_QUASI_RATIO,
                                    double theThreshold = GUM_WEIGHT_THRESHOLD);

  // ==============================================================================
  /// basic constructor. initialize the triangulation
  /** Note that, in the graph passed in argument, the type of the arcs/edges differ
   * from gumEdge. However, the junction trees and triangulated graphs produced
   * by the triangulation algorithm will all have edges of type gumEdge. */
  // ==============================================================================
  template <typename Edge, typename Arc>
  explicit gumDefaultTriangulation (const gumMixedGraphTpl<Node,Edge,Arc>& graph,
                                    const gumHashTable<gumId,unsigned int>& modal,
                                    bool minimality = false,
                                    double theRatio = GUM_QUASI_RATIO,
                                    double theThreshold = GUM_WEIGHT_THRESHOLD);

  // ==============================================================================
  /// default constructor: initialize the triangulation for an empty graph
  // ==============================================================================
  gumDefaultTriangulation (bool minimality = false,
                           double theRatio = GUM_QUASI_RATIO,
                           double theThreshold = GUM_WEIGHT_THRESHOLD);

  // ==============================================================================
  /// copy constructor (deep copy)
  // ==============================================================================
  gumDefaultTriangulation (const gumDefaultTriangulation<Node>& from);

  // ==============================================================================
   /// destructor
  // ==============================================================================
  ~gumDefaultTriangulation ();

  /// @}



  // ##############################################################################
  /// @name Accessors / Modifiers
  // ##############################################################################
  /// @{

  // ==============================================================================
  /// returns the fill-ins added by the triangulation algorithm
  // ==============================================================================
  const gumEdgeList<gumEdge>& getFillIns ();

  // ==============================================================================
  /// returns an elimination ordering compatible with the triangulated graph
  // ==============================================================================
  const std::vector<gumId>& eliminationOrder ();

  // ==============================================================================
  /** @brief returns the number of a given node in the elimination order
   * (0 = first node eliminated) */
  // ==============================================================================
  unsigned int eliminationOrder (const gumId&);

  // ==============================================================================
  /// returns the triangulated graph
  // ==============================================================================
  const gumUndiGraphTpl<Node,gumEdge>& triangulatedGraph ();

  // ==============================================================================
  /// returns the elimination tree of a compatible ordering
  // ==============================================================================
  const gumCliqueGraphTpl<gumNode,gumEdge,Node>& eliminationTree ();

  // ==============================================================================
  /// returns a compatible junction tree
  // ==============================================================================
  const gumCliqueGraphTpl<gumNode,gumEdge,Node>& junctionTree ();

  // ==============================================================================
  /** @brief returns the Id of the clique created by the
   * elimination of a given node during the triangulation process */
  // ==============================================================================
  const gumId& createdClique (const gumId& id);

  // ==============================================================================
  /// returns a junction tree of maximal prime subgraphs
  /** @warning Actually, the cliques of the junction tree are guarranteed to be
   * maximal prime subgraph of the original graph that was triangulated only if
   * the triangulation performed is minimal (in the sense that removing any edge in
   * the triangulated graph results in a nontriangulated graph). This can be
   * ensured by requiring minimality of the triangulation. */
  // ==============================================================================
  const gumCliqueGraphTpl<gumNode,gumEdge,Node>& maxPrimeSubgraphTree ();

  // ==============================================================================
  /** @brief returns the Id of the maximal prime subgraph created by the
   * elimination of a given node during the triangulation process */
  // ==============================================================================
  const gumId& createdMaxPrimeSubgraph (const gumId& id);

  // ==============================================================================
  /// reinitialize the graph to be triangulated to an empty graph
  // ==============================================================================
  void clear ();

  // ==============================================================================
  /// initialize the triangulation algorithm for a new graph
  // ==============================================================================
  template <typename Edge>
  void setGraph (const gumUndiGraphTpl<Node,Edge>& theGraph,
                 const gumHashTable<gumId,unsigned int>& modal);

  // ==============================================================================
  /// sets/unset the minimality requirement
  // ==============================================================================
  void setMinimalRequirement (bool);

  // ==============================================================================
  /// sets/unset the fill-ins storage in the standard triangulation procedure
  // ==============================================================================
  void setFillIns(bool);

  /// @}



  // ##############################################################################
  /// @name Operators
  // ##############################################################################
  /// @{

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  gumDefaultTriangulation<Node>& operator= (const gumDefaultTriangulation<Node>&);

  /// @}


private:
  /// the original graph (that which will be triangulated)
  gumUndiGraphTpl<Node,gumEdge> original_graph;

  /// the triangulated graph
  gumUndiGraphTpl<Node,gumEdge> triangulated_graph;

  /// the modalities of the nodes of the graph
  gumHashTable<gumId,float> log_modalities;

  /// the fill-ins added by the triangulation
  gumEdgeList<gumEdge> fill_ins;

  /// the order in which nodes are eliminated by the algorithm
  std::vector<gumId> elim_order;

  /// the elimination order (access by gumId)
  gumHashTable<gumId, unsigned int> reverse_elim_order;

  /// the cliques formed by the elimination of the nodes
  gumHashTable< gumId, std::vector<gumId> > elim_cliques;

  /// the elimination tree computed by the algorithm
  gumCliqueGraphTpl<gumNode,gumEdge,Node> elim_tree;

  /// the junction tree computed by the algorithm
  gumCliqueGraphTpl<gumNode,gumEdge,Node> junction_tree;

  /** @brief indicates which clique of the junction tree was created
   * by the elimination of a given node (the key of the table) */
  gumHashTable<gumId,gumId> node_2_junction_clique;

  /// the maximal prime subgraph junction tree computed from the junction tree
  gumCliqueGraphTpl<gumNode,gumEdge,Node> max_prime_junction_tree;

  /** @brief indicates which clique of the max prime junction tree was created
   * by the elmination of a given node (the key of the table) */
  gumHashTable<gumId,gumId> node_2_max_prime_clique;

  /// a boolean indicating whether we have parformed a triangulation
  bool has_triangulation;

  /// a boolean indicating whether we have constructed the triangulated graph
  bool has_triangulated_graph;

  /// a boolean indicating whether the elimination tree has been computed
  bool has_elimination_tree;

  /// a boolean indicating whether the junction tree has been constructed
  bool has_junction_tree;

  /// indicates whether a maximal prime subgraph junction tree has been constructed
  bool has_max_prime_junction_tree;

  /// the ratio above which we consider nodes to be quasi simplicial
  float quasi_ratio;

  /** @brief threshold under which almost and quasi simplicial nodes can be chosen
   * to be eliminated */
  float threshold;

  /// indicates whether the triangulation must be minimal
  bool minimality_required;

  /** @brief a vector containing the set of fill-ins added after each node
   * elimination (used by recursive thinning) */
  std::vector< gumHashTable<std::pair<gumId,gumId>,bool> > added_fill_ins;

  /// a boolean indicating if we want fill-ins list with the standard triangulation method
  bool we_want_fill_ins;

  // fill-ins list of the standard triangulation method
  gumEdgeList<gumEdge> fill_ins_list_standard;

  // ==============================================================================
  /// the function that performs the triangulation
  // ==============================================================================
  void triangulate ();

  // ==============================================================================
  /// returns an elimination tree from a triangulated graph
  // ==============================================================================
  void computeEliminationTree ();

  // ==============================================================================
  /// computes a junction tree from an elimination tree
  // ==============================================================================
  void computeJunctionTree ();

  // ==============================================================================
  /// computes the junction tree of the maximal prime subgraphs
  // ==============================================================================
  void computeMaxPrimeJunctionTree ();

  // ==============================================================================
  /// removes redondant fill-ins and compute proper elimination cliques and order
  // ==============================================================================
  void computeRecursiveThinning ();

  // ==============================================================================
  /// used for computing the junction tree of the maximal prime subgraphs
  // ==============================================================================
  void computeMaxPrimeMergings
  (const gumId& node, const gumId& from,
   std::vector< std::pair<gumId,gumId> >& merged_cliques,
   gumHashTable<gumId,bool>& mark) const;
};






/* ============================================================================== */
/* ===             CLASS THAT PERFORMS INCREMENTAL TRIANGULATIONS             === */
/* ============================================================================== */
/** @class gumIncrementalTriangulation
 * @brief Class that performs incremental triangulations
 */
/* ============================================================================== */
template <typename Node, template <typename> class Triangulation>
class gumIncrementalTriangulation {
public:
  // ##############################################################################
  /// @name Constructors / Destructors
  // ##############################################################################
  /// @{

  // ==============================================================================
  /// constructor
  /** Note that, in the graph passed in argument, the type of the edges may differ
   * from gumEdge. However, the junction trees and triangulated graphs produced
   * by the triangulation algorithm will all have edges of type gumEdge. */
  // ==============================================================================
  template <typename Edge>
  gumIncrementalTriangulation (const gumUndiGraphTpl<Node,Edge>& theGraph,
                               const gumHashTable<gumId,unsigned int>& modal);

  // ==============================================================================
  /// default constructor: initialize the triangulation with en empty graph
  // ==============================================================================
  gumIncrementalTriangulation ();

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  gumIncrementalTriangulation
  (const gumIncrementalTriangulation<Node,Triangulation>& from);

  // ==============================================================================
  /// destructor
  // ==============================================================================
  ~gumIncrementalTriangulation ();

  /// @}



  // ##############################################################################
  /// @name Accessors / Modifiers
  // ##############################################################################
  /// @{

  // ==============================================================================
  /// updates the triangulated graph using the modif list
  // ==============================================================================
  void updateTriangulation ();

  // ==============================================================================
  /// adds a new node to the graph
  // ==============================================================================
  void insertNode (const gumRefPtr<Node>& node, unsigned int modal);

  // ==============================================================================
  /// removes a node from the graph (the join tree may need a triangulation update)
  // ==============================================================================
  void eraseNode (const gumId& node);

  // ==============================================================================
  /// removes a node from the graph (the join tree may need a triangulation update)
  // ==============================================================================
  void eraseNode (const Node& node);

  // ==============================================================================
  /// adds a new edge to the graph (the join tree may need a triangulation update)
  // ==============================================================================
  void insertEdge (const gumId& X, const gumId& Y);

  // ==============================================================================
  /// removes an edge from the graph (the join tree may need a retriangulation)
  // ==============================================================================
  void eraseEdge (const gumId& node1, const gumId& node2);

  // ==============================================================================
  /// returns a junction tree corresponding to the current graph
  // ==============================================================================
  const gumCliqueGraphTpl<gumNode,gumEdge,Node>& junctionTree ();

  // ==============================================================================
  /// returns the junction tree of the maximal prime subgraphs
  // ==============================================================================
  const gumCliqueGraphTpl<gumNode,gumEdge,Node>& maxPrimeSubgraphTree ();

  // ==============================================================================
  /// sets the graph to the empty graph
  // ==============================================================================
  void clear ();

  // ==============================================================================
  /// returns a clique containing a given node of the triangulated graph
  /**
  * @throw gumNotFound
  */
  // ==============================================================================
  gumId getContainerClique (const gumId& id) const ;

  // ==============================================================================
  /// changes the current graph
  // ==============================================================================
  template <typename Edge>
  void setGraph (const gumUndiGraphTpl<Node,Edge>& theGraph,
                 const gumHashTable<gumId,unsigned int>& modal);

  // ==============================================================================
  /// returns the triangulation algorithm (useful for fine tuning it)
  // ==============================================================================
  Triangulation<Node>& getTriangulationAlgo () const ;

  /// @}



  // ##############################################################################
  /// @name Operators
  // ##############################################################################
  /// @{

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  gumIncrementalTriangulation<Node,Triangulation>& operator=
    (const gumIncrementalTriangulation<Node,Triangulation>& from);

  /// @}



private:
  /// the graph that needs be triangulated
  gumUndiGraphTpl<Node,gumEdge> graph;

  /// the modalities of the nodes
  gumHashTable<gumId,unsigned int> modalities;

  /// the junction tree computed so far
  gumCliqueGraphTpl<gumNode,gumEdge,Node> junction_tree;

  /// the maximal prime subgraph tree
  gumCliqueGraphTpl<gumNode,gumEdge,Node> T_mpd;

  /// for each node in graph, store the MPS containing the node
  gumHashTable< gumId,gumList<gumId> > mps_of_node;

  /// indicate for each MPS its set of cliques in the junction tree
  gumHashTable< gumId,std::vector<gumId> > cliques_of_mps;

  /// indicate for each clique the MPS it belongs to
  gumHashTable< gumId,gumId > mps_of_clique;

  /// an Id factory to create new nodes in the junction tree and in the T_mpd
  gumIdFactory idFactory;

  /// the set of MPS affected by a new triangulation
  gumHashTable<gumId,bool> mps_affected;

  /// the triangulation algorithm that will be used incremantally
  Triangulation<Node> triangulation;

  /// a boolean indicating whether the triangulation need be updated
  bool require_update;



  // ==============================================================================
  /// mark the mps affected by the deletion of a given edge
  // ==============================================================================
  void markAffectedMPSsByRemoveLink (const gumId& My, const gumId& Mz,
                                     const std::pair<gumId,gumId>& edge);

  // ==============================================================================
  /// mark the mps affected by the insertion of a new edge
  // ==============================================================================
  int markAffectedMPSsByAddLink (const gumId& My, const gumId& Mz,
                                 const gumId& X, const gumId& Y);

  // ==============================================================================
  /// remove a given node from the T_mpd structure
  // ==============================================================================
  void performRemoveNode (const gumId& node, const gumId& My, const gumId& Mz);

  // ==============================================================================
  /// adds a new node to T_mpd, the graph and the clique graph
  // ==============================================================================
  void performAddNode (const Node& node);

  // ==============================================================================
  /// set-up the connected subgraph that needs be retriangulated
  // ==============================================================================
  void setUpConnectedTriangulation (gumId Mx, gumId Mfrom,
                                    gumUndiGraphTpl<Node,gumEdge>& theGraph,
                                    std::vector<gumEdge>& notAffectedneighbourClique,
                                    gumHashTable<gumId,bool>& cliques_affected);

  // ==============================================================================
  /// used for computing the junction tree of the maximal prime subgraphs
  // ==============================================================================
  void computeMaxPrimeMergings(const gumId& node, const gumId& from,
                               std::vector<std::pair<gumId,gumId> >& merged_cliques,
                               gumHashTable<gumId,bool>& mark,
                               const gumId& max_old_id) const;

  void check();
};


/// always include the template implementations
#include "gumTriangulation.tcc"



#endif /* GUM_TRIANGULATION_H */
