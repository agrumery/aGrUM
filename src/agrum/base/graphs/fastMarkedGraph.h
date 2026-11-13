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
 * @brief Builds a marked graph from a "fast" DOT-like textual description.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_FAST_MARKED_GRAPH_H
#define GUM_FAST_MARKED_GRAPH_H

#include <concepts>
#include <string>
#include <string_view>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/graphs/fastGraph.h>
#include <agrum/base/graphs/markedGraph.h>

namespace gum {

#ifndef SWIG
  namespace graph::detail {

    /// a node token of a marked graph and the marks of the edge which precedes it (at the previous
    /// node, at this node)
    struct FastMarkedToken {
      std::string token;
      EdgeMark    markAtPrevious;
      EdgeMark    markAtThis;
    };

    /**
     * @brief Scans one ";"-delimited chain of a marked graph: each '-' (outside brackets) is an
     * edge, with the mark just before it at the previous node and the mark just after it at the
     * next node: '<' or '>' is an arrowhead, 'o' is a circle when it is separated by a space from
     * a node name on its side of the edge or when it touches a quoted name ("'A'o->'B'"), anything
     * else is a tail. Hence 'o' is a node name in "o->B", "A->o->B" or "A-o".
     */
    GUM_SHARED_PUBLIC std::vector< FastMarkedToken >
                      fastMarkedGraphTokenize(std::string_view chain);

  }   // namespace graph::detail
#endif   // SWIG

  /**
   * @brief Builds a marked graph (MarkedGraph, PAG) from a DOT-like textual description.
   *
   * Syntax: as for fastGraph(), with an edge given by its two marks around a '-': '<' or '>' for
   * an arrowhead, 'o' for a circle, nothing for a tail, e.g. "A->B; B<->C; C o-> D; D o-o E; E-F"
   * ("A-F" has a tail at both ends). A circle must be separated from the name of its node by a
   * space ("C o-> D", not "Co->D", where "Co" would be a node). Otherwise, and when no node name
   * lies on its side of the edge, 'o' is read as a node name: "o->B", "A->o->B" and "A-o" use
   * a node named "o". A name may be quoted as in fastGraph() ("'Visit Asia' o-> X"); a circle may
   * touch a quoted name ("'A'o->'B'", "A <-o'B'").
   *
   * An edge may be described several times with the same marks ("A->B; A->B"), but not with
   * different ones ("A->B; B->A", "A o-o B; A->B").
   *
   * @tparam GRAPH_TYPE MarkedGraph or one of its subclasses (PAG).
   * @param desc the DOT-like description.
   * @return the built graph.
   * @throw InvalidArgument if the description is malformed (an operator without a following node
   *        token, a node token which is not a node name: e.g. "A>" in "A>-B", "A o" in "A o -B", a
   *        malformed quoted name), or if the nodes are NodeIds and the largest one leaves more
   *        than max(number of nodes, 65536) absent ids below it (e.g. "0->2000000000").
   * @throw InvalidEdge if an edge is described with conflicting marks.
   * @throw DuplicateElement propagated as-is if @p desc reuses a name across distinct nodes.
   */
  template < typename GRAPH_TYPE = MarkedGraph >
#ifndef SWIG   // SWIG 4.3 cannot read a requires-clause
    requires std::derived_from< GRAPH_TYPE, MarkedGraph >
#endif
  GRAPH_TYPE fastMarkedGraph(std::string_view desc);

}   // namespace gum

#include <agrum/base/graphs/fastMarkedGraph_tpl.h>

#endif   // GUM_FAST_MARKED_GRAPH_H
