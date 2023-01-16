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
 * @brief Class for computing default triangulations of graphs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_INCREMENTAL_TRIANGULATION_H
#define GUM_INCREMENTAL_TRIANGULATION_H

#include <iostream>
#include <sstream>
#include <vector>

#include <agrum/tools/graphs/algorithms/triangulations/unconstrainedTriangulation.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace gum_tests {
  class IncrementalTriangulationTestSuite;
}
#endif

namespace gum {

  /** @class IncrementalTriangulation
   * @brief Class that performs incremental triangulations
   */
  class IncrementalTriangulation: public Triangulation {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// constructor
    /** Note that, in the graph passed in argument, the type of the edges may
     * differ from Edge. However, the junction trees and triangulated graphs
     * produced by the triangulation algorithm will all have edges of type Edge.
     */
    IncrementalTriangulation(const UnconstrainedTriangulation& triang_algo,
                             const UndiGraph*                  theGraph,
                             const NodeProperty< Size >*       modal);

    /// default constructor: initialize the triangulation with en empty graph
    IncrementalTriangulation(const UnconstrainedTriangulation& triangAlgo);

    /// copy operator
    IncrementalTriangulation(const IncrementalTriangulation& from);

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
    void addNode(const NodeId node, Size modal);

    /** @brief removes a node from the graph (the join tree may need a
     * triangulation update) */
    void eraseNode(const NodeId node);

    /** @brief adds a new edge to the graph (the join tree may need a
     * triangulation update) */
    void addEdge(const NodeId X, const NodeId Y);

    /// removes an edge from the graph (the join tree may need a
    /// retriangulation)
    void eraseEdge(const Edge& edge);

    /// returns the fill-ins added by the triangulation algorithm
    const EdgeSet& fillIns() { GUM_ERROR(OperationNotAllowed, "Not implemented yet") }

    /// returns an elimination ordering compatible with the triangulated graph
    const std::vector< NodeId >& eliminationOrder();

    /** @brief returns the number of a given node in the elimination order
     * (0 = first node eliminated) */
    Idx eliminationOrder(const NodeId);

    /// returns the triangulated graph
    const UndiGraph& triangulatedGraph() { GUM_ERROR(OperationNotAllowed, "Not implemented yet") }

    /// returns the current graph (that which is incrementally triangulated)
    const UndiGraph& graph() const;

    /// returns the elimination tree of a compatible ordering
    const CliqueGraph& eliminationTree() { GUM_ERROR(OperationNotAllowed, "Not implemented yet") }

    /// returns a junction tree corresponding to the current graph
    const CliqueGraph& junctionTree();

    /** @brief returns the Id of the clique created by the
     * elimination of a given node during the triangulation process */
    NodeId createdJunctionTreeClique(const NodeId id);

    /** @brief returns the Ids of the cliques of the junction tree created by
     * the elimination of the nodes */
    const NodeProperty< NodeId >& createdJunctionTreeCliques();

    /// returns the junction tree of the maximal prime subgraphs
    const CliqueGraph& maxPrimeSubgraphTree();

    /** @brief returns the Id of the maximal prime subgraph created by the
     * elimination of a given node during the triangulation process */
    NodeId createdMaxPrimeSubgraph(const NodeId id);

    /// sets the graph to the empty graph
    void clear();

    /// changes the current graph
    void setGraph(const UndiGraph* theGraph, const NodeProperty< Size >* domain_sizes);

    /// returns the triangulation algorithm (useful for fine tuning it)
    const UnconstrainedTriangulation& triangulationAlgo() const;

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    IncrementalTriangulation& operator=(const IncrementalTriangulation& from);

    /// virtual clone constructor
    virtual IncrementalTriangulation* newFactory() const final;

    /// virtual copy constructor
    virtual IncrementalTriangulation* copyFactory() const final;

    /// @}


    private:
    /// the graph that needs be triangulated
    UndiGraph _graph_;

