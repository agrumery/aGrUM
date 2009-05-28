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
 * @brief Class for fast retrielval of (quasi) simplicial nodes
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_SIMPLICIAL_SET_H
#define GUM_SIMPLICIAL_SET_H

#include <iostream>
#include <utility>
#include <agrum/core/utils.h>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/priorityQueue.h>
#include <agrum/graphs/cliqueGraph.h>

#define GUM_QUASI_RATIO      0.95
#define GUM_WEIGHT_THRESHOLD 0.0


namespace gum {


  /* =========================================================================== */
  /* ===  CLASS FOR RETRIEVING SIMPLICIAL, ALMOST AND QUASI SIMPLICIAL NODES === */
  /* =========================================================================== */
  /** @class SimplicialSet
   * @brief Class enabling fast retrieval of (quasi) simplicial nodes
   *
   * \ingroup graph_group
   *
   */
  /* =========================================================================== */
  class SimplicialSet {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    // ============================================================================
    /// constructor. initialize the simplicial set w.r.t. a given graph
    /** creates a class for managing the simplicial sets of a given undirected
     * graph. When we add or remove nodes or edges within this undirected graph,
     * the simplicial set updates its list of simplicial, almost simplicial
     * and quasi simplicial sets.
     *
     * @param graph The undirected graph the simplicial sets of which we are
     * interested in.
     * @param log_modalities The logarithm of the nodes modalities. This is used
     * for two different reasons: i) it enables to retrieve the simplicial nodes
     * that have the smallest number of modalities (useful for triangulations);
     * and ii) it enables to compute and update the weight of the cliques
     * containing the nodes (the weight of a clique is the sum of the
     * log_modalities of its nodes).
     * @param log_weights The logarithm of the wieghts of cliques.
     * @param theRatio Let L be the number of edges between neighbours of a
     * given node and let L' be the number of all the possible edges between these
     * neighbours (n * (n+1) / 2). If L/L' >= theRatio, then we consider that
     * the node and its neighbours quasi form a clique and, hence is a
     * quasi-simplicial node.
     * @param theThreshold for a safe triangulation (see Bodlaender), almost and
     * quasi-simplicial nodes should not be eliminated, unless their weight is
     * lower than the highest weight of the cliques created so far. Here, we
     * consider it safe if the wieight of a new clique is lower than
     * (1+theThreshold) * this highest weight. This enables so flexibility. */
    // ============================================================================
    explicit SimplicialSet( UndiGraph& graph,
                            const Property<float>::onNodes& log_modalities,
                            Property<float>::onNodes& log_weights,
                            float theRatio = GUM_QUASI_RATIO,
                            float theThreshold = GUM_WEIGHT_THRESHOLD );
    
    // ============================================================================
    /// copy constructor
    // ============================================================================
    SimplicialSet( const SimplicialSet& from );

    // ============================================================================
    /// destructor
    // ============================================================================
    ~SimplicialSet();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    // ============================================================================
    /// adds the necessary edges so that node 'id' and its neighbours form a clique
    // ============================================================================
    void makeClique( NodeId id );

    // ============================================================================
    /// removes a node and its adjacent edges from the underlying graph
    /** The node should form a clique with its neighbours.
     * @throw NotFound exception is thrown if the node cannot be found
     * in the graph or if it is not a clique. */
    // ============================================================================
    void eraseClique( NodeId id );

    // ============================================================================
    /// removes a node and its adjacent edges from the underlying graph
    /** @throw NotFound exception is thrown if the node cannot be found
     * in the graph. */
    // ============================================================================
    void eraseNode( NodeId id );

    // ============================================================================
    /// removes an edge from the graph and recomputes the simplicial set
    // ============================================================================
    void eraseEdge( const Edge& edge );

    // ============================================================================
    /// adds a new edge to the graph and recomputes the simplicial set
    // ============================================================================
    void insertEdge( const Edge& edge );

    // ============================================================================
    /// indicates whether a given node is a simplicial node
    /** A simplicial node is a node such that the latter and its neighbours form
     * a clique. */
    // ============================================================================
    bool isSimplicial( NodeId id );

    // ============================================================================
    /// indicates whether there exists a simplicial node
    /** A simplicial node is a node such that the latter and its neighbours form
     * a clique. */
    // ============================================================================
    bool hasSimplicialNode();

