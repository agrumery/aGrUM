
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
#  include <agrum/graphs/mixedGraph.h>

namespace gum {

  template < typename GS1, typename GS2 >
  void StructuralComparator::compare(const BayesNet< GS1 >& ref,
                                     const BayesNet< GS2 >& test) {
    if (ref.size() != test.size()) {
      GUM_ERROR(OperationNotAllowed, "Graphs of different sizes");
    }
    for (const NodeId node : ref.dag().asNodeSet()) {
      if (!test.dag().existsNode(node)) {
        GUM_ERROR(InvalidNode,
                  "Test doesn't contain node " << node << " from ref");
      }
    }

    MixedGraph ref_eg = EssentialGraph(ref).mixedGraph();
    MixedGraph test_eg = EssentialGraph(test).mixedGraph();

    this->compare(ref_eg, test_eg);
  }

  template < typename GUM_SCALAR >
  void StructuralComparator::compare(const BayesNet< GUM_SCALAR >& ref,
                                     const MixedGraph&             test) {
    MixedGraph ref_eg = EssentialGraph(ref).mixedGraph();
    this->compare(ref_eg, test);
  }

  template < typename GUM_SCALAR >
  void StructuralComparator::compare(const MixedGraph&             ref,
                                     const BayesNet< GUM_SCALAR >& test) {
    MixedGraph test_eg = EssentialGraph(test).mixedGraph();

    this->compare(ref, test_eg);
  }
} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
