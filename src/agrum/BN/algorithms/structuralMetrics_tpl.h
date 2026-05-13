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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/base/graphs/PDAG.h>
#  include <agrum/BN/algorithms/essentialGraph.h>

namespace gum {

  template < typename GS1, typename GS2 >
  void StructuralMetrics::compare(const BayesNet< GS1 >& ref, const BayesNet< GS2 >& test) {
    if (ref.size() != test.size()) { GUM_ERROR(OperationNotAllowed, "Graphs of different sizes") }
    for (const NodeId node: ref.dag().asNodeSet()) {
      if (!test.exists(ref.variable(node).name())) {
        GUM_ERROR(InvalidNode, "Test doesn't contain node " << node << " from ref")
      }
    }
    // Build a BN with ref's variables/NodeIds and test's arc structure (mapped
    // by name): comparison and essential-graph extraction must operate on
    // matching NodeIds, but the semantic identity is the variable name.
    BayesNet< GS2 > aligned_test;
    for (const NodeId id: ref.dag().asNodeSet()) {
      aligned_test.add(ref.variable(id), id);
    }
    for (const Arc& arc: test.dag().arcs()) {
      const NodeId tail = ref.idFromName(test.variable(arc.tail()).name());
      const NodeId head = ref.idFromName(test.variable(arc.head()).name());
      aligned_test.addArc(tail, head);
    }

    PDAG ref_eg  = EssentialGraph(ref).pdag();
    PDAG test_eg = EssentialGraph(aligned_test).pdag();
    this->compare(ref_eg, test_eg);
  }

  template < GUM_Numeric GUM_SCALAR >
  void StructuralMetrics::compare(const BayesNet< GUM_SCALAR >& ref, const PDAG& test) {
    PDAG ref_eg = EssentialGraph(ref).pdag();
    this->compare(ref_eg, test);
  }

  template < GUM_Numeric GUM_SCALAR >
  void StructuralMetrics::compare(const PDAG& ref, const BayesNet< GUM_SCALAR >& test) {
    PDAG test_eg = EssentialGraph(test).pdag();

    this->compare(ref, test_eg);
  }

  template < typename GS1, typename GS2 >
  double StructuralMetrics::sid(const BayesNet< GS1 >& ref, const BayesNet< GS2 >& test) const {
    if (ref.size() != test.size()) { GUM_ERROR(OperationNotAllowed, "Graphs of different sizes") }
    for (const NodeId node: ref.dag().asNodeSet()) {
      if (!test.exists(ref.variable(node).name())) {
        GUM_ERROR(InvalidNode, "Test doesn't contain node " << node << " from ref")
      }
    }
    // Align test's DAG to ref's NodeIds by variable name (DAG-level only,
    // no need to materialize an aligned BN since SID works on the DAG).
    DAG aligned_test;
    for (const NodeId id: ref.dag().asNodeSet()) {
      aligned_test.addNodeWithId(id);
    }
    for (const Arc& arc: test.dag().arcs()) {
      const NodeId tail = ref.idFromName(test.variable(arc.tail()).name());
      const NodeId head = ref.idFromName(test.variable(arc.head()).name());
      aligned_test.addArc(tail, head);
    }
    return this->sid(ref.dag(), aligned_test);
  }
} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
