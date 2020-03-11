
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


/** @file
 * @brief the base class for structural constraints used by learning algorithms
 * that learn a directed graph structure
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintDiGraph.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintDiGraph_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintDiGraph::StructuralConstraintDiGraph() {
      GUM_CONSTRUCTOR(StructuralConstraintDiGraph);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintDiGraph::StructuralConstraintDiGraph(Size nb_nodes) {
      setGraph(nb_nodes);
      GUM_CONSTRUCTOR(StructuralConstraintDiGraph);
    }

    /// constructor starting with a given graph
    StructuralConstraintDiGraph::StructuralConstraintDiGraph(
       const DiGraph& graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintDiGraph);
    }

    /// copy constructor
    StructuralConstraintDiGraph::StructuralConstraintDiGraph(
       const StructuralConstraintDiGraph& from) :
        _DiGraph__graph(from._DiGraph__graph) {
      GUM_CONS_CPY(StructuralConstraintDiGraph);
    }

    /// move constructor
    StructuralConstraintDiGraph::StructuralConstraintDiGraph(
       StructuralConstraintDiGraph&& from) :
        _DiGraph__graph(std::move(from._DiGraph__graph)) {
      GUM_CONS_MOV(StructuralConstraintDiGraph);
    }

    /// destructor
    StructuralConstraintDiGraph::~StructuralConstraintDiGraph() {
      GUM_DESTRUCTOR(StructuralConstraintDiGraph);
    }

    /// copy operator
    StructuralConstraintDiGraph& StructuralConstraintDiGraph::operator=(
       const StructuralConstraintDiGraph& from) {
      if (this != &from) { _DiGraph__graph = from._DiGraph__graph; }
      return *this;
    }

    /// move operator
    StructuralConstraintDiGraph&
       StructuralConstraintDiGraph::operator=(StructuralConstraintDiGraph&& from) {
      if (this != &from) { _DiGraph__graph = std::move(from._DiGraph__graph); }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
