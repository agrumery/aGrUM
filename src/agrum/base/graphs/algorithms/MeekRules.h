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

#ifndef GUM_MEEKRULES_H
#define GUM_MEEKRULES_H

#include <vector>

#include <agrum/base/graphs/PDAG.h>

namespace gum {
  /**
   * @class MeekRules
   * @brief Applies Meek's orientation rules to propagate arc directions in a mixed graph.
   *
   * Given a MixedGraph (containing both directed arcs and undirected edges, with no
   * double-headed arcs in the input), this class applies rules R1–R4 exhaustively to orient
   * as many undirected edges as possible without introducing new v-structures.  Three output
   * flavours are available: MixedGraph (partial orientation), PDAG (the closure: a CPDAG without
   * background knowledge, an MPDAG with it, if the input is extendable), and DAG (fully oriented).
   *
   * If the input is an extendable PDAG (Dor and Tarsi 1992), its closure under the rules is unique
   * (Meek 1995): propagate and propagateToCPDAG compute it with graph::meekClosure (Wienöbst,
   * Bannach and Liśkiewicz 2021, Algorithm 2). Otherwise (a non-extendable PDAG,
   * as a constraint-based learner may produce from conflicting independence tests, or a mixed graph
   * with double connections), the rules are applied by a fixpoint iteration whose result depends
   * on the order of the nodes, and propagateToCPDAG resolves the double-headed arcs with a
   * min-parents heuristic.
   *
   * propagateToDAG orients the result of propagateToCPDAG with graph::orientAsDAG, which looks for
   * small CPTs: a consistent extension if this PDAG is extendable, a DAG with some new v-structures
   * otherwise; in both cases, the DAG contains all its arcs. The orientations of the edges that the
   * propagation leaves undirected are recorded and accessible via choices().
   *
   * @par References
   * - C. Meek. Causal inference and causal explanation with background knowledge. Proceedings of
   *   the 11th Conference on Uncertainty in Artificial Intelligence (UAI 1995), pp. 403-410.
   *   arXiv:1302.4972.
   * - D. Dor, M. Tarsi. A simple algorithm to construct a consistent extension of a partially
   *   oriented graph. UCLA Cognitive Systems Laboratory, Technical Report R-185, October 1992.
   * - M. Wienöbst, M. Bannach, M. Liśkiewicz. Extendability of causal graphical models: Algorithms
   *   and computational complexity. Proceedings of the 37th Conference on Uncertainty in Artificial
   *   Intelligence, PMLR 161:1248-1257, 2021.
   */
  class GUM_SHARED_PUBLIC MeekRules {
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

    /// Applies Meek rules and returns a PDAG (a CPDAG or an MPDAG if mg is extendable).
    /** Meek rules R1–R4 are applied exhaustively, then any double-headed arcs are resolved
     *  using a min-parents heuristic to avoid cycles.  The result is a valid PDAG with no
     *  double-headed arcs.
     *  @param mg the mixed graph to orient
     *  @return if mg is extendable, its closure: the CPDAG of its Markov equivalence class without
     *  background knowledge in its arcs, an MPDAG with it
     *  @throw InvalidDirectedCycle if mg contains a directed cycle of length 3 or more (the
     *  2-cycles, i.e. double-headed arcs, are resolved)
     */
    PDAG propagateToCPDAG(const MixedGraph& mg);

    /// Orients a MixedGraph into a DAG, all the domains being considered equal.
    /** Same as propagateToDAG(mg, domainSizes) with the same domain size for all the nodes: the
     *  heuristic then looks for nodes with few parents.
     *  @param mg the mixed graph to orient
     *  @return a DAG containing the arcs of propagateToCPDAG(mg)
     *  @throw InvalidDirectedCycle if mg contains a directed cycle of length 3 or more
     */
    DAG propagateToDAG(const MixedGraph& mg);

