/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief base class for all non-incremental triangulations.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_STATIC_TRIANGULATION_H
#define GUM_STATIC_TRIANGULATION_H

#include <vector>

#include <agrum/agrum.h>

#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/eliminationSequenceStrategy.h>
#include <agrum/tools/graphs/algorithms/triangulations/junctionTreeStrategies/junctionTreeStrategy.h>
#include <agrum/tools/graphs/algorithms/triangulations/triangulation.h>


namespace gum {


  /** @class StaticTriangulation
   * @brief base class for all non-incremental triangulation methods
   *
   * \ingroup graph_group
   *
   */
  class StaticTriangulation: public Triangulation {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /** @brief returns a fresh triangulation of the same type as the current
     * object but over an empty graph
     *
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++. */
    virtual StaticTriangulation* newFactory() const = 0;

    /// virtual copy constructor
    /** note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++. */
    virtual StaticTriangulation* copyFactory() const = 0;

    /// destructor
    virtual ~StaticTriangulation();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// initialize the triangulation data structures for a new graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which
     * will be eliminated
     * @param domsizes the domain sizes of the nodes to be eliminated
     * @warning Note that we allow domsizes to be defined over nodes/variables
     * that do not belong to graph. These sizes will simply be ignored. However,
     * it is compulsory that all the nodes of graph belong to dom_sizes
     * @warning the graph is not copied but only referenced by the elimination
     * sequence algorithm. */
    virtual void setGraph(const UndiGraph* graph, const NodeProperty< Size >* domsizes);

    /// returns the fill-ins added by the triangulation algorithm
    const EdgeSet& fillIns();

    /// returns an elimination ordering compatible with the triangulated graph
    const std::vector< NodeId >& eliminationOrder();

    /** @brief returns the index of a given node in the elimination order
     * (0 = first node eliminated) */
    Idx eliminationOrder(const NodeId);

    /** @brief returns a table indicating, for each node, at which step it was
     * deleted by the triangulation process */
    const NodeProperty< NodeId >& reverseEliminationOrder();

    /// returns the triangulated graph
    const UndiGraph& triangulatedGraph();

    /// returns the elimination tree of a compatible ordering
    const CliqueGraph& eliminationTree();

    /// returns a compatible junction tree
    const CliqueGraph& junctionTree();

    /** @brief returns the Id of the clique of the junction tree created by the
     * elimination of a given node during the triangulation process */
    NodeId createdJunctionTreeClique(const NodeId id);

    /** @brief returns the Ids of the cliques of the junction tree created by
     * the elimination of the nodes */
    const NodeProperty< NodeId >& createdJunctionTreeCliques();

    /// returns a junction tree of maximal prime subgraphs
    /** @warning Actually, the cliques of the junction tree are guarranteed to
     * be maximal prime subgraph of the original graph that was triangulated
     * only if the triangulation performed is minimal (in the sense that
     * removing any edge in the triangulated graph results in a nontriangulated
     * graph). This can be ensured by requiring minimality of the
     * triangulation. */
    const CliqueGraph& maxPrimeSubgraphTree();

    /** @brief returns the Id of the maximal prime subgraph created by the
     * elimination of a given node during the triangulation process */
    NodeId createdMaxPrimeSubgraph(const NodeId id);

    /// reinitialize the graph to be triangulated to an empty graph
    void clear();

    /// sets/unset the minimality requirement
    void setMinimalRequirement(bool);

    /// indicates wether minimality is required
    virtual bool isMinimalityRequired() const final;

    /** @brief sets/unsets the record of the fill-ins in the standard
     * triangulation procedure */
    void setFillIns(bool);

    /// returns the graph to be triangulated
    /** @warning if we have not set yet a graph, then originalGraph () will
     * return a nullptr. */
    const UndiGraph* originalGraph() const;

    /// returns the elimination sequence strategy used by the triangulation
    EliminationSequenceStrategy& eliminationSequenceStrategy() const;

    /// returns the junction tree strategy used by the triangulation
    JunctionTreeStrategy& junctionTreeStrategy() const;

    /// @}


    protected:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor: without any graph
    /** @param elimSeq the elimination sequence used to triangulate the graph
     * @param JTStrategy the junction tree strategy used to create junction
     * trees from elimination trees
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion */
    StaticTriangulation(const EliminationSequenceStrategy& elimSeq,
                        const JunctionTreeStrategy&        JTStrategy,
                        bool                               minimality = false);

