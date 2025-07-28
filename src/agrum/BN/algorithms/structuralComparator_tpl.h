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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/base/graphs/PDAG.h>
#  include <agrum/BN/algorithms/essentialGraph.h>

namespace gum {

  template < typename GS1, typename GS2 >
  void StructuralComparator::compare(const BayesNet< GS1 >& ref, const BayesNet< GS2 >& test) {
    if (ref.size() != test.size()) { GUM_ERROR(OperationNotAllowed, "Graphs of different sizes") }
    for (const NodeId node: ref.dag().asNodeSet()) {
      if (!test.dag().existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain node " << node << " from ref")
      }
    }
    PDAG ref_eg  = EssentialGraph(ref).pdag();
    auto eg      = EssentialGraph(test);
    PDAG test_eg = eg.pdag();

    this->compare(ref_eg, test_eg);
  }

  template < typename GUM_SCALAR >
  void StructuralComparator::compare(const BayesNet< GUM_SCALAR >& ref, const PDAG& test) {
    PDAG ref_eg = EssentialGraph(ref).pdag();
    this->compare(ref_eg, test);
  }

  template < typename GUM_SCALAR >
  void StructuralComparator::compare(const PDAG& ref, const BayesNet< GUM_SCALAR >& test) {
    PDAG test_eg = EssentialGraph(test).pdag();

    this->compare(ref, test_eg);
  }
} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
