/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Inline implementation of Base classes for undirected graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// to ease parser
#include <agrum/base/graphs/undiGraph.h>

namespace gum {

  INLINE void UndiGraph::addEdge(const NodeId first, const NodeId second) {
    if (!exists(first)) { GUM_ERROR(InvalidNode, "Node (" << first << ") does not exist.") }

    if (!exists(second)) { GUM_ERROR(InvalidNode, "Node (" << second << ") does not exist.") }

    EdgeGraphPart::addEdge(second, first);
  }

  INLINE void UndiGraph::clear() {
    EdgeGraphPart::clearEdges();
    NodeGraphPart::clearNodes();
  }

  INLINE UndiGraph& UndiGraph::operator=(const UndiGraph& g) {
    if (this != &g) {
      UndiGraph::clear();
      NodeGraphPart::operator=(g);
      EdgeGraphPart::operator=(g);
    }

    return *this;
  }

  INLINE void UndiGraph::eraseNode(const NodeId id) {
    // warning: to remove the edges adjacent to id, use the unvirtualized
    // versions
    // of edge removals
    EdgeGraphPart::unvirtualizedEraseNeighbours(id);
    NodeGraphPart::eraseNode(id);
  }

  INLINE bool UndiGraph::operator==(const UndiGraph& p) const {
    return EdgeGraphPart::operator==(p) && NodeGraphPart::operator==(p);
  }

  INLINE bool UndiGraph::operator!=(const UndiGraph& p) const { return !operator==(p); }

} /* namespace gum */
