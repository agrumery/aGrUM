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
 * @brief Partial Ancestral Graph (PAG) — graph type for FCI output.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_PAG_H
#define GUM_PAG_H

#include <agrum/agrum.h>

#include <agrum/base/graphs/markedGraph.h>
#include <agrum/base/graphs/mixedGraph.h>

namespace gum {

  /** @class PAG
   * @brief Partial Ancestral Graph: a marked graph representing a Markov equivalence class of
   * maximal ancestral graphs (MAG).
   *
   * An arrowhead (resp. a tail) of a PAG is in all the MAGs of the class, a circle varies across
   * the class (Zhang 2008). The edges of a PAG are:
   *   - o-o (Circle-Circle)   : completely uncertain
   *   - o-> (Circle-Arrowhead): possibly directed
   *   - ->  (Tail-Arrowhead)  : definitely directed
   *   - <-> (Arrowhead-Arrowhead) : bidirected (latent confounder)
   *   - --  (Tail-Tail)       : definitely undirected (selection bias)
   *
   * A PAG is the output of FCI: no invariant is checked (the output of FCI on finite data may not
   * be a valid PAG).
   *
   * @par References
   * - J. Zhang. On the completeness of orientation rules for causal discovery in the presence of
   *   latent confounders and selection bias. Artificial Intelligence 172(16-17):1873-1896, 2008.
   *   doi:10.1016/j.artint.2008.08.001.
   *
   * @ingroup graph_group
   */
  class GUM_SHARED_PUBLIC PAG: public MarkedGraph {
    public:
    // ########################################################################
    /// @name Constructors / Destructors
    // ########################################################################
    /// @{

    PAG();
    PAG(const PAG&);
    PAG(PAG&&);
    ~PAG() override;

    PAG& operator=(const PAG&);
    PAG& operator=(PAG&&);

    /// @}

    /// approximate conversion: Tail-Arrowhead → arc; Arrowhead-Arrowhead → two arcs; anything
    /// else → undirected edge
    /** @deprecated deprecated after 3.3.2: a PAG is not a mixed graph (the Meek rules do not apply
     * to it); use MAG::fromPAG for a representative MAG of the PAG. */
    [[deprecated("PAG::toMixedGraph is deprecated after 3.3.2, use MAG::fromPAG")]]
    MixedGraph toMixedGraph() const;
  };

} /* namespace gum */

#endif /* GUM_PAG_H */
