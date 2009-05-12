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
 * @brief Class for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_DEFAULT_TRIANGULATION_H
#define GUM_DEFAULT_TRIANGULATION_H

#include <iostream>
#include <utility>
#include <sstream>
#include <limits>
#include <cmath>

#include <agrum/core/gum_utils.h>
#include <agrum/core/gumDebug.h>
#include <agrum/core/gumExceptions.h>
#include <agrum/gumNode.h>
#include <agrum/graphs/gumGraphElements.h>
#include <agrum/gumGraph.h>
#include <agrum/graphs/gumCliqueGraph.h>
#include <agrum/core/gumHeap.h>
#include <agrum/gumList.h>

#include <agrum/gumSimplicial.h>
#include <agrum/graphs/gumTriangulation.h>

/* ============================================================================== */
/* ===              BASIC TRIANGULATION ALGORITHM USED BY AGRUM             === */
/* ============================================================================== */
/** @class gumDefaultTriangulation
 * @brief The default triangulation algorithm used by aGrUM
 *
 * By default, this is the very class used by aGrUM for performing triangulations.
 * The algorithm used is the following:
 * # the graph passed in argument is completed by fill-ins until it becomes
 *   triangulated
 * # then an elimination tree is computed from this triangulated graph
 * # finally, a junction tree is derived from the elimination tree
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

class gumDefaultTriangulation : public gumTriangulation {
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
    explicit gumDefaultTriangulation( const gumUndiGraphTpl<gumNodeId,Edge>& graph,
                                      const gumHashTable<gumNodeId,unsigned int>& modal,
                                      bool minimality = false,
                                      double theRatio = GUM_QUASI_RATIO,
                                      double theThreshold = GUM_WEIGHT_THRESHOLD );

    // ==============================================================================
    /// basic constructor. initialize the triangulation
    /** Note that, in the graph passed in argument, the type of the edges differ
     * from gumEdge. However, the junction trees and triangulated graphs produced
     * by the triangulation algorithm will all have edges of type gumEdge. */
    // ==============================================================================
    template <typename Arc>
    explicit gumDefaultTriangulation( const gumDiGraphTpl<gumNodeId,Arc>& graph,
                                      const gumHashTable<gumNodeId,unsigned int>& modal,
                                      bool minimality = false,
                                      double theRatio = GUM_QUASI_RATIO,
                                      double theThreshold = GUM_WEIGHT_THRESHOLD );

    // ==============================================================================
    /// basic constructor. initialize the triangulation
    /** Note that, in the graph passed in argument, the type of the arcs/edges differ
     * from gumEdge. However, the junction trees and triangulated graphs produced
     * by the triangulation algorithm will all have edges of type gumEdge. */
    // ==============================================================================
    template <typename Edge, typename Arc>
    explicit gumDefaultTriangulation( const gumMixedGraphTpl<gumNodeId,Edge,Arc>& graph,
                                      const gumHashTable<gumNodeId,unsigned int>& modal,
                                      bool minimality = false,
                                      double theRatio = GUM_QUASI_RATIO,
                                      double theThreshold = GUM_WEIGHT_THRESHOLD );

    // ==============================================================================
    /// default constructor: initialize the triangulation for an empty graph
    // ==============================================================================
    gumDefaultTriangulation( bool minimality = false,
                             double theRatio = GUM_QUASI_RATIO,
                             double theThreshold = GUM_WEIGHT_THRESHOLD );

    // ==============================================================================
    /// copy constructor (deep copy)
    // ==============================================================================
    gumDefaultTriangulation( const gumDefaultTriangulation<gumNodeId>& from );

    // ==============================================================================
    /// destructor
    // ==============================================================================
    ~gumDefaultTriangulation();

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// returns the fill-ins added by the triangulation algorithm
    // ==============================================================================
    const gumEdgeList<gumEdge>& fillIns();

    // ==============================================================================
    /// returns an elimination ordering compatible with the triangulated graph
    // ==============================================================================
    const std::vector<gumNodeId>& eliminationOrder();

    // ==============================================================================
    /** @brief returns the number of a given node in the elimination order
     * (0 = first node eliminated) */
    // ==============================================================================
    unsigned int eliminationOrder( const gumNodeId& );

    // ==============================================================================
    /// returns the triangulated graph
    // ==============================================================================
    const gumUndiGraph& triangulatedGraph();

    // ==============================================================================
    /// returns the elimination tree of a compatible ordering
    // ==============================================================================
    const gumUndiGraph& eliminationTree();

    // ==============================================================================
    /// returns a compatible junction tree
    // ==============================================================================
    const gumUndiGraph& junctionTree();

    // ==============================================================================
    /** @brief returns the Id of the clique created by the
     * elimination of a given node during the triangulation process */
    // ==============================================================================
    const gumNodeId& createdClique( const gumNodeId& id );

    // ==============================================================================
    /// returns a junction tree of maximal prime subgraphs
    /** @warning Actually, the cliques of the junction tree are guarranteed to be
     * maximal prime subgraph of the original graph that was triangulated only if
     * the triangulation performed is minimal (in the sense that removing any edge in
     * the triangulated graph results in a nontriangulated graph). This can be
     * ensured by requiring minimality of the triangulation. */
    // ==============================================================================
    const gumUndiGraph& maxPrimeSubgraphTree();

    // ==============================================================================
    /** @brief returns the Id of the maximal prime subgraph created by the
     * elimination of a given node during the triangulation process */
    // ==============================================================================
    const gumNodeId& createdMaxPrimeSubgraph( const gumNodeId& id );

    // ==============================================================================
    /// reinitialize the graph to be triangulated to an empty graph
    // ==============================================================================
    void clear();

    // ==============================================================================
    /// initialize the triangulation algorithm for a new graph
    // ==============================================================================
    template <typename Edge>
    void setGraph( const gumUndiGraphTpl<gumNodeId,Edge>& theGraph,
                   const gumHashTable<gumNodeId,unsigned int>& modal );

    // ==============================================================================
    /// sets/unset the minimality requirement
    // ==============================================================================
    void setMinimalRequirement( bool );

    // ==============================================================================
    /// sets/unset the fill-ins storage in the standard triangulation procedure
    // ==============================================================================
    void setFillIns( bool );

    /// @}



    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator
    // ==============================================================================
    gumDefaultTriangulation<gumNodeId>& operator= ( const gumDefaultTriangulation<gumNodeId>& );

    /// @}


  private:
    /// the original graph (that which will be triangulated)
    gumUndiGraph __original_graph;

    /// the triangulated graph
    gumUndiGraph __triangulated_graph;

    /// the modalities of the nodes of the graph
    gumNodePROPERTY( float ) __log_modalities;

    /// the fill-ins added by the triangulation
    gumEdgeSet __fill_ins;

    /// the order in which nodes are eliminated by the algorithm
    std::vector<gumNodeId> __elim_order;

    /// the elimination order (access by gumNodeId)
    gumNodePROPERTY( unsigned int ) __reverse_elim_order;

    /// the cliques formed by the elimination of the nodes
    gumNodePROPERTY( gumNodeSet ) __elim_cliques;

    /// the elimination tree computed by the algorithm
    gumUndiGraph __elim_tree;

    /// the junction tree computed by the algorithm
    gumUndiGraph __junction_tree;

    /** @brief indicates which clique of the junction tree was created
     * by the elimination of a given node (the key of the table) */
    gumNodePROPERTY( gumNodeId ) __node_2_junction_clique;

    /// the maximal prime subgraph junction tree computed from the junction tree
    gumUndiGraph __max_prime_junction_tree;

    /** @brief indicates which clique of the max prime junction tree was created
     * by the elmination of a given node (the key of the table) */
    gumNodePROPERTY( gumNodeId ) __node_2_max_prime_clique;

    /// a boolean indicating whether we have parformed a triangulation
    bool __has_triangulation;

    /// a boolean indicating whether we have constructed the triangulated graph
    bool __has_triangulated_graph;

    /// a boolean indicating whether the elimination tree has been computed
    bool __has_elimination_tree;

    /// a boolean indicating whether the junction tree has been constructed
    bool __has_junction_tree;

    /// indicates whether a maximal prime subgraph junction tree has been constructed
    bool __has_max_prime_junction_tree;

    /// the ratio above which we consider nodes to be quasi simplicial
    float __quasi_ratio;

    /** @brief threshold under which almost and quasi simplicial nodes can be chosen
     * to be eliminated */
    float threshold;

    /// indicates whether the triangulation must be minimal
    bool __minimality_required;

    /** @brief a vector containing the set of fill-ins added after each node
     * elimination (used by recursive thinning) */
    std::vector< gumEdgeSet > __added_fill_ins;

    /// a boolean indicating if we want fill-ins list with the standard triangulation method
    bool __we_want_fill_ins;

    // fill-ins list of the standard triangulation method
    gumEdgeSet __fill_ins_list_standard;

    // ==============================================================================
    /// the function that performs the triangulation
    // ==============================================================================
    void __triangulate();

    // ==============================================================================
    /// returns an elimination tree from a triangulated graph
    // ==============================================================================
    void __computeEliminationTree();

    // ==============================================================================
    /// computes a junction tree from an elimination tree
    // ==============================================================================
    void __computeJunctionTree();

    // ==============================================================================
    /// computes the junction tree of the maximal prime subgraphs
    // ==============================================================================
    void __computeMaxPrimeJunctionTree();

    // ==============================================================================
    /// removes redondant fill-ins and compute proper elimination cliques and order
    // ==============================================================================
    void __computeRecursiveThinning();

    // ==============================================================================
    /// used for computing the junction tree of the maximal prime subgraphs
    // ==============================================================================
    void __computeMaxPrimeMergings( const gumNodeId node, const gumNodeId from,
                                  std::vector< gumEdgeSet >& merged_cliques,
                                  gumNodeSet& mark ) const;
};






