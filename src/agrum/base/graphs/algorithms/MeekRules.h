/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief Meek rules for orienting edges in a mixed graph
 *
 * @author Mahdi Hadj Ali and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef AGRUM_MEEKRULES_H
#define AGRUM_MEEKRULES_H

#include <agrum/base/graphs/PDAG.h>

namespace gum {
  /**
   * @class MeekRules
   * @brief Applies Meek's orientation rules to propagate arc directions in a mixed graph.
   *
   * Given a MixedGraph (containing both directed arcs and undirected edges, with no
   * double-headed arcs in the input), this class applies rules R1–R4 exhaustively to orient
   * as many undirected edges as possible without introducing new v-structures.  Three output
   * flavours are available: MixedGraph (partial orientation), PDAG (completed PDAG / CPDAG),
   * and DAG (fully oriented).
   *
   * When the rules alone cannot resolve all undirected edges, an arbitrary orientation is chosen
   * using a min-parents heuristic; the affected arcs are recorded and accessible via choices().
   */
  class MeekRules {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    MeekRules();

    /// destructor
    virtual ~MeekRules();

    /// @}

    /// Applies Meek rules to a MixedGraph and returns the partially oriented result.
    /** Meek rules R1–R4 are applied exhaustively; remaining undirected edges are left as-is.
     *  Double-headed arcs may appear in the output if two rules fire in opposite directions
     *  on the same edge.
     *  @param mg the mixed graph to orient (arcs and undirected edges, no double-headed arcs)
     *  @return the partially oriented graph; may still contain undirected edges or double-headed
     * arcs
     */
    MixedGraph propagate(const MixedGraph& mg);

    /// Applies Meek rules and returns a CPDAG (completed PDAG).
    /** Meek rules R1–R4 are applied exhaustively, then any double-headed arcs are resolved
     *  using a min-parents heuristic to avoid cycles.  The result is a valid PDAG with no
     *  double-headed arcs.
     *  @param mg the mixed graph to orient
     *  @return a PDAG representing the essential graph
     */
    PDAG propagateToCPDAG(const MixedGraph& mg);

    /// Applies Meek rules and completes the result into a DAG.
    /** Meek rules R1–R4 are applied, remaining undirected edges are arbitrarily oriented,
     *  and double-headed arcs are resolved to ensure an acyclic result.
     *  @param mg the mixed graph to orient
     *  @return a DAG consistent with the orientations implied by the input
     */
    DAG propagateToDAG(const MixedGraph& mg);

    /// Returns the arcs for which the algorithm made an arbitrary orientation choice.
    /** An arc is recorded here when no Meek rule could determine its direction and the
     *  algorithm had to pick one arbitrarily (e.g. to break a double-headed arc or to
     *  orient a residual undirected edge).
     *  @return the list of arbitrarily chosen arcs, in order of selection
     */
    [[nodiscard]] const std::vector< Arc >& choices() const { return _choices_; }

    private:
    /// Applies Meek rules R1–R4 exhaustively and returns the resulting graph.
    MixedGraph _propagates_(const MixedGraph& graph);

    std::vector< Arc > _choices_;

    /// Tries to orient edges incident to xj using Meek rules R1–R4.
    /** @param graph the graph (modified in place)
     *  @param xj   the node whose incident undirected edges are examined
     *  @return true if at least one edge was oriented
     */
    bool _applyMeekRules_(MixedGraph& graph, NodeId xj);

    /// Arbitrarily orients remaining undirected edges when no Meek rule applies.
    /** Uses a BFS-based heuristic: picks the node with the most children as a local root
     *  and orients edges away from it.
     *  @param graph the graph (modified in place)
     */
    void _propagatesOrientationInChainOfRemainingEdges_(MixedGraph& graph);

    /// Resolves double-headed arcs (x→y and y→x simultaneously) to avoid cycles.
    /** For each double-headed arc, erases one direction based on existing directed paths;
     *  if neither direction creates a cycle, applies the min-parents heuristic.
     *  @param mg the graph (modified in place)
     */
    void _orientDoubleHeadedArcs_(MixedGraph& mg);

    /// Returns true if the edge xi–xj can be oriented as xi→xj under Meek rules R1–R3.
    /** @param graph the graph
     *  @param xi   candidate tail
     *  @param xj   candidate head
     */
    [[nodiscard]] bool _isOrientable_(const MixedGraph& graph, NodeId xi, NodeId xj) const;

    /// Returns true if there is a directed path from n1 to n2 (BFS, double arcs ignored).
    static bool _existsDirectedPath_(const MixedGraph& graph, NodeId n1, NodeId n2);

    /// Orients all remaining undirected edges, applying Meek rules then a chain heuristic.
    void _complete_(MixedGraph& graph);

    /// Returns the arc to erase when resolving a double-headed arc, using a min-parents heuristic.
    /** Prefers to keep the direction whose head already has more parents.  Ties are broken by
     *  neighbour count.
     *  @param graph the graph
     *  @param x     one endpoint
     *  @param y     the other endpoint
     *  @return the arc (x→y or y→x) that should be erased
     */
    Arc _selectArcToEraseByMinParents_(const MixedGraph& graph, NodeId x, NodeId y);
  };   /// class MeekRules

}   // namespace gum


#endif   // AGRUM_MEEKRULES_H
