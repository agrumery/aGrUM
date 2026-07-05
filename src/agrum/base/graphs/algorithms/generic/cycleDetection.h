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


/**
 * @file
 * @brief Generic cycle-detection algorithms for aGrUM graphs.
 *
 * All functions live in namespace gum::graph and are constrained by the
 * concepts defined in graphConcepts.h :
 *
 *   GUM_DiGraphable   — topologicalOrder
 *   GUM_UndiGraphable — hasUndirectedCycle
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_CYCLE_DETECTION_H
#define GUM_GRAPH_CYCLE_DETECTION_H

#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/sequence.h>
#include <agrum/base/graphs/graphConcepts.h>

namespace gum::graph {

  // =========================================================================
  // Directed graphs (GUM_DiGraphable)
  // =========================================================================

  /**
   * @brief Returns a topological ordering of the nodes of @p g (Kahn's algorithm).
   *
   * Nodes are ordered so that every arc goes from an earlier to a later
   * position in the sequence.
   *
   * @throw InvalidDirectedCycle if @p g contains a directed cycle.
   */
  template < GUM_DiGraphable G >
  Sequence< NodeId > topologicalOrder(const G& g);

  // =========================================================================
  // Undirected graphs (GUM_UndiGraphable)
  // =========================================================================

  /**
   * @brief Returns true if @p g contains at least one undirected cycle.
   *
   * Uses BFS per connected component, tracking the predecessor to avoid
   * treating the edge we came from as a back-edge.
   */
  template < GUM_UndiGraphable G >
  bool hasUndirectedCycle(const G& g);

}   // namespace gum::graph

#include <agrum/base/graphs/algorithms/generic/cycleDetection_tpl.h>

#endif   // GUM_GRAPH_CYCLE_DETECTION_H
