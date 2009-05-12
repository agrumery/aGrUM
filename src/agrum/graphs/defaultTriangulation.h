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
#ifndef GUM_DEFAULT_TRIANGULATION_H
#define GUM_DEFAULT_TRIANGULATION_H

#include <iostream>
#include <utility>
#include <sstream>
#include <limits>
#include <cmath>
#include <agrum/core/utils.h>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/graphs/simplicialSet.h>
#include <agrum/graphs/triangulation.h>


namespace gum {


  /* ============================================================================ */
  /* ===              BASIC TRIANGULATION ALGORITHM USED BY AGRUM             === */
  /* ============================================================================ */
  /** @class DefaultTriangulation
   * @brief The default triangulation algorithm used by aGrUM
   *
   * \ingroup graph_group
   *
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
   * neighbours form a clique. Then quasi simplicial nodes are removed, that is,
   * nodes such that the ratio of the number of fill-ins to add to form a clique
   * by the number of edges in a clique is small. Then nodes that create cliques
   * of small weight are removed.
   *
   * The transformation from the elimination tree to the join tree is performed
   * bottom-up. Each time a node of the elimination tree is identified to be a
   * sub-clique, it is removed and all of its parents but one are linked to the
   * latter. The identification of sub-cliques is very fast (comparison of 2 ints).
   */
  /* ============================================================================ */
  class DefaultTriangulation : public Triangulation {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    // ============================================================================
    /// basic constructor. initialize the triangulation
    /** Note that, in the graph passed in argument, the type of the edges may differ
     * from Edge. However, the junction trees and triangulated graphs produced
     * by the triangulation algorithm will all have edges of type Edge. */
    // ============================================================================
    explicit DefaultTriangulation( const UndiGraph& graph,
                                   const Property<unsigned int>::onNodes& modal,
                                   bool minimality = false,
                                   double theRatio = GUM_QUASI_RATIO,
                                   double theThreshold = GUM_WEIGHT_THRESHOLD );
    
    // ============================================================================
    /// basic constructor. initialize the triangulation
    /** Note that, in the graph passed in argument, the type of the edges differ
     * from Edge. However, the junction trees and triangulated graphs produced
     * by the triangulation algorithm will all have edges of type Edge. */
    // ============================================================================
    explicit DefaultTriangulation( const DiGraph& graph,
                                   const Property<unsigned int>::onNodes& modal,
                                   bool minimality = false,
                                   double theRatio = GUM_QUASI_RATIO,
                                   double theThreshold = GUM_WEIGHT_THRESHOLD );

    // ============================================================================
    /// basic constructor. initialize the triangulation
    /** Note that, in the graph passed in argument, the type of the arcs/edges differ
     * from Edge. However, the junction trees and triangulated graphs produced
     * by the triangulation algorithm will all have edges of type Edge. */
    // ============================================================================
    explicit DefaultTriangulation( const MixedGraph& graph,
                                   const Property<unsigned int>::onNodes& modal,
                                   bool minimality = false,
                                   double theRatio = GUM_QUASI_RATIO,
                                   double theThreshold = GUM_WEIGHT_THRESHOLD );

    // ============================================================================
    /// default constructor: initialize the triangulation for an empty graph
    // ============================================================================
    DefaultTriangulation( bool minimality = false,
                          double theRatio = GUM_QUASI_RATIO,
                          double theThreshold = GUM_WEIGHT_THRESHOLD );

    // ============================================================================
    /// copy constructor (deep copy)
    // ============================================================================
    DefaultTriangulation( const DefaultTriangulation& from );

    // ============================================================================
    /// destructor
    // ============================================================================
    ~DefaultTriangulation();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    // ============================================================================
    /// returns the fill-ins added by the triangulation algorithm
    // ============================================================================
    const EdgeSet& fillIns();
    
    // ============================================================================
    /// returns an elimination ordering compatible with the triangulated graph
    // ============================================================================
    const std::vector<NodeId>& eliminationOrder();

    // ============================================================================
    /** @brief returns the number of a given node in the elimination order
     * (0 = first node eliminated) */
    // ============================================================================
    unsigned int eliminationOrder( const NodeId );

    // ============================================================================
    /// returns the triangulated graph
    // ============================================================================
    const UndiGraph& triangulatedGraph();

    // ============================================================================
    /// returns the elimination tree of a compatible ordering
    // ============================================================================
    const CliqueGraph& eliminationTree();

    // ============================================================================
    /// returns a compatible junction tree
    // ============================================================================
    const CliqueGraph& junctionTree();

