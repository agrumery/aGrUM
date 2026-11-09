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


#pragma once


/** @file
 * @brief Inline implementation of chain graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

// to ease IDE parser
#include <agrum/base/core/exceptions.h>
#include <agrum/base/graphs/chainGraph.h>

namespace gum {

  INLINE ChainGraph& ChainGraph::operator=(const ChainGraph& g) {
    if (this != &g) {
      PDAG::operator=(g);
      GUM_OP_CPY(ChainGraph);
    }
    return *this;
  }

  INLINE ChainGraph& ChainGraph::operator=(ChainGraph&& g) {
    if (this != &g) {
      PDAG::operator=(std::move(g));
      GUM_OP_MOV(ChainGraph);
    }
    return *this;
  }

  INLINE void ChainGraph::addArc(const NodeId tail, const NodeId head) {
    // the new arc tail->head closes a partially directed cycle with any path from head to tail
    // following the arcs in their direction and the edges in either direction, even without arc
    if (head != tail && !this->existsEdge(tail, head) && !this->hasDirectedPath(head, tail)
        && this->hasMixedOrientedPath(head, tail)) {
      GUM_ERROR(InvalidPartiallyDirectedCycle,
                "Add a partially directed cycle in a chain graph : between " << head << " and "
                                                                             << tail)
    }
    PDAG::addArc(tail, head);
  }

  INLINE void ChainGraph::addEdge(const NodeId first, const NodeId second) {
    if (first != second && !this->existsArc(first, second) && !this->existsArc(second, first)
        && (this->hasMixedReallyOrientedPath(first, second)
            || this->hasMixedReallyOrientedPath(second, first))) {
      GUM_ERROR(InvalidPartiallyDirectedCycle,
                "Add a partially directed cycle in a chain graph : between " << first << " and "
                                                                             << second)
    }
    PDAG::addEdge(first, second);
  }

} /* namespace gum */
