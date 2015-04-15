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
#include <sstream>
#include <vector>

#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/triangulations/unconstrainedTriangulation.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace gum_tests {
  class IncrementalTriangulationTestSuite;
}
#endif

namespace gum {

  /* =========================================================================== */
  /* ===            CLASS THAT PERFORMS INCREMENTAL TRIANGULATIONS           === */
  /* =========================================================================== */
  /** @class IncrementalTriangulation
   * @brief Class that performs incremental triangulations
   */
  /* =========================================================================== */

  class IncrementalTriangulation : public Triangulation {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// constructor
    /** Note that, in the graph passed in argument, the type of the edges may
     * differ from Edge. However, the junction trees and triangulated graphs
     * produced by the triangulation algorithm will all have edges of type Edge. */

    IncrementalTriangulation(const UnconstrainedTriangulation &triang_algo,
                             const UndiGraph &theGraph,
                             const NodeProperty<Size> &modal);

    /// default constructor: initialize the triangulation with en empty graph

    IncrementalTriangulation(const UnconstrainedTriangulation &triangAlgo);

    /// copy operator

    IncrementalTriangulation(const IncrementalTriangulation &from);

    /// destructor

    ~IncrementalTriangulation();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// updates the triangulated graph using the modif list

    void updateTriangulation();

    /// adds a new node to the graph
    GUM_DEPRECATED(void insertNode(const NodeId node, Size modal));
    void addNode(const NodeId node, Size modal);

    /** @brief removes a node from the graph (the join tree may need a
     * triangulation update) */
    void eraseNode(const NodeId node);

    /** @brief adds a new edge to the graph (the join tree may need a
     * triangulation update) */

    GUM_DEPRECATED(void insertEdge(const NodeId X, const NodeId Y));
    void addEdge(const NodeId X, const NodeId Y);

    /// removes an edge from the graph (the join tree may need a retriangulation)

    void eraseEdge(const Edge &edge);

    /// returns the fill-ins added by the triangulation algorithm

    const EdgeSet &fillIns() {
      GUM_ERROR(OperationNotAllowed, "Not implemented yet");
    };

    /// returns an elimination ordering compatible with the triangulated graph
    /// @todo : whu not a Sequence ?

    const std::vector<NodeId> &eliminationOrder();

    /** @brief returns the number of a given node in the elimination order
     * (0 = first node eliminated) */

    Idx eliminationOrder(const NodeId);

    /// returns the triangulated graph

    const UndiGraph &triangulatedGraph() {
      GUM_ERROR(OperationNotAllowed, "Not implemented yet");
    };

    /// returns the current graph (that which is incrementally triangulated)

    const UndiGraph &graph() const;

    /// returns the elimination tree of a compatible ordering

    const CliqueGraph &eliminationTree() {
      GUM_ERROR(OperationNotAllowed, "Not implemented yet");
    };

    /// returns a junction tree corresponding to the current graph

    const CliqueGraph &junctionTree();

    /** @brief returns the Id of the clique created by the
     * elimination of a given node during the triangulation process */

    NodeId createdJunctionTreeClique(const NodeId id);

    /** @brief returns the Ids of the cliques of the junction tree created by the
     * elimination of the nodes */

    const NodeProperty<NodeId> &createdJunctionTreeCliques();

    /// returns the junction tree of the maximal prime subgraphs

    const CliqueGraph &maxPrimeSubgraphTree();

    /** @brief returns the Id of the maximal prime subgraph created by the
     * elimination of a given node during the triangulation process */

    NodeId createdMaxPrimeSubgraph(const NodeId id);

    /// sets the graph to the empty graph

    void clear();

    /// changes the current graph

    void setGraph(const UndiGraph &theGraph, const NodeProperty<Size> &modal);

    /// returns the triangulation algorithm (useful for fine tuning it)

    const UnconstrainedTriangulation &triangulationAlgo() const;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator

    IncrementalTriangulation &operator=(const IncrementalTriangulation &from);

    /// virtual copy constructor

    IncrementalTriangulation *newFactory() const;

    /// @}

    private:
    /// the graph that needs be triangulated
    UndiGraph __graph;

    /// the junction tree computed so far
    CliqueGraph __junction_tree;

    /// the maximal prime subgraph tree
    CliqueGraph __T_mpd;

    /// for each node in graph, store the MPS containing the node
    NodeProperty<List<NodeId>> __mps_of_node;

    /// indicate for each MPS its set of cliques in the junction tree
    NodeProperty<std::vector<NodeId>> __cliques_of_mps;

    /// indicate for each clique the MPS it belongs to
    NodeProperty<NodeId> __mps_of_clique;

    /// the set of MPS affected by a new triangulation
    NodeProperty<bool> __mps_affected;

    /// the triangulation algorithm that will be used incremantally
    UnconstrainedTriangulation *__triangulation;

    /// a Boolean indicating whether the triangulation need be updated
    bool __require_update;

    /// a Boolean indicating wether we should update the elimination order
    bool __require_elimination_order;

    /// the current elimination ordering
    std::vector<NodeId> __elimination_order;

    /// the elimination order (access by NodeId)
    NodeProperty<Idx> __reverse_elimination_order;

    /// a Boolean indicating whether we should compute the createdJTCliques
    bool __require_created_JT_cliques;

    /// For each node, a clique that contains it
    NodeProperty<NodeId> __created_JT_cliques;

    /// mark the mps affected by the deletion of a given edge

    void __markAffectedMPSsByRemoveLink(const NodeId My, const NodeId Mz,
                                        const Edge &edge);

    /// mark the mps affected by the insertion of a new edge

    int __markAffectedMPSsByAddLink(const NodeId My, const NodeId Mz, const NodeId X,
                                    const NodeId Y);

    /// remove a given node from the T_mpd structure

    void __performRemoveNode(const NodeId node, const NodeId My, const NodeId Mz);

    /// adds a new node to T_mpd, the graph and the clique graph

    void __performAddNode(const NodeId node);

    /// set-up the connected subgraph that needs be retriangulated

    void __setUpConnectedTriangulation(NodeId Mx, NodeId Mfrom, UndiGraph &theGraph,
                                       std::vector<Edge> &notAffectedneighborClique,
                                       HashTable<NodeId, bool> &cliques_affected);

    /// used for computing the junction tree of the maximal prime subgraphs

    void
    __computeMaxPrimeMergings(const NodeId node, const NodeId from,
                              std::vector<std::pair<NodeId, NodeId>> &merged_cliques,
                              NodeProperty<bool> &mark,
                              const NodeSet &new_nodes_in_junction_tree) const;

    /// update the junction tree

    void __updateJunctionTree(NodeProperty<bool> &all_cliques_affected,
                              NodeSet &new_nodes_in_junction_tree);

    /// update the max prime subgraph

    void __updateMaxPrimeSubgraph(NodeProperty<bool> &cliques_affected,
                                  const NodeSet &new_nodes_in_junction_tree);

    /// a collect algorithm to compute elimination orderings

    void __collectEliminationOrder(const NodeId node, const NodeId from,
                                   NodeProperty<bool> &examined, Idx &index);

    /// a collect algorithm to compute, for each node, one container JT's clique

    void __collectJTCliques(const NodeId clique, const NodeId from,
                            NodeProperty<bool> &examined);

    /// checks that the incremental triangulation works properly

    bool __check();

    /// to enable testunits to use __check

    friend class gum_tests::IncrementalTriangulationTestSuite;
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/graphs/triangulations/incrementalTriangulation.inl>
#endif // GUM_NO_INLINE

#endif /* GUM_INCREMENTAL_TRIANGULATION_H */
