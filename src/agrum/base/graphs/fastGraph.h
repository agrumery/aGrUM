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
 * @brief Builds a graph from a "fast" DOT-like textual description.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_FAST_GRAPH_H
#define GUM_FAST_GRAPH_H

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/graphs/mixedGraph.h>

namespace gum {

#ifndef SWIG
  namespace graph::detail {

    /// which operator (if any) precedes a node token in a fastGraph chain
    enum class FastGraphOp : std::uint8_t { None, Arc, RevArc, Edge };

    /**
     * @brief Scans one ";"-delimited chain, splitting it into node tokens
     * while remembering, for each token, whether it was preceded by "->"
     * (Arc), "<-" (RevArc) or "-" (Edge). A plain split() cannot preserve
     * this, since it only handles one delimiter at a time.
     *
     * "-" (not "--") is used for edges: MRF's own fast syntax already uses
     * "--" to list the variables of a single factor (a clique), which is a
     * different construct from a chain of pairwise edges. Reusing "--" here
     * would silently give the same token two incompatible meanings.
     *
     * A '-' inside '['...']', '{'...'}' (the domain of a variable: "A[-3,3]", "B{-1.5:6.3:5}") or
     * a quoted name is not an operator (see fastSyntax.h).
     */
    GUM_SHARED_PUBLIC std::vector< std::pair< std::string, FastGraphOp > >
                      fastGraphTokenize(std::string_view chain);

    /// the NodeId denoted by token if it is written with digits only (no sign) and is in the range
    /// of an int, std::nullopt otherwise
    GUM_SHARED_PUBLIC std::optional< NodeId > fastGraphNodeId(std::string_view token);

    /// true if an unquoted token can name a node of fastGraph() or fastMarkedGraph(): no blank,
    /// '<' nor '>' outside its brackets (a misplaced operator, e.g. "A<" in "A<->B" or "A o" in
    /// "A o -B"); "A{<5|>=5}" is a name
    GUM_SHARED_PUBLIC bool fastGraphIsNodeName(std::string_view token);

    /**
     * @brief Checks the NodeIds of a description read in NodeId mode.
     *
     * A graph stores its absent ids lower than its largest one (holes): as for the GUM format,
     * their number must not exceed max(number of distinct ids, 65536), so that the memory of the
     * graph is linear in the size of the description.
     *
     * @param desc the description, only used to report errors.
     * @param ids the NodeIds of the description (possibly repeated).
     * @throw InvalidArgument if there are too many holes.
     */
    GUM_SHARED_PUBLIC void fastGraphCheckNodeIds(std::string_view desc, std::vector< NodeId > ids);

  }   // namespace graph::detail
#endif   // SWIG

  /**
   * @brief Builds a GRAPH_TYPE from a DOT-like textual description.
   *
   * Syntax: "->" separates two node tokens with a directed arc, "<-" with a
   * directed arc in the other direction, "-" separates them with an
   * undirected edge, ";" or a line break separates independent chains, e.g. "A->B->C; B->E"
   * or "A->B<-C" (both A and C point to B) or "A->B-C".
   *
   * "-" (not "--") is used for edges on purpose: MRF's own fast syntax
   * already uses "--" to list the variables of a single factor (a clique),
   * a different construct from a chain of pairwise edges.
   *
   * If every node token in @p desc parses as a non-negative integer, those
   * integers are used directly as NodeIds (addNodeWithId). Otherwise every
   * token — including numeric-looking ones — is treated as a string name
   * (addNode + setName). A node name contains no blank, '<' nor '>', unless it is written between
   * single quotes ("'Visit Asia'->X", a doubled quote in a quoted name stands for a quote, see
   * fastSyntax.h); a quoted name is always a name ("'12'" is not a NodeId).
   *
   * @tparam GRAPH_TYPE Concrete graph type to build (DiGraph, UndiGraph,
   *         MixedGraph, DAG, PDAG, ...). Defaults to MixedGraph.
   * @param desc the DOT-like description.
   * @return the built graph.
   * @throw InvalidArgument if the description is malformed (an operator without a following
   *        node token, a node token which is not a node name, e.g. "A<" in "A<->B" or "A B" in
   *        "A B->C", a malformed quoted name), or if the nodes are NodeIds and the largest one
   *        leaves more than max(number of nodes, 65536) absent ids below it ("0->2000000000").
   * @throw InvalidArc if @p desc requires a directed arc but GRAPH_TYPE
   *        cannot represent arcs (not GUM_DiGraphable).
   * @throw InvalidEdge if @p desc requires an undirected edge but
   *        GRAPH_TYPE cannot represent edges (not GUM_UndiGraphable).
   * @throw InvalidDirectedCycle propagated as-is if GRAPH_TYPE is a DAG (or
   *        any type enforcing acyclicity) and @p desc would create a cycle.
   * @throw DuplicateElement propagated as-is if @p desc reuses a name across
   *        distinct nodes.
   */
  template < GUM_NodeGraphable GRAPH_TYPE = MixedGraph >
  GRAPH_TYPE fastGraph(std::string_view desc);

}   // namespace gum

#include <agrum/base/graphs/fastGraph_tpl.h>

#endif   // GUM_FAST_GRAPH_H
