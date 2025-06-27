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
 * @brief Inline implementation of Base classes for mixed graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

namespace gum {

  INLINE MixedGraph& MixedGraph::operator=(const MixedGraph& g) {
    // avoid self assignment
    if (this != &g) {
      // remove the old graph properly
      EdgeGraphPart::clearEdges();
      ArcGraphPart::clearArcs();
      NodeGraphPart::clearNodes();

      // fill the new graph
      NodeGraphPart::operator=(g);
      EdgeGraphPart::operator=(g);
      ArcGraphPart:: operator=(g);
    }

    return *this;
  }

  INLINE void MixedGraph::clear() {
    EdgeGraphPart::clearEdges();
    ArcGraphPart::clearArcs();
    NodeGraphPart::clearNodes();
  }

  INLINE void MixedGraph::eraseNode(const NodeId node) {
    EdgeGraphPart::unvirtualizedEraseNeighbours(node);
    ArcGraphPart::unvirtualizedEraseParents(node);
    ArcGraphPart::unvirtualizedEraseChildren(node);
    NodeGraphPart::eraseNode(node);
  }

  INLINE bool MixedGraph::operator==(const MixedGraph& g) const {
    return ArcGraphPart::operator==(g) && EdgeGraphPart::operator==(g)
        && NodeGraphPart::operator==(g);
  }

  INLINE NodeSet MixedGraph::boundary(NodeId node) const {
    return neighbours(node) + parents(node) + children(node);
  }
} /* namespace gum */
