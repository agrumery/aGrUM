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
 * @brief Inline implementation of the marked graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#pragma once

// to ease parsing by IDE
#include <agrum/base/graphs/markedGraph.h>

namespace gum {

  INLINE bool MarkedGraph::isArrowhead(NodeId src, NodeId dst) const {
    return markAt(src, dst) == EdgeMark::Arrowhead;
  }

  INLINE bool MarkedGraph::isTail(NodeId src, NodeId dst) const {
    return markAt(src, dst) == EdgeMark::Tail;
  }

  INLINE bool MarkedGraph::isCircle(NodeId src, NodeId dst) const {
    return markAt(src, dst) == EdgeMark::Circle;
  }

  INLINE bool MarkedGraph::isDefinitelyDirected(NodeId x, NodeId y) const {
    return isTail(y, x) && isArrowhead(x, y);
  }

  INLINE bool MarkedGraph::isBidirected(NodeId x, NodeId y) const {
    return isArrowhead(x, y) && isArrowhead(y, x);
  }

  INLINE bool MarkedGraph::isUndirected(NodeId x, NodeId y) const {
    return isTail(x, y) && isTail(y, x);
  }

  INLINE bool MarkedGraph::isDefCollider(NodeId x, NodeId z, NodeId y) const {
    return isArrowhead(x, z) && isArrowhead(y, z);
  }

}   // namespace gum
