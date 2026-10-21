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
 * @brief Selection of the roots of the connected components of a junction tree
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_JUNCTION_TREE_ROOTS_H
#define GUM_JUNCTION_TREE_ROOTS_H

#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/graphicalModel.h>
#include <agrum/base/graphs/cliqueGraph.h>

namespace gum {

  /**
   * @brief Returns one root for each connected component of a junction tree.
   *
   * In each connected component, the root is a clique of preferred if the component contains
   * some, otherwise any clique of the component. Among the candidates, the clique with the
   * smallest domain (the product of the domain sizes of its variables) is selected. The domain
   * sizes are compared through their logarithms, so that large cliques cannot overflow.
   *
   * Complexity: linear in the size of the junction tree (no sort, no recursion).
   *
   * @param jt the junction tree
   * @param model the graphical model whose variables are the nodes of the cliques of jt
   * @param preferred the cliques that should be preferred as roots (e.g. those containing targets)
   * @return the set of roots, exactly one per connected component of jt
   */
  GUM_SHARED_PUBLIC NodeSet junctionTreeRoots(const JunctionTree&   jt,
                                              const GraphicalModel& model,
                                              const NodeSet&        preferred);

}   // namespace gum

#endif   // GUM_JUNCTION_TREE_ROOTS_H