    /// the domain sizes of the nodes
    NodeProperty< Size > _domain_sizes_;

    /// the junction tree computed so far
    CliqueGraph _junction_tree_;

    /// the maximal prime subgraph tree
    CliqueGraph _T_mpd_;

    /// for each node in graph, store the MPS containing the node
    NodeProperty< List< NodeId > > _mps_of_node_;

    /// indicate for each MPS its set of cliques in the junction tree
    NodeProperty< std::vector< NodeId > > _cliques_of_mps_;

    /// indicate for each clique the MPS it belongs to
    NodeProperty< NodeId > _mps_of_clique_;

    /// the set of MPS affected by a new triangulation
    NodeProperty< bool > _mps_affected_;

    /// the triangulation algorithm that will be used incremantally
    UnconstrainedTriangulation* _triangulation_;

    /// a Boolean indicating whether the triangulation need be updated
    bool _require_update_{false};

    /// a Boolean indicating wether we should update the elimination order
    bool _require_elimination_order_{false};

    /// the current elimination ordering
    std::vector< NodeId > _elimination_order_;

    /// the elimination order (access by NodeId)
    NodeProperty< Idx > _reverse_elimination_order_;

    /// a Boolean indicating whether we should compute the createdJTCliques
    bool _require_created_JT_cliques_{false};

    /// For each node, a clique that contains it
    NodeProperty< NodeId > _created_JT_cliques_;

    /// mark the mps affected by the deletion of a given edge
    void _markAffectedMPSsByRemoveLink_(const NodeId My, const NodeId Mz, const Edge& edge);

    /// mark the mps affected by the insertion of a new edge
    int _markAffectedMPSsByAddLink_(const NodeId My,
                                    const NodeId Mz,
                                    const NodeId X,
                                    const NodeId Y);

    /// remove a given node from the T_mpd structure
    void _performRemoveNode_(const NodeId node, const NodeId My, const NodeId Mz);

    /// adds a new node to T_mpd, the graph and the clique graph
    void _performAddNode_(const NodeId node);

    /// set-up the connected subgraph that needs be retriangulated
    void _setUpConnectedTriangulation_(NodeId                     Mx,
                                       NodeId                     Mfrom,
                                       UndiGraph&                 theGraph,
                                       std::vector< Edge >&       notAffectedneighborClique,
                                       HashTable< NodeId, bool >& cliques_affected);

    /// used for computing the junction tree of the maximal prime subgraphs
    void _computeMaxPrimeMergings_(const NodeId                                node,
                                   const NodeId                                from,
                                   std::vector< std::pair< NodeId, NodeId > >& merged_cliques,
                                   NodeProperty< bool >&                       mark,
                                   const NodeSet& new_nodes_in_junction_tree) const;

    /// update the junction tree
    void _updateJunctionTree_(NodeProperty< bool >& all_cliques_affected,
                              NodeSet&              new_nodes_in_junction_tree);

    /// update the max prime subgraph
    void _updateMaxPrimeSubgraph_(NodeProperty< bool >& cliques_affected,
                                  const NodeSet&        new_nodes_in_junction_tree);

    /// a collect algorithm to compute elimination orderings
    void _collectEliminationOrder_(const NodeId          node,
                                   const NodeId          from,
                                   NodeProperty< bool >& examined,
                                   Idx&                  index);

    /// a collect algorithm to compute, for each node, one container JT's clique
    void _collectJTCliques_(const NodeId clique, const NodeId from, NodeProperty< bool >& examined);

    /// checks that the incremental triangulation works properly
    bool _check_();

    /// to enable testunits to use  _check_
    friend class gum_tests::IncrementalTriangulationTestSuite;
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/triangulations/incrementalTriangulation_inl.h>
#endif   // GUM_NO_INLINE

#endif /* GUM_INCREMENTAL_TRIANGULATION_H */
