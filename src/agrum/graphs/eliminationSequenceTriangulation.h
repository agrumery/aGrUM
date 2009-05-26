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
 * @brief Class for computing triangulations of graphs
 *
 * @author Jean-Philippe DUBUS
 */
#ifndef GUM_ELIMINATION_SEQUENCE_TRIANGULATION_H
#define GUM_ELIMINATION_SEQUENCE_TRIANGULATION_H

#include <iostream>
#include <utility>
#include <agrum/core/utils.h>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/core/heap.h>
#include <agrum/graphs/triangulation.h>


namespace gum {


  /* ============================================================================ */
  /* ===               BASIC INTERFACE TO PERFORM TRIANGULATIONS              === */
  /* ============================================================================ */
  /** @class EliminationSequenceTriangulation
   * @brief Triangulation with elimination sequence
   *
   * \ingroup graph_group
   *
   */
  /* ============================================================================ */
  class EliminationSequenceTriangulation : public Triangulation {

  public:

    // ============================================================================
    /// constructor
    // ============================================================================
    EliminationSequenceTriangulation(const UndiGraph& graph,
				     const std::vector<NodeId>& eliminationSequence);

    // ============================================================================
    /// destructor
    // ============================================================================
    virtual ~EliminationSequenceTriangulation();

    // ============================================================================
    /// copy constructor
    // ============================================================================
    EliminationSequenceTriangulation(const EliminationSequenceTriangulation& toCopy);

    // ============================================================================
    /// copy operator
    // ============================================================================
    EliminationSequenceTriangulation& operator=(const EliminationSequenceTriangulation& toCopy);



    // ============================================================================
    /// returns the fill-ins added by the triangulation algorithm
    // ============================================================================
    const EdgeSet& fillIns() ;
    
    // ============================================================================
    /// returns an elimination ordering compatible with the triangulated graph
    /// @todo : whu not a Sequence ?
    // ============================================================================
    const std::vector<NodeId>& eliminationOrder() ;

    // ============================================================================
    /** @brief returns the number of a given node in the elimination order
     * (0 = first node eliminated) */
    // ============================================================================
    unsigned int eliminationOrder( const NodeId ) ;

    // ============================================================================
    /// returns the triangulated graph
    // ============================================================================
    const UndiGraph& triangulatedGraph() ;

    // ============================================================================
    /// returns the elimination tree of a compatible ordering
    // ============================================================================
    const CliqueGraph& eliminationTree() ;

    // ============================================================================
    /// returns a compatible junction tree
    // ============================================================================
    const CliqueGraph& junctionTree() ;

    // ============================================================================
    /** @brief returns the Id of the clique created by the
     * elimination of a given node during the triangulation process */
    // ============================================================================
    NodeId createdClique( const NodeId id ) ;

    // ============================================================================
    /// returns a junction tree of maximal prime subgraphs
    /**
     * TODO 
     * @warning Actually, the cliques of the junction tree are guarranteed to be
     * maximal prime subgraph of the original graph that was triangulated only if
     * the triangulation performed is minimal (in the sense that removing any edge
     * in the triangulated graph results in a nontriangulated graph). This can be
     * ensured by requiring minimality of the triangulation. */
    // ============================================================================
    const CliqueGraph& maxPrimeSubgraphTree() ;

    // ============================================================================
    /**
     * TODO 
     * @brief returns the Id of the maximal prime subgraph created by the
     * elimination of a given node during the triangulation process */
    // ============================================================================
    NodeId createdMaxPrimeSubgraph( const NodeId id ) ;

    // ============================================================================
    /// reinitialize the graph to be triangulated to an empty graph
    // ============================================================================
    void clear() ;

    // ============================================================================
    /// initialize the triangulation algorithm for a new graph
    // ============================================================================
    void setGraph ( const UndiGraph& theGraph,
                            const Property<unsigned int>::onNodes& modal ) ;

    // ============================================================================
    /// virtual copy constructor
    /** Function clone returns a "true" copy of the current Triangulation class.
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Trianuglation pointers. See item 25 of the
     * more effective C++.*/
    // ============================================================================
    virtual Triangulation* copyFactory () const ;
    


  private:

    UndiGraph __graph;
    std::vector<NodeId> __eliminationSequence;
    Property<unsigned int>::onNodes __invertedElimination;
    EdgeSet* __fillIns;
    UndiGraph* __triangulatedGraph;
    CliqueGraph* __eliminationTree;
    CliqueGraph* __junctionTree;
    HashTable<NodeId, NodeId>* __createdCliqueTab;
    HashTable<NodeId, unsigned int> __cliqueOrder;

    void __computeEliminationTree();

    void __computeJunctionTree();

    // TODO
    CliqueGraph __maxPrimeSubgraph;

  };

#ifndef GUM_NO_INLINE
#include <agrum/graphs/eliminationSequenceTriangulation.inl>
#endif

} /* namespace gum */

  
#endif /* GUM_ELIMINATION_SEQUENCE_TRIANGULATION_H */
