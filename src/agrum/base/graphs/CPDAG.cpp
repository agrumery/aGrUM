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
 * @brief Outlined implementation of the completed PDAGs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <agrum/base/graphs/algorithms/orientation.h>
#include <agrum/base/graphs/CPDAG.h>

namespace gum {

  CPDAG::CPDAG(ChainGraph&& g, DAG&& extension) :
      ImmutableMixedGraph< ChainGraph >(std::move(g), std::move(extension)) {
    GUM_CONSTRUCTOR(CPDAG);
  }

  CPDAG::CPDAG(const CPDAG& g) : ImmutableMixedGraph< ChainGraph >(g) { GUM_CONS_CPY(CPDAG); }

  CPDAG::CPDAG(CPDAG&& g) : ImmutableMixedGraph< ChainGraph >(std::move(g)) { GUM_CONS_MOV(CPDAG); }

  CPDAG::~CPDAG() { GUM_DESTRUCTOR(CPDAG); }

  CPDAG& CPDAG::operator=(const CPDAG& g) {
    if (this != &g) {
      ImmutableMixedGraph< ChainGraph >::operator=(g);
      GUM_OP_CPY(CPDAG);
    }
    return *this;
  }

  CPDAG& CPDAG::operator=(CPDAG&& g) {
    if (this != &g) {
      ImmutableMixedGraph< ChainGraph >::operator=(std::move(g));
      GUM_OP_MOV(CPDAG);
    }
    return *this;
  }

  CPDAG CPDAG::fromDAG(const DAG& dag) { return CPDAG(graph::cpdag(dag), DAG(dag)); }

  CPDAG CPDAG::fromPDAG(const PDAG& pdag) {
    auto extension = graph::consistentExtension(pdag);
    if (!extension.has_value())
      GUM_ERROR(InvalidArgument, "The PDAG is not a CPDAG: it has no consistent extension")
    auto cpdag = graph::cpdag(*extension);
    if (!(static_cast< const MixedGraph& >(cpdag) == static_cast< const MixedGraph& >(pdag)))
      GUM_ERROR(InvalidArgument,
                "The PDAG is not a CPDAG: it is not the CPDAG of its consistent extensions")
    return CPDAG(std::move(cpdag), std::move(*extension));
  }

  bool CPDAG::contains(const DAG& dag) const {
    if (dag.size() != size() || dag.sizeArcs() != sizeArcs() + sizeEdges()) return false;
    return graph::cpdag(dag) == graph_;
  }

}   // namespace gum
