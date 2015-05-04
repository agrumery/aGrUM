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
 * @brief base class for all non-incremental triangulations.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_STATIC_TRIANGULATION_H
#define GUM_STATIC_TRIANGULATION_H

#include <vector>

#include <agrum/config.h>

#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/triangulations/triangulation.h>
#include <agrum/graphs/eliminations/eliminationSequenceStrategy.h>
#include <agrum/graphs/junctionTreeStrategy.h>

namespace gum {

  /* =========================================================================== */
  /** @class StaticTriangulation
   * @brief base class for all non-incremental triangulation methods
   *
   * \ingroup graph_group
   *
   */
  /* =========================================================================== */

  class StaticTriangulation : public Triangulation {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /** @brief returns a fresh triangulation (over an empty graph) of the same
     * type as the current object
     *
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++.*/
    virtual StaticTriangulation *newFactory() const = 0;

    /// destructor
    virtual ~StaticTriangulation();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the fill-ins added by the triangulation algorithm
    const EdgeSet &fillIns();

    /// returns an elimination ordering compatible with the triangulated graph
    const std::vector<NodeId> &eliminationOrder();

    /** @brief returns the number of a given node in the elimination order
     * (0 = first node eliminated) */
    Idx eliminationOrder(const NodeId);

    /** @brief returns a table indicating, for each node, at which step it was
     * deleted by the triangulation process */
    const NodeProperty<Idx> &reverseEliminationOrder();

    /// returns the triangulated graph
    const UndiGraph &triangulatedGraph();

    /// returns the elimination tree of a compatible ordering
    const CliqueGraph &eliminationTree();

    /// returns a compatible junction tree
    const CliqueGraph &junctionTree();

    /** @brief returns the Id of the clique of the junction tree created by the
     * elimination of a given node during the triangulation process */
    NodeId createdJunctionTreeClique(const NodeId id);

    /** @brief returns the Ids of the cliques of the junction tree created by the
     * elimination of the nodes */
    const NodeProperty<NodeId> &createdJunctionTreeCliques();

    /// returns a junction tree of maximal prime subgraphs
    /** @warning Actually, the cliques of the junction tree are guarranteed to be
     * maximal prime subgraph of the original graph that was triangulated only if
     * the triangulation performed is minimal (in the sense that removing any edge
     * in the triangulated graph results in a nontriangulated graph). This can be
     * ensured by requiring minimality of the triangulation. */
    const CliqueGraph &maxPrimeSubgraphTree();

    /** @brief returns the Id of the maximal prime subgraph created by the
     * elimination of a given node during the triangulation process */
    NodeId createdMaxPrimeSubgraph(const NodeId id);

    /// reinitialize the graph to be triangulated to an empty graph
    void clear();

    /// sets/unset the minimality requirement
    void setMinimalRequirement(bool);

    /// sets/unset the fill-ins storage in the standard triangulation procedure
    void setFillIns(bool);

    /// returns the graph to be triangulated
    const UndiGraph *originalGraph() const;

    /// @}

    protected:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor: without any graph
    /** @param elimSeq the elimination sequence used to triangulate the graph
     * @param JTStrategy the junction tree strategy used to create junction trees
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion */
    StaticTriangulation(const EliminationSequenceStrategy &elimSeq,
                        const JunctionTreeStrategy &JTStrategy,
                        bool minimality = false);

    /// constructor with a given graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param dom the domain sizes of the nodes to be eliminated
     * @param elimSeq the elimination sequence used to triangulate the graph
     * @param JTStrategy the junction tree strategy used to create junction trees
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion
     * @warning note that, by aGrUM's rule, the graph and the modalities are not
     * copied but only referenced by the elimination sequence algorithm. */
    StaticTriangulation(const UndiGraph *graph, const NodeProperty<Size> *dom,
                        const EliminationSequenceStrategy &elimSeq,
                        const JunctionTreeStrategy &JTStrategy,
                        bool minimality = false);

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// initialize the triangulation data structures for a new graph
    /** @param gr the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param modal the domain sizes of the nodes to be eliminated
     * @warning note that, by aGrUM's rule, the graph and the modalities are not
     * copied but only referenced by the elimination sequence algorithm. */
    void _setGraph(const UndiGraph *gr, const NodeProperty<Size> *modal);

