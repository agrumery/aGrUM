/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/algorithms/essentialGraph.h>
#  include <agrum/tools/graphs/PDAG.h>

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