/* ============================================================================== */
/* ===             CLASS THAT PERFORMS INCREMENTAL TRIANGULATIONS             === */
/* ============================================================================== */
/** @class gumIncrementalTriangulation
 * @brief Class that performs incremental triangulations
 */
/* ============================================================================== */
template <typename gumNodeId, template <typename> class Triangulation>
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
    gumIncrementalTriangulation( const gumUndiGraphTpl<gumNodeId,Edge>& theGraph,
                                 const gumHashTable<gumNodeId,unsigned int>& modal );

    // ==============================================================================
    /// default constructor: initialize the triangulation with en empty graph
    // ==============================================================================
    gumIncrementalTriangulation();

    // ==============================================================================
    /// copy operator
    // ==============================================================================
    gumIncrementalTriangulation
    ( const gumIncrementalTriangulation<gumNodeId,Triangulation>& from );

    // ==============================================================================
    /// destructor
    // ==============================================================================
    ~gumIncrementalTriangulation();

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// updates the triangulated graph using the modif list
    // ==============================================================================
    void updateTriangulation();

    // ==============================================================================
    /// adds a new node to the graph
    // ==============================================================================
    void insertNode( const gumRefPtr<gumNodeId>& node, unsigned int modal );

    // ==============================================================================
    /// removes a node from the graph (the join tree may need a triangulation update)
    // ==============================================================================
    void eraseNode( const gumNodeId& node );

    // ==============================================================================
    /// removes a node from the graph (the join tree may need a triangulation update)
    // ==============================================================================
    void eraseNode( const gumNodeId& node );

    // ==============================================================================
    /// adds a new edge to the graph (the join tree may need a triangulation update)
    // ==============================================================================
    void insertEdge( const gumNodeId& X, const gumNodeId& Y );

    // ==============================================================================
    /// removes an edge from the graph (the join tree may need a retriangulation)
    // ==============================================================================
    void eraseEdge( const gumNodeId& node1, const gumNodeId& node2 );

    // ==============================================================================
    /// returns a junction tree corresponding to the current graph
    // ==============================================================================
    const gumUndiGraph& junctionTree();

    // ==============================================================================
    /// returns the junction tree of the maximal prime subgraphs
    // ==============================================================================
    const gumUndiGraph& maxPrimeSubgraphTree();

    // ==============================================================================
    /// sets the graph to the empty graph
    // ==============================================================================
    void clear();

    // ==============================================================================
    /// returns a clique containing a given node of the triangulated graph
    /**
    * @throw gumNotFound
    */
    // ==============================================================================
    gumNodeId getContainerClique( const gumNodeId& id ) const ;

    // ==============================================================================
    /// changes the current graph
    // ==============================================================================
    template <typename Edge>
    void setGraph( const gumUndiGraphTpl<gumNodeId,Edge>& theGraph,
                   const gumHashTable<gumNodeId,unsigned int>& modal );

    // ==============================================================================
    /// returns the triangulation algorithm (useful for fine tuning it)
    // ==============================================================================
    Triangulation<gumNodeId>& getTriangulationAlgo() const ;

    /// @}



    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator
    // ==============================================================================
    gumIncrementalTriangulation<gumNodeId,Triangulation>& operator=
    ( const gumIncrementalTriangulation<gumNodeId,Triangulation>& from );

    /// @}



  private:
    /// the graph that needs be triangulated
    gumUndiGraph graph;

    /// the modalities of the nodes
    gumHashTable<gumNodeId,unsigned int> modalities;

    /// the junction tree computed so far
    gumUndiGraph junction_tree;

    /// the maximal prime subgraph tree
    gumUndiGraph T_mpd;

    /// for each node in graph, store the MPS containing the node
    gumHashTable< gumNodeId,gumList<gumNodeId> > mps_of_node;

    /// indicate for each MPS its set of cliques in the junction tree
    gumHashTable< gumNodeId,std::vector<gumNodeId> > cliques_of_mps;

    /// indicate for each clique the MPS it belongs to
    gumHashTable< gumNodeId,gumNodeId > mps_of_clique;

    /// an Id factory to create new nodes in the junction tree and in the T_mpd
    gumIdFactory idFactory;

    /// the set of MPS affected by a new triangulation
    gumHashTable<gumNodeId,bool> mps_affected;

    /// the triangulation algorithm that will be used incremantally
    Triangulation<gumNodeId> triangulation;

    /// a boolean indicating whether the triangulation need be updated
    bool require_update;



    // ==============================================================================
    /// mark the mps affected by the deletion of a given edge
    // ==============================================================================
    void markAffectedMPSsByRemoveLink( const gumNodeId& My, const gumNodeId& Mz,
                                       const std::pair<gumNodeId,gumNodeId>& edge );

    // ==============================================================================
    /// mark the mps affected by the insertion of a new edge
    // ==============================================================================
    int markAffectedMPSsByAddLink( const gumNodeId& My, const gumNodeId& Mz,
                                   const gumNodeId& X, const gumNodeId& Y );

    // ==============================================================================
    /// remove a given node from the T_mpd structure
    // ==============================================================================
    void performRemoveNode( const gumNodeId& node, const gumNodeId& My, const gumNodeId& Mz );

    // ==============================================================================
    /// adds a new node to T_mpd, the graph and the clique graph
    // ==============================================================================
    void performAddNode( const gumNodeId& node );

    // ==============================================================================
    /// set-up the connected subgraph that needs be retriangulated
    // ==============================================================================
    void setUpConnectedTriangulation( gumNodeId Mx, gumNodeId Mfrom,
                                      gumUndiGraph& theGraph,
                                      std::vector<gumEdge>& notAffectedneighbourClique,
                                      gumHashTable<gumNodeId,bool>& cliques_affected );

    // ==============================================================================
    /// used for computing the junction tree of the maximal prime subgraphs
    // ==============================================================================
    void computeMaxPrimeMergings( const gumNodeId& node, const gumNodeId& from,
                                  std::vector<std::pair<gumNodeId,gumNodeId> >& merged_cliques,
                                  gumHashTable<gumNodeId,bool>& mark,
                                  const gumNodeId& max_old_id ) const;

    void check();
};


/// always include the template implementations
#include "gumTriangulation.tcc"



#endif /* GUM_DEFAULT_TRIANGULATION_H */
