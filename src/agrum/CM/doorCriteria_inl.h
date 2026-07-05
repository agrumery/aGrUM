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

#include <agrum/CM/doorCriteria.h>

namespace gum {


  INLINE std::optional< NodeSet > DoorCriteria::firstBackdoor(const DAG&     dag,
                                                              NodeId         X,
                                                              NodeId         Y,
                                                              const NodeSet& excluded_nodes,
                                                              std::size_t    max_cardinality,
                                                              bool           only_minimal) {
    auto bds
        = enumerateBackdoorSets(dag, X, Y, excluded_nodes, max_cardinality, only_minimal, true);
    if (bds.empty()) { return std::nullopt; }
    return bds.front();
  }

  INLINE std::optional< NodeSet > DoorCriteria::firstFrontdoor(const DAG&     dag,
                                                               NodeId         X,
                                                               NodeId         Y,
                                                               const NodeSet& excluded_nodes,
                                                               std::size_t    max_cardinality,
                                                               bool           only_minimal) {
    auto fds
        = enumerateFrontdoorSets(dag, X, Y, excluded_nodes, max_cardinality, only_minimal, true);
    if (fds.empty()) { return std::nullopt; }
    return fds.front();
  }

}   // namespace gum
