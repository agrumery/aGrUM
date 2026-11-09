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
 * @brief The separation criterion of each kind of graph: d-separation for DAGs and the classes of
 * DAGs (CPDAG, MPDAG), c-separation for PDAGs and chain graphs, m-separation for MAGs.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_SEPARATION_CRITERIA_H
#define GUM_GRAPH_SEPARATION_CRITERIA_H

#include <agrum/agrum.h>

#include <agrum/base/graphs/PDAG.h>

namespace gum {
  class CPDAG;
  class MPDAG;
  class MAG;
}   // namespace gum

namespace gum::graph {

  /**
   * @name Separation of @p X and @p Y by @p Z, with the criterion of the kind of graph
   *
   * The three criteria are equal on a DAG. As for each of them, the nodes of @p X and @p Y which
   * belong to @p Z are observed, hence separated from any node.
   *
   * @throw InvalidArgument if @p X and @p Y are not disjoint
   */
  /// @{

  /// d-separation (graph::dSeparated)
  GUM_SHARED_PUBLIC bool
      separation(const DAG& g, const NodeSet& X, const NodeSet& Y, const NodeSet& Z);

  /// c-separation (graph::cSeparated), for a PDAG or a chain graph
  GUM_SHARED_PUBLIC bool
      separation(const PDAG& g, const NodeSet& X, const NodeSet& Y, const NodeSet& Z);

  /// d-separation in the DAGs of the class (CPDAG::dSeparation)
  GUM_SHARED_PUBLIC bool
      separation(const CPDAG& g, const NodeSet& X, const NodeSet& Y, const NodeSet& Z);

  /// d-separation in the DAGs of the class (MPDAG::dSeparation)
  GUM_SHARED_PUBLIC bool
      separation(const MPDAG& g, const NodeSet& X, const NodeSet& Y, const NodeSet& Z);

  /// m-separation (MAG::mSeparation)
  GUM_SHARED_PUBLIC bool
      separation(const MAG& g, const NodeSet& X, const NodeSet& Y, const NodeSet& Z);

  /// @}

}   // namespace gum::graph

#endif   // GUM_GRAPH_SEPARATION_CRITERIA_H