    /// constructor with a given graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param dom_sizes the domain sizes of the nodes to be eliminated
     * @param elimSeq the elimination sequence used to triangulate the graph
     * @param JTStrategy the junction tree strategy used to create junction
     * trees
     * @warning Note that we allow dom_sizes to be defined over nodes/variables
     * that do not belong to graph. These sizes will simply be ignored. However,
     * it is compulsory that all the nodes of graph belong to dom_sizes
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion
     * @warning note that, by aGrUM's rule, the graph is not copied but only
     * referenced by the triangulation algorithm. */
    StaticTriangulation(const UndiGraph*                   graph,
                        const NodeProperty< Size >*        dom_sizes,
                        const EliminationSequenceStrategy& elimSeq,
                        const JunctionTreeStrategy&        JTStrategy,
                        bool                               minimality = false);

    /// forbid copy constructor except in newfactory
    StaticTriangulation(const StaticTriangulation&);

    /// forbid move constructor except in children's constructors
    StaticTriangulation(StaticTriangulation&&);

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// the function called to initialize the triangulation process
    /** This function is called when the triangulation process starts and is
     * used to initialize the elimination sequence strategy. Actually, the
     * graph that is modified by the triangulation algorithm is a copy of
     * the original graph, and this copy needs be known by the elimination
     * sequence strategy. initTriangulation_ is used to transmit this
     * knowledge to the elimination sequence (through method setGraph of the
     * elimination sequence class).
     * @param graph the very graph that is triangulated (this is a copy of
     *  _original_graph_) */
    virtual void initTriangulation_(UndiGraph& graph);

    /// @}


    /// the elimination sequence strategy used by the triangulation
    EliminationSequenceStrategy* elimination_sequence_strategy_{nullptr};

    /// the junction tree strategy used by the triangulation
    JunctionTreeStrategy* junction_tree_strategy_{nullptr};


    private:
    /// a pointer to the (external) original graph (which will be triangulated)
    const UndiGraph* _original_graph_{nullptr};

    /// the triangulated graph
    UndiGraph _triangulated_graph_;

    /// the fill-ins added during the whole triangulation process
    EdgeSet _fill_ins_;

    /// the order in which nodes are eliminated by the algorithm
    std::vector< NodeId > _elim_order_;

    /// the elimination order (access by NodeId)
    NodeProperty< NodeId > _reverse_elim_order_;

    /// the cliques formed by the elimination of the nodes
    NodeProperty< NodeSet > _elim_cliques_;

    /// the elimination tree computed by the algorithm
    CliqueGraph _elim_tree_;

    /// the junction tree computed by the algorithm
    /** note that the junction tree is owned by the junctionTreeStrategy and,
     * therefore, its deletion from memory is not handled by the static
     * triangulation class. */
    const CliqueGraph* _junction_tree_{nullptr};

    /// the maximal prime subgraph junction tree computed from the junction tree
    CliqueGraph _max_prime_junction_tree_;

    /** @brief indicates which clique of the max prime junction tree was created
     * by the elmination of a given node (the key of the table) */
    NodeProperty< NodeId > _node_2_max_prime_clique_;

    /// a boolean indicating whether we have parformed a triangulation
    bool _has_triangulation_{false};

    /// a boolean indicating whether we have constructed the triangulated graph
    bool _has_triangulated_graph_{false};

    /// a boolean indicating whether the elimination tree has been computed
    bool _has_elimination_tree_{false};

    /// a boolean indicating whether the junction tree has been constructed
    bool _has_junction_tree_{false};

    /** @brief indicates whether a maximal prime subgraph junction tree has
     * been constructed */
    bool _has_max_prime_junction_tree_{false};

    /// indicates whether we actually computed fill-ins
    bool _has_fill_ins_{false};

    /// indicates whether the triangulation must be minimal
    bool _minimality_required_{false};

    /** @brief a vector containing the set of fill-ins added after each node
     * elimination (used by recursive thinning) */
    std::vector< EdgeSet > _added_fill_ins_;

    /** @brief a boolean indicating if we want fill-ins list with the standard
     * triangulation method */
    bool _we_want_fill_ins_{false};

    // ===========================================================================

    /// the function that performs the triangulation
    void _triangulate_();

    /// returns an elimination tree from a triangulated graph
    void _computeEliminationTree_();

    /// computes the junction tree of the maximal prime subgraphs
    void _computeMaxPrimeJunctionTree_();

    /// removes redondant fill-ins and compute proper elimination cliques and
    /// order
    void _computeRecursiveThinning_();

    /// used for computing the junction tree of the maximal prime subgraphs
    void _computeMaxPrimeMergings_(const NodeId        node,
                                   const NodeId        from,
                                   std::vector< Arc >& merged_cliques,
                                   NodeSet&            mark) const;

    /// forbid copy operator
    StaticTriangulation& operator=(const StaticTriangulation&);
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/triangulations/staticTriangulation_inl.h>
#endif   // GUM_NO_INLINE

#endif /* GUM_STATIC_TRIANGULATION_H */
