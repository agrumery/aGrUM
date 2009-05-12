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
 * @brief Class for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_INCREMENTAL_TRIANGULATION_H
#define GUM_INCREMENTAL_TRIANGULATION_H

#include <iostream>
#include <utility>
#include <sstream>
#include <limits>
#include <cmath>

#include <agrum/core/utils.h>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/heap.h>
#include <agrum/core/list.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/simplicialSet.h>
#include <agrum/graphs/triangulation.h>


namespace gum {


  /* ============================================================================ */
  /* ===            CLASS THAT PERFORMS INCREMENTAL TRIANGULATIONS            === */
  /* ============================================================================ */
  /** @class IncrementalTriangulation
   * @brief Class that performs incremental triangulations
   */
  /* ============================================================================ */
  class IncrementalTriangulation : public Triangulation {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /// constructor
    /** Note that, in the graph passed in argument, the type of the edges may
     * differ from Edge. However, the junction trees and triangulated graphs
     * produced by the triangulation algorithm will all have edges of type Edge. */
    // ============================================================================
    IncrementalTriangulation( const Triangulation& triang_algo,
                              const UndiGraph& theGraph,
                              const Property<unsigned int>::onNodes& modal );
    
    // ============================================================================
    /// default constructor: initialize the triangulation with en empty graph
    // ============================================================================
    IncrementalTriangulation( const Triangulation& triang_algo );

    // ============================================================================
    /// copy operator
    // ============================================================================
    IncrementalTriangulation ( const IncrementalTriangulation& from );

    // ============================================================================
    /// destructor
    // ============================================================================
    ~IncrementalTriangulation ();

    /// @}



    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// updates the triangulated graph using the modif list
    // ============================================================================
    void updateTriangulation();

    // ============================================================================
    /// adds a new node to the graph
    // ============================================================================
    void insertNode( NodeId node, unsigned int modal );

    // ============================================================================
    /** @brief removes a node from the graph (the join tree may need a
     * triangulation update) */
    // ============================================================================
    void eraseNode( NodeId node );

    // ============================================================================
    /// adds a new edge to the graph (the join tree may need a triangulation update)
    // ============================================================================
    void insertEdge( NodeId X, NodeId Y );

    // ============================================================================
    /// removes an edge from the graph (the join tree may need a retriangulation)
    // ============================================================================
    void eraseEdge( NodeId node1, NodeId node2 );

    // ============================================================================
    /// returns the fill-ins added by the triangulation algorithm
    // ============================================================================
    const EdgeSet& fillIns() {GUM_ERROR(OperationNotAllowed,"Not implemented yet");};
    
    // ============================================================================
    /// returns an elimination ordering compatible with the triangulated graph
    /// @todo : whu not a Sequence ?
    // ============================================================================
    const std::vector<NodeId>& eliminationOrder() {GUM_ERROR(OperationNotAllowed,"Not implemented yet");};

    // ============================================================================
    /** @brief returns the number of a given node in the elimination order
     * (0 = first node eliminated) */
    // ============================================================================
    unsigned int eliminationOrder( const NodeId ) {GUM_ERROR(OperationNotAllowed,"Not implemented yet");};

    // ============================================================================
    /// returns the triangulated graph
    // ============================================================================
    const UndiGraph& triangulatedGraph() {GUM_ERROR(OperationNotAllowed,"Not implemented yet");};

    // ============================================================================
    /// returns the elimination tree of a compatible ordering
    // ============================================================================
    const CliqueGraph& eliminationTree() {GUM_ERROR(OperationNotAllowed,"Not implemented yet");};

    // ============================================================================
    /// returns a junction tree corresponding to the current graph
    // ============================================================================
    const CliqueGraph& junctionTree();

    // ============================================================================
    /** @brief returns the Id of the clique created by the
     * elimination of a given node during the triangulation process */
    // ============================================================================
    NodeId createdClique( const NodeId id ) {GUM_ERROR(OperationNotAllowed,"Not implemented yet");};

    // ============================================================================
    /// returns the junction tree of the maximal prime subgraphs
    // ============================================================================
    const CliqueGraph& maxPrimeSubgraphTree();

