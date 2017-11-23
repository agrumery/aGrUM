/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/BN/algorithms/essentialGraph.h>
#include <agrum/graphs/mixedGraph.h>

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
