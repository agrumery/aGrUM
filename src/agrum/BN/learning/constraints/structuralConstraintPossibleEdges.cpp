
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


/** @file
 * @brief the structural constraint for forbidding the creation of some arcs
 * during structure learning
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/learning/constraints/structuralConstraintPossibleEdges.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintPossibleEdges_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintPossibleEdges::StructuralConstraintPossibleEdges() {
      GUM_CONSTRUCTOR(StructuralConstraintPossibleEdges);
    }

    /// constructor starting with a given graph
    StructuralConstraintPossibleEdges::StructuralConstraintPossibleEdges(
       const DiGraph& graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintPossibleEdges);
    }

    /// copy constructor
    StructuralConstraintPossibleEdges::StructuralConstraintPossibleEdges(
       const StructuralConstraintPossibleEdges& from) :
        _PossibleEdges__possible_edges(from._PossibleEdges__possible_edges) {
      GUM_CONS_CPY(StructuralConstraintPossibleEdges);
    }

    /// move constructor
    StructuralConstraintPossibleEdges::StructuralConstraintPossibleEdges(
       StructuralConstraintPossibleEdges&& from) :
        _PossibleEdges__possible_edges(
           std::move(from._PossibleEdges__possible_edges)) {
      GUM_CONS_MOV(StructuralConstraintPossibleEdges);
    }

    /// destructor
    StructuralConstraintPossibleEdges::~StructuralConstraintPossibleEdges() {
      GUM_DESTRUCTOR(StructuralConstraintPossibleEdges);
    }

    /// copy operator
    StructuralConstraintPossibleEdges&
       StructuralConstraintPossibleEdges::operator=(
          const StructuralConstraintPossibleEdges& from) {
      if (this != &from) {
        _PossibleEdges__possible_edges = from._PossibleEdges__possible_edges;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintPossibleEdges&
       StructuralConstraintPossibleEdges::operator=(
          StructuralConstraintPossibleEdges&& from) {
      if (this != &from) {
        _PossibleEdges__possible_edges =
           std::move(from._PossibleEdges__possible_edges);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
