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

#include <string_view>

#include <agrum/agrum.h>

#include <agrum/base/graphs/mixedGraph.h>

namespace gum {

  /**
   * @brief Builds a GRAPH_TYPE from a DOT-like textual description.
   *
   * Syntax: "->" separates two node tokens with a directed arc, "<-" with a
   * directed arc in the other direction, "-" separates them with an
   * undirected edge, ";" separates independent chains, e.g. "A->B->C; B->E"
   * or "A->B<-C" (both A and C point to B) or "A->B-C".
   *
   * "-" (not "--") is used for edges on purpose: MRF's own fast syntax
   * already uses "--" to list the variables of a single factor (a clique),
   * a different construct from a chain of pairwise edges.
   *
   * If every node token in @p desc parses as a non-negative integer, those
   * integers are used directly as NodeIds (addNodeWithId). Otherwise every
   * token — including numeric-looking ones — is treated as a string name
   * (addNode + setName).
   *
   * @tparam GRAPH_TYPE Concrete graph type to build (DiGraph, UndiGraph,
   *         MixedGraph, DAG, PDAG, ...). Defaults to MixedGraph.
   * @param desc the DOT-like description.
   * @return the built graph.
   * @throw InvalidArc if @p desc requires a directed arc but GRAPH_TYPE
   *        cannot represent arcs (not GUM_DiGraphable), or if the
   *        description is malformed (e.g. an operator without a following
   *        node token).
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