    // ============================================================================
    /** @brief returns the Id of the clique created by the
     * elimination of a given node during the triangulation process */
    // ============================================================================
    NodeId createdClique( const NodeId id );

    // ============================================================================
    /// returns a junction tree of maximal prime subgraphs
    /** @warning Actually, the cliques of the junction tree are guarranteed to be
     * maximal prime subgraph of the original graph that was triangulated only if
     * the triangulation performed is minimal (in the sense that removing any edge
     * in the triangulated graph results in a nontriangulated graph). This can be
     * ensured by requiring minimality of the triangulation. */
    // ============================================================================
    const CliqueGraph& maxPrimeSubgraphTree();

    // ============================================================================
    /** @brief returns the Id of the maximal prime subgraph created by the
     * elimination of a given node during the triangulation process */
    // ============================================================================
    NodeId createdMaxPrimeSubgraph( const NodeId id );

    // ============================================================================
    /// reinitialize the graph to be triangulated to an empty graph
    // ============================================================================
    void clear();

    // ============================================================================
    /// initialize the triangulation algorithm for a new graph
    // ============================================================================
    void setGraph( const UndiGraph& gr,
                   const Property<unsigned int>::onNodes& modal );

    // ============================================================================
    /// sets/unset the minimality requirement
    // ============================================================================
    void setMinimalRequirement( bool );

    // ============================================================================
    /// sets/unset the fill-ins storage in the standard triangulation procedure
    // ============================================================================
    void setFillIns( bool );

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{
    // ============================================================================
    /// copy operator
    // ============================================================================
    DefaultTriangulation& operator= ( const DefaultTriangulation& );
    
    // ============================================================================
    /// virtual copy constructor
    // ============================================================================
    virtual DefaultTriangulation* copyFactory () const;

    /// @}


  private:
    /// the original graph (that which will be triangulated)
    UndiGraph __original_graph;

    /// the triangulated graph
    UndiGraph __triangulated_graph;

    /// the modalities of the nodes of the graph
    Property<float>::onNodes __log_modalities;

    /// the fill-ins added by the triangulation
    EdgeSet __fill_ins;

    /// the order in which nodes are eliminated by the algorithm
    std::vector<NodeId> __elim_order;

    /// the elimination order (access by NodeId)
    Property<unsigned int>::onNodes __reverse_elim_order;

    /// the cliques formed by the elimination of the nodes
    Property<NodeSet>::onNodes __elim_cliques;

    /// the elimination tree computed by the algorithm
    CliqueGraph __elim_tree;

    /// the junction tree computed by the algorithm
    CliqueGraph __junction_tree;

    /** @brief indicates which clique of the junction tree was created
     * by the elimination of a given node (the key of the table) */
    Property<NodeId>::onNodes __node_2_junction_clique;

    /// the maximal prime subgraph junction tree computed from the junction tree
    CliqueGraph __max_prime_junction_tree;

    /** @brief indicates which clique of the max prime junction tree was created
     * by the elmination of a given node (the key of the table) */
    Property<NodeId>::onNodes __node_2_max_prime_clique;

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
    float __threshold;

    /// indicates whether the triangulation must be minimal
    bool __minimality_required;

    /** @brief a vector containing the set of fill-ins added after each node
     * elimination (used by recursive thinning) */
    std::vector< EdgeSet > __added_fill_ins;

    /** @brief a boolean indicating if we want fill-ins list with the standard
     * triangulation method */
    bool __we_want_fill_ins;
    
    // fill-ins list of the standard triangulation method
    EdgeSet __fill_ins_list_standard;

    // ============================================================================
    /// the function that performs the triangulation
    // ============================================================================
    void __triangulate();

    // ============================================================================
    /// returns an elimination tree from a triangulated graph
    // ============================================================================
    void __computeEliminationTree();

    // ============================================================================
    /// computes a junction tree from an elimination tree
    // ============================================================================
    void __computeJunctionTree();

    // ============================================================================
    /// computes the junction tree of the maximal prime subgraphs
    // ============================================================================
    void __computeMaxPrimeJunctionTree();

    // ============================================================================
    /// removes redondant fill-ins and compute proper elimination cliques and order
    // ============================================================================
    void __computeRecursiveThinning();

    // ============================================================================
    /// used for computing the junction tree of the maximal prime subgraphs
    // ============================================================================
    void __computeMaxPrimeMergings( const NodeId node, const NodeId from,
                                    std::vector< Edge >& merged_cliques,
                                    NodeSet& mark ) const;
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/graphs/defaultTriangulation.inl>
#endif //GUM_NO_INLINE


#endif /* GUM_DEFAULT_TRIANGULATION_H */
