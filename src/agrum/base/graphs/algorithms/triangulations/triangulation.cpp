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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief Abstract base class for computing triangulations of graphs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/agrum.h>

#include <agrum/base/graphs/algorithms/triangulations/triangulation.h>

#include <agrum/base/core/math/math_utils.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/algorithms/triangulations/triangulation_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  // constructor
  Triangulation::Triangulation() {   // for debugging purposes
    GUM_CONSTRUCTOR(Triangulation);
  }

  // constructor with a domain size specified
  Triangulation::Triangulation(const NodeProperty< Size >* domsizes) : domain_sizes_(domsizes) {
    GUM_CONSTRUCTOR(Triangulation);
  }

  // destructor
  Triangulation::~Triangulation() {   // for debugging purposes
    GUM_DESTRUCTOR(Triangulation);
  }

  // copy constructor
  Triangulation::Triangulation(const Triangulation& from) : domain_sizes_(from.domain_sizes_) {
    GUM_CONS_CPY(Triangulation);
  }

  // move constructor
  Triangulation::Triangulation(Triangulation&& from) : domain_sizes_(from.domain_sizes_) {
    GUM_CONS_MOV(Triangulation);
  }

  // returns the max of log10DomainSize of cliques in the junction tree
  double Triangulation::maxLog10CliqueDomainSize() {
    double              res = 0.0;
    double              dSize;
    const JunctionTree& jt = junctionTree();   // here, the fact that we get
    // a junction tree ensures that domain_sizes_ is different from nullptr

    for (const NodeId cl: jt) {
      dSize = 0.0;

      for (const auto node: jt.clique(cl))
        dSize += std::log10((*domain_sizes_)[node]);

      if (res < dSize) res = dSize;
    }

    return res;
  }

} /* namespace gum */