    // ============================================================================
    /** @brief returns the Id of the maximal prime subgraph created by the
     * elimination of a given node during the triangulation process */
    // ============================================================================
    NodeId createdMaxPrimeSubgraph( const NodeId id ) {GUM_ERROR(OperationNotAllowed,"Not implemented yet");};

     // ============================================================================
    /// sets the graph to the empty graph
    // ============================================================================
    void clear();

    // ============================================================================
    /// returns a clique containing a given node of the triangulated graph
    /** @throw NotFound */
    // ============================================================================
    NodeId getContainerClique( NodeId id ) const ;

    // ============================================================================
    /// changes the current graph
    // ============================================================================
    void setGraph( const UndiGraph& theGraph,
                   const Property<unsigned int>::onNodes& modal );

    // ============================================================================
    /// returns the triangulation algorithm (useful for fine tuning it)
    // ============================================================================
    const Triangulation& getTriangulationAlgo() const ;

    /// @}



    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// copy operator
    // ============================================================================
    IncrementalTriangulation& operator= ( const IncrementalTriangulation& from );

    // ============================================================================
    /// virtual copy constructor
    // ============================================================================
    IncrementalTriangulation* copyFactory () const  {GUM_ERROR(OperationNotAllowed,"Not implemented yet");};
    
    /// @}



  private:
    /// the graph that needs be triangulated
    UndiGraph __graph;

    /// the modalities of the nodes
    Property<unsigned int>::onNodes __modalities;

    /// the junction tree computed so far
    CliqueGraph __junction_tree;

    /// the maximal prime subgraph tree
    CliqueGraph __T_mpd;

    /// for each node in graph, store the MPS containing the node
    Property< List<NodeId> >::onNodes __mps_of_node;

    /// indicate for each MPS its set of cliques in the junction tree
    Property< std::vector<NodeId> >::onNodes __cliques_of_mps;

    /// indicate for each clique the MPS it belongs to
    Property<NodeId>::onNodes __mps_of_clique;

    /// the set of MPS affected by a new triangulation
     Property<bool>::onNodes __mps_affected;

    /// the triangulation algorithm that will be used incremantally
    Triangulation* __triangulation;

    /// a boolean indicating whether the triangulation need be updated
    bool __require_update;



    // ============================================================================
    /// mark the mps affected by the deletion of a given edge
    // ============================================================================
    void __markAffectedMPSsByRemoveLink( const NodeId& My, const NodeId& Mz,
                                         const std::pair<NodeId,NodeId>& edge );

    // ============================================================================
    /// mark the mps affected by the insertion of a new edge
    // ============================================================================
    int __markAffectedMPSsByAddLink( const NodeId& My, const NodeId& Mz,
                                     const NodeId& X, const NodeId& Y );

    // ============================================================================
    /// remove a given node from the T_mpd structure
    // ============================================================================
    void __performRemoveNode( const NodeId& node,
                              const NodeId& My, const NodeId& Mz );

    // ============================================================================
    /// adds a new node to T_mpd, the graph and the clique graph
    // ============================================================================
    void __performAddNode( const NodeId& node );

    // ============================================================================
    /// set-up the connected subgraph that needs be retriangulated
    // ============================================================================
    void __setUpConnectedTriangulation( NodeId Mx, NodeId Mfrom,
                                        UndiGraph& theGraph,
                                        std::vector<Edge>& notAffectedneighborClique,
                                        HashTable<NodeId,bool>& cliques_affected );

    // ============================================================================
    /// used for computing the junction tree of the maximal prime subgraphs
    // ============================================================================
    void __computeMaxPrimeMergings
    ( const NodeId& node, const NodeId& from,
      std::vector<std::pair<NodeId,NodeId> >& merged_cliques,
      HashTable<NodeId,bool>& mark,
      const NodeId& max_old_id ) const;

    void __check();
  };


} /* namespace gum */



#ifndef GUM_NO_INLINE
#include <agrum/graphs/incrementalTriangulation.inl>
#endif //GUM_NO_INLINE



#endif /* GUM_INCREMENTAL_TRIANGULATION_H */
