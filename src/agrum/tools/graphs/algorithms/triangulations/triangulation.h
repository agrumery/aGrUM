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
 * @brief Abstract base class for computing triangulations of graphs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_TRIANGULATION_H
#define GUM_TRIANGULATION_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/core/sequence.h>
#include <agrum/tools/graphs/cliqueGraph.h>
#include <vector>

namespace gum {


  /** @class Triangulation
   * @brief Interface for all the triangulation methods
   *
   * \ingroup graph_group
   *
   */
  class Triangulation {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /** @brief returns a fresh triangulation of the same type as the current
     * object but with an empty graph
     *
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++.*/
    virtual Triangulation* newFactory() const = 0;

    /// virtual copy constructor
    /** note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++.*/
    virtual Triangulation* copyFactory() const = 0;

    /// destructor
    virtual ~Triangulation();

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
    virtual void setGraph(const UndiGraph* graph, const NodeProperty< Size >* domsizes) = 0;

    /// returns the fill-ins added by the triangulation algorithm
    virtual const EdgeSet& fillIns() = 0;

    /// returns an elimination ordering compatible with the triangulated graph
    virtual const std::vector< NodeId >& eliminationOrder() = 0;

    /** @brief returns the index of a given node in the elimination order
     * (0 = first node eliminated) */
    virtual Idx eliminationOrder(const NodeId) = 0;

    /// returns the triangulated graph
    virtual const UndiGraph& triangulatedGraph() = 0;

    /// returns the elimination tree of a compatible ordering
    virtual const CliqueGraph& eliminationTree() = 0;

    /// returns a compatible junction tree
    virtual const CliqueGraph& junctionTree() = 0;

    /// returns the max of log10DomainSize of the cliques in the junction tree.
    /** This is usefull for instance to estimate the complexity (both in space
     * and in time) of the inference that will use the junction tree.
     *
     * This method is not 'const' since it can be called before building any
     * junction tree and hence it needs to build it...
     */
    double maxLog10CliqueDomainSize();

    /** @brief returns the Id of the clique created by the
     * elimination of a given node during the triangulation process */
    virtual NodeId createdJunctionTreeClique(const NodeId id) = 0;

    /** @brief returns the Ids of the cliques of the junction tree created by
     * the elimination of the nodes */
    virtual const NodeProperty< NodeId >& createdJunctionTreeCliques() = 0;

    /// returns a junction tree of maximal prime subgraphs
    /** @warning Actually, the cliques of the junction tree are guarranteed to
     * be maximal prime subgraph of the original graph that was triangulated
     * only if the triangulation performed is minimal (in the sense that
     * removing any edge in the triangulated graph results in a nontriangulated
     * graph). This can be ensured by requiring minimality of the
     * triangulation. */
    virtual const CliqueGraph& maxPrimeSubgraphTree() = 0;

    /** @brief returns the Id of the maximal prime subgraph created by the
     * elimination of a given node during the triangulation process */
    virtual NodeId createdMaxPrimeSubgraph(const NodeId id) = 0;

    /// reinitialize the graph to be triangulated to an empty graph
    virtual void clear() = 0;

    /// returns the domain sizes of the variables of the graph to be
    /// triangulated
    const NodeProperty< Size >* domainSizes() const;

    /// @}


    protected:
    /// the domain sizes of the variables/nodes of the graph
    const NodeProperty< Size >* domain_sizes_{nullptr};


    /// default constructor
    Triangulation();

    /// constructor with a domain size specified
    /** @warning note that, by aGrUM's rule, domsizes is not copied but only
     * referenced by the triangulation algorithm. */
    Triangulation(const NodeProperty< Size >* domsizes);

    /// prevent copy constructor except when using newFactory
    Triangulation(const Triangulation&);

    /// prevent move constructor except when used by children
    Triangulation(Triangulation&&);


    private:
    /// prevent copy operator
    Triangulation& operator=(const Triangulation&);
  };

} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/triangulations/triangulation_inl.h>
#endif   // GUM_NO_INLINE


#endif /* GUM_TRIANGULATION_H */