    /// the function called to initialize the triangulation process
    /** This function is called when the triangulation process starts and is
     * used to initialize the elimination sequence strategy. Actually, the
     * graph that is modified by the triangulation algorithm is a copy of
     * the original graph, and this copy need be known by the elimination
     * sequence strategy. _initTriangulation is used to transmit this
     * knowledge to the elimination sequence (through method setGraph of the
     * elimination sequence class). As method setGraph has different prototypes
     * depending on the class of the elimination sequence, we delegate the
     * _initTriangulation to a triangulation class that knows which prototype
     * should be used.
     * @param graph the very graph that is triangulated (this is a copy of
     * __original_graph) */
    virtual void _initTriangulation(UndiGraph &graph) = 0;

    /// @}

    /// the elimination sequence strategy used by the triangulation
    EliminationSequenceStrategy *_elimination_sequence_strategy;

    /// the junction tree strategy used by the triangulation
    JunctionTreeStrategy *_junction_tree_strategy;

    private:
    /// the original graph (that which will be triangulated)
    const UndiGraph *__original_graph;

    /// the triangulated graph
    UndiGraph __triangulated_graph;

    /// the fill-ins added during the whole triangulation process
    EdgeSet __fill_ins;

    /// the order in which nodes are eliminated by the algorithm
    std::vector<NodeId> __elim_order;

    /// the elimination order (access by NodeId)
    NodeProperty<Idx> __reverse_elim_order;

    /// the cliques formed by the elimination of the nodes
    NodeProperty<NodeSet> __elim_cliques;

    /// the elimination tree computed by the algorithm
    CliqueGraph __elim_tree;

    /// the junction tree computed by the algorithm
    const CliqueGraph *__junction_tree;

    /// the maximal prime subgraph junction tree computed from the junction tree
    CliqueGraph __max_prime_junction_tree;

    /** @brief indicates which clique of the max prime junction tree was created
     * by the elmination of a given node (the key of the table) */
    NodeProperty<NodeId> __node_2_max_prime_clique;

    /// a boolean indicating whether we have parformed a triangulation
    bool __has_triangulation;

    /// a boolean indicating whether we have constructed the triangulated graph
    bool __has_triangulated_graph;

    /// a boolean indicating whether the elimination tree has been computed
    bool __has_elimination_tree;

    /// a boolean indicating whether the junction tree has been constructed
    bool __has_junction_tree;

    /** @brief indicates whether a maximal prime subgraph junction tree has
      * been constructed */
    bool __has_max_prime_junction_tree;

    /// indicates whether we actually computed fill-ins
    bool __has_fill_ins;

    /// indicates whether the triangulation must be minimal
    bool __minimality_required;

    /** @brief a vector containing the set of fill-ins added after each node
     * elimination (used by recursive thinning) */
    std::vector<EdgeSet> __added_fill_ins;

    /** @brief a boolean indicating if we want fill-ins list with the standard
     * triangulation method */
    bool __we_want_fill_ins;

    // ===========================================================================

    /// the function that performs the triangulation
    void __triangulate();

    /// returns an elimination tree from a triangulated graph
    void __computeEliminationTree();

    /// computes the junction tree of the maximal prime subgraphs
    void __computeMaxPrimeJunctionTree();

    /// removes redondant fill-ins and compute proper elimination cliques and order
    void __computeRecursiveThinning();

    /// used for computing the junction tree of the maximal prime subgraphs
    void __computeMaxPrimeMergings(const NodeId node, const NodeId from,
                                   std::vector<Arc> &merged_cliques,
                                   NodeSet &mark) const;

    /// forbid copy constructor
    StaticTriangulation(const StaticTriangulation &);

    /// forbid copy operator
    StaticTriangulation &operator=(const StaticTriangulation &);
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/graphs/triangulations/staticTriangulation.inl>
#endif // GUM_NO_INLINE

#endif /* GUM_STATIC_TRIANGULATION_H */
