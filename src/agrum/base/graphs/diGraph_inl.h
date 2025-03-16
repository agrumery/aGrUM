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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/** @file
 * @brief Inline implementation of Base classes for oriented graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// to ease parser in IDEs
#include <agrum/base/graphs/diGraph.h>

namespace gum {

  INLINE void DiGraph::addArc(const NodeId tail, const NodeId head) {
    if (!exists(head)) { GUM_ERROR(InvalidNode, "no head node : " << head) }

    if (!exists(tail)) { GUM_ERROR(InvalidNode, "no tail node : " << tail) }

    ArcGraphPart::addArc(tail, head);
  }

  INLINE void DiGraph::clear() {
    ArcGraphPart::clearArcs();
    NodeGraphPart::clearNodes();
  }

  INLINE DiGraph& DiGraph::operator=(const DiGraph& g) {
    // avoid self assigment
    if (this != &g) {
      DiGraph::clear();
      NodeGraphPart::operator=(g);
      ArcGraphPart::operator=(g);
    }

    return *this;
  }

  INLINE void DiGraph::eraseNode(const NodeId id) {
    // warning: to remove the arcs adjacent to id, use the unvirtualized
    // versions
    // of arc removals
    ArcGraphPart::unvirtualizedEraseParents(id);
    ArcGraphPart::unvirtualizedEraseChildren(id);

    NodeGraphPart::eraseNode(id);
  }

  INLINE bool DiGraph::operator==(const DiGraph& p) const {
    return ArcGraphPart::operator==(p) && NodeGraphPart::operator==(p);
  }

  // INLINE bool DiGraph::operator!=(const DiGraph& p) const { return !operator==(p); }

} /* namespace gum */
