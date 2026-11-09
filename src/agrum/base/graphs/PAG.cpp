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
 * @brief Partial Ancestral Graph (PAG) — implementation.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/base/graphs/PAG.h>

namespace gum {

  PAG::PAG() { GUM_CONSTRUCTOR(PAG); }

  PAG::PAG(const PAG& other) : NodeGraphPart(other), MarkedGraph(other) { GUM_CONS_CPY(PAG); }

  PAG::PAG(PAG&& other) : NodeGraphPart(std::move(other)), MarkedGraph(std::move(other)) {
    GUM_CONS_MOV(PAG);
  }

  PAG::~PAG() { GUM_DESTRUCTOR(PAG); }

  PAG& PAG::operator=(const PAG& other) {
    MarkedGraph::operator=(other);
    return *this;
  }

  PAG& PAG::operator=(PAG&& other) {
    MarkedGraph::operator=(std::move(other));
    return *this;
  }

  MixedGraph PAG::toMixedGraph() const {
    MixedGraph g;
    for (const NodeId n: nodes()) {
      g.addNodeWithId(n);
      if (hasName(n)) g.setName(n, nameFromId(n));
    }

    for (const Edge& e: edges()) {
      const NodeId x      = e.first();
      const NodeId y      = e.second();
      const bool   xArrow = isArrowhead(y, x);   // arrowhead at x-endpoint
      const bool   yArrow = isArrowhead(x, y);   // arrowhead at y-endpoint
      if (xArrow && yArrow) {
        g.addArc(x, y);
        g.addArc(y, x);
      } else if (yArrow) {
        g.addArc(x, y);
      } else if (xArrow) {
        g.addArc(y, x);
      } else {
        g.addEdge(x, y);
      }
    }
    return g;
  }

} /* namespace gum */