    /// Orients a MixedGraph into a DAG with small CPTs.
    /** The input is first propagated by propagateToCPDAG, then the resulting PDAG is oriented by
     *  graph::orientAsDAG: a consistent extension if the PDAG is extendable (the input then gives
     *  the same result as its closure), a DAG with new v-structures otherwise. The DAG contains all
     *  the arcs of the propagated PDAG.
     *  @param mg the mixed graph to orient
     *  @param domainSizes the domain size of each node of mg
     *  @return a DAG containing the arcs of propagateToCPDAG(mg)
     *  @throw InvalidArgument if a node of mg has no domain size, or a domain size 0
     *  @throw InvalidDirectedCycle if mg contains a directed cycle of length 3 or more
     */
    DAG propagateToDAG(const MixedGraph& mg, const NodeProperty< Size >& domainSizes);

    /// Returns the arcs for which the algorithm made an arbitrary orientation choice.
    /** An arc is recorded here (once per pair of nodes) when the fixpoint iteration of the rules
     *  (for an input which is not an extendable PDAG) derives both orientations of an edge.
     *  propagateToCPDAG and propagateToDAG then replace it by the orientation kept by the
     *  resolution of the double-headed arcs, and drop it if both orientations are erased.
     *  propagateToDAG also records the orientations of the edges left undirected by
     *  propagateToCPDAG. After propagateToCPDAG or propagateToDAG, each recorded arc is an arc of
     *  the result.
     *  @return the list of arbitrarily chosen arcs, in order of selection
     */
    std::vector< Arc > choices() const;

    private:
    /// Applies Meek rules R1–R4 exhaustively and returns the resulting graph.
    MixedGraph _propagates_(const MixedGraph& graph);

    /// propagateToCPDAG for an input which is not an extendable PDAG: the fixpoint iteration of
    /// the rules, then the resolution of the double-headed arcs
    PDAG _propagatesToPDAG_(const MixedGraph& mg);

    /// the arbitrary choices, in order of selection (at most one orientation per pair of nodes)
    Sequence< Arc > _choices_;

    /// records arc as a choice, unless one of the orientations of its pair is already recorded
    void _recordChoice_(const Arc& arc);

    /// the choice recorded as erased (if any) is replaced by its reverse, the orientation kept
    void _keepReverseChoice_(const Arc& erased);

    /// Tries to orient edges incident to xj using Meek rules R1–R4.
    /** @param graph the graph (modified in place)
     *  @param xj   the node whose incident undirected edges are examined
     *  @return true if at least one edge was oriented
     */
    bool _applyMeekRules_(MixedGraph& graph, NodeId xj);

    /// Resolves double-headed arcs (x→y and y→x simultaneously) to avoid cycles.
    /** For each double-headed arc, erases one direction based on existing directed paths;
     *  if neither direction creates a cycle, applies the min-parents heuristic. The recorded
     *  choices follow the orientations kept.
     *  @param mg the graph (modified in place)
     */
    void _orientDoubleHeadedArcs_(MixedGraph& mg);

    /// Returns true if the edge xi–xj can be oriented as xi→xj under Meek rules R1–R3.
    /** @param graph the graph
     *  @param xi   candidate tail
     *  @param xj   candidate head
     */
    [[nodiscard]] static bool _isOrientable_(const MixedGraph& graph, NodeId xi, NodeId xj);

    /// Returns true if there is a directed path from n1 to n2 (BFS, double arcs ignored).
    static bool _existsDirectedPath_(const MixedGraph& graph, NodeId n1, NodeId n2);

    /// Returns the arc to erase when resolving a double-headed arc, using a min-parents heuristic.
    /** Prefers to keep the direction whose head already has more parents.  Ties are broken by
     *  neighbor count.
     *  @param graph the graph
     *  @param x     one endpoint
     *  @param y     the other endpoint
     *  @return the arc (x→y or y→x) that should be erased
     */
    static Arc _selectArcToEraseByMinParents_(const MixedGraph& graph, NodeId x, NodeId y);
  };   /// class MeekRules

}   // namespace gum


#ifndef GUM_NO_INLINE
#  include <agrum/base/graphs/algorithms/MeekRules_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_MEEKRULES_H
