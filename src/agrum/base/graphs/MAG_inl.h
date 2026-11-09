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
 * @brief Inline implementation of the maximal ancestral graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#pragma once

// to ease parsing by IDE
#include <agrum/base/graphs/algorithms/ancestral.h>
#include <agrum/base/graphs/MAG.h>

namespace gum {

  INLINE NodeSet MAG::neighbours(NodeId node) const { return graph_.undirectedNeighbours(node); }

  INLINE const NodeSet& MAG::adjacents(NodeId node) const { return graph_.neighbours(node); }

  INLINE NodeSet MAG::parents(NodeId node) const { return graph_.parents(node); }

  INLINE NodeSet MAG::children(NodeId node) const { return graph_.children(node); }

  INLINE NodeSet MAG::spouses(NodeId node) const { return graph_.spouses(node); }

  INLINE EdgeMark MAG::markAt(NodeId src, NodeId dst) const { return graph_.markAt(src, dst); }

  INLINE bool MAG::mSeparation(NodeId X, NodeId Y, const NodeSet& Z) const {
    return graph::mSeparation(graph_, NodeSet{X}, NodeSet{Y}, Z);
  }

  INLINE bool MAG::mSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const {
    return graph::mSeparation(graph_, X, Y, Z);
  }

}   // namespace gum