    // ============================================================================
    /// indicates whether there exists an almost simplicial node
    // ============================================================================
    bool hasAlmostSimplicialNode();

    // ============================================================================
    /// indicates whether there exists a quasi simplicial node
    // ============================================================================
    bool hasQuasiSimplicialNode();

    // ============================================================================
    /// returns the simplicial node with the lowest clique weight
    /** A simplicial node is a node such that the latter and its neighbours form
     * a clique. */
    // ============================================================================
    NodeId bestSimplicialNode();

    // ============================================================================
    /// returns all the simplicial nodes
    /** In the priority queue returned, the floats correspond to the weight of
     * the cliques the nodes belong to. */
    // ============================================================================
    const PriorityQueue<NodeId,float>& allSimplicialNodes();

    // ============================================================================
    /// gets the almost simplicial node with the lowest clique weight
    // ============================================================================
    NodeId bestAlmostSimplicialNode();

    // ============================================================================
    /// returns all the almost simplicial nodes
    /** In the priority queue returned, the floats correspond to the weight of
     * cliques formed by the nodes and their neighbours. */
    // ============================================================================
    const PriorityQueue<NodeId,float>& allAlmostSimplicialNodes();

    // ============================================================================
    /// gets a quasi simplicial node with the lowest clique weight
    // ============================================================================
    NodeId bestQuasiSimplicialNode();

    // ============================================================================
    /// returns all the quasi simplicial nodes
    /** In the priority queue returned, the floats correspond to the weight of
     * cliques formed by the nodes and their neighbours. */
    // ============================================================================
    const PriorityQueue<NodeId,float>& allQuasiSimplicialNodes();

    // ============================================================================
    /// sets/unset the fill-ins storage in the standard triangulation procedure
    // ============================================================================
    void setFillIns( bool );

    // ============================================================================
    /// gets a quasi simplicial node
    // ============================================================================
    const EdgeSet& fillIns() const;

    /// @}


  private:
    /// the graph on which we perform the simplicial computations
    UndiGraph& __graph;

    /// the weights of the nodes (i.e., weight of their clique)
    Property<float>::onNodes& __log_weights;

    /// the log of the modalities of the nodes
    const Property<float>::onNodes& __log_modalities;

    /// a queue of the simplicial nodes ordered by increasing node weight
    PriorityQueue<NodeId,float> __simplicial_nodes;

    /// a queue of the almost simplicial nodes ordered by increasing node weight
    PriorityQueue<NodeId,float> __almost_simplicial_nodes;

    /// a queue of the quasi simplicial nodes ordered by increasing node weight
    PriorityQueue<NodeId,float> __quasi_simplicial_nodes;

    /** @brief indicates for each node to which list (simplicial, almost
     * simplicial, quasi simplicial) it belongs */
    enum __Belong {
      GUM_SIMPLICIAL,
      GUM_ALMOST_SIMPLICIAL,
      GUM_QUASI_SIMPLICIAL,
      GUM_NO_LIST
    };
    Property<__Belong>::onNodes __containing_list;

    /** @brief for each edge, keep track of the number of triangles passing
     * through this egde */
    Property<unsigned int>::onEdges __nb_triangles;

    /// for each node, the number of pairs of adjacent neighbours
    Property<unsigned int>::onNodes __nb_adjacent_neighbours;

    /// the current tree width
    float __log_tree_width;

    /** @brief for a given node, if the number of pairs of neighbours that are
     * adjacent / the number of adjacent neighbours in a clique is greater than the
     * quasi ratio, then the node should belong the quasi simplicial list */
    float __quasi_ratio;

    /** @brief quasi and almost simplicial nodes may not be elminated unless their
     * weight is lower than (1 + threshold) * tree_width */
    float __log_threshold;

    /// the set of nodes that have potentially changed of status
    NodeSet __changed_status;

    /** @brief a boolean indicating if we want fill-ins list with the standard
     * triangulation method */
    bool __we_want_fill_ins;
    
    /// fill-ins list
    EdgeSet __fill_ins_list;


    // ============================================================================
    /** @brief put node id in the correct simplicial/almost simplicial/quasi
     * simplicial list */
    // ============================================================================
    void __updateList( NodeId id );

    // ============================================================================
    /// put all the nodes in their appropriate list
    // ============================================================================
    void __updateAllNodes();


  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/graphs/simplicialSet.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_SIMPLICIAL_SET_H */
