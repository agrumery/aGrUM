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
 * @brief The Prim algorithm for computing min cost spanning trees or forests
 *
 * @author Jean-Philippe DUBUS and Christophe GONZALES
 */
#ifndef GUM_SPANNING_FOREST_PRIM_H
#define GUM_SPANNING_FOREST_PRIM_H

#include <agrum/core/priorityQueue.h>
#include <agrum/graphs/spanningForest.h>

namespace gum {

  /* =========================================================================== */
  /** @class SpanningForestPrim
   * @brief The Prim algorithm for computing min cost spanning trees or forests
   *
   * Binary heap implementation : O(E log(V)) */
  /* =========================================================================== */
  class SpanningForestPrim : public SpanningForest {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor
    /** Note that this algorithm takes into account the fact that the graph given
     * in input is not connected (that, is, it may contain several connected
     * components)
     * @param graph the graph the spanning forest of which we look for
     * @param costTable the cost for each edge of graph
     * @warning note that, by aGrUM's rule, the graph and the costs are not
     * copied but only referenced by the elimination sequence algorithm.
     * @throws GraphError if the grand and/or the cost table are null pointers */
    SpanningForestPrim(const UndiGraph *graph, const EdgeProperty<float> *costTable);

    /// Copy constructor
    SpanningForestPrim(const SpanningForestPrim &toCopy);

    /// Destructor
    virtual ~SpanningForestPrim();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// Returns the edges in a min cost spanning forest
    /** @returns edges in the spanning forest */
    const EdgeSet &edgesInSpanningForest();

    /// Construct the spanning forest
    /** @return the spanning forest */
    const UndiGraph &spanningForest();

    /// Returns the cost of the spanning forest
    /** @return cost of the spanning forest */
    float costOfSpanningForest();

    /// @}

    private:
    /// the graph the spanning tree of which we wish to compute
    const UndiGraph &__graph;

    /// the costs of the edges
    const EdgeProperty<float> &__costTable;

    /// the next edges that may be added to the spanning tree
    PriorityQueue<Edge, float> __edgesToExplore;

    /// the computed spanning tree
    UndiGraph __spanning_tree;

    /// the cost of the spanning tree
    float __spanning_tree_cost;

    /// a Boolean indicating whether we need recompute the spanning tree
    bool __require_computation;

    /// Computes the spanning forest
    void __compute();

    /// compute a spanning tree in a given connected component of __graph
    void __computeInAComponent(const NodeId id);

    /// explore the neighborhood of a node belonging to the spanning tree
    void __exploreNode(const NodeId id);

    /// Copy operator: private to prevent using it
    SpanningForestPrim &operator=(const SpanningForestPrim &toCopy);
  };

} /* namespace gum */

#endif /* GUM_SPANNING_FOREST_PRIM_H */
