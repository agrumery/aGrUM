/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief The Prim algorithm for computing min cost spanning trees or forests
 *
 * @author Jean-Philippe DUBUS & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_SPANNING_FOREST_PRIM_H
#define GUM_SPANNING_FOREST_PRIM_H

#include <agrum/base/core/priorityQueue.h>
#include <agrum/base/graphs/algorithms/spanningForest.h>

namespace gum {

  /* ===========================================================================
   */
  /** @class SpanningForestPrim
   * @brief The Prim algorithm for computing min cost spanning trees or forests
   *
   * Binary heap implementation : O(E log(V)) */
  /* ===========================================================================
   */
  class SpanningForestPrim: public SpanningForest {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor
    /** Note that this algorithm takes into account the fact that the graph
     * given
     * in input is not connected (that, is, it may contain several connected
     * components)
     * @param graph the graph the spanning forest of which we look for
     * @param costTable the cost for each edge of graph
     * @warning note that, by aGrUM's rule, the graph and the costs are not
     * copied but only referenced by the elimination sequence algorithm.
     * @throws GraphError if the grand and/or the cost table are null pointers
     */
    SpanningForestPrim(const UndiGraph* graph, const EdgeProperty< float >* costTable);

    /// Copy constructor
    SpanningForestPrim(const SpanningForestPrim& toCopy);

    /// Destructor
    virtual ~SpanningForestPrim();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// Returns the edges in a min cost spanning forest
    /** @returns edges in the spanning forest */
    const EdgeSet& edgesInSpanningForest();

    /// Construct the spanning forest
    /** @return the spanning forest */
    const UndiGraph& spanningForest();

    /// Returns the cost of the spanning forest
    /** @return cost of the spanning forest */
    float costOfSpanningForest();

    /// @}

    private:
    /// the graph the spanning tree of which we wish to compute
    const UndiGraph& _graph_;

    /// the costs of the edges
    const EdgeProperty< float >& _costTable_;

    /// the next edges that may be added to the spanning tree
    PriorityQueue< Edge, float > _edgesToExplore_;

    /// the computed spanning tree
    UndiGraph _spanning_tree_;

    /// the cost of the spanning tree
    float _spanning_tree_cost_;

    /// a Boolean indicating whether we need recompute the spanning tree
    bool _require_computation_;

    /// Computes the spanning forest
    void _compute_();

    /// compute a spanning tree in a given connected component of  _graph_
    void _computeInAComponent_(const NodeId id);

    /// explore the neighborhood of a node belonging to the spanning tree
    void _exploreNode_(const NodeId id);

    /// Copy operator: private to prevent using it
    SpanningForestPrim& operator=(const SpanningForestPrim& toCopy);
  };

} /* namespace gum */

#endif /* GUM_SPANNING_FOREST_PRIM_H */
