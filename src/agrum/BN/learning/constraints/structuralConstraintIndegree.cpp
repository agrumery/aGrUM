
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
 * @brief the class for structural constraints limiting the number of parents
 * of nodes in a directed graph
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/learning/constraints/structuralConstraintIndegree.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintIndegree_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintIndegree::StructuralConstraintIndegree() {
      GUM_CONSTRUCTOR(StructuralConstraintIndegree);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintIndegree::StructuralConstraintIndegree(Size nb_nodes,
                                                               Size max_indegree) :
        StructuralConstraintDiGraph(nb_nodes),
        _Indegree__max_indegree(max_indegree) {
      for (NodeId i = 0; i < nb_nodes; ++i) {
        _Indegree__max_parents.insert(i, max_indegree);
      }
      GUM_CONSTRUCTOR(StructuralConstraintIndegree);
    }

    /// constructor starting with a given graph
    StructuralConstraintIndegree::StructuralConstraintIndegree(
       const DiGraph& graph, Size max_indegree) :
        StructuralConstraintDiGraph(graph),
        _Indegree__max_indegree(max_indegree) {
      for (const auto id: graph) {
        _Indegree__max_parents.insert(id, max_indegree);
      }
      GUM_CONSTRUCTOR(StructuralConstraintIndegree);
    }

    /// copy constructor
    StructuralConstraintIndegree::StructuralConstraintIndegree(
       const StructuralConstraintIndegree& from) :
        StructuralConstraintDiGraph(from),
        _Indegree__max_parents(from._Indegree__max_parents),
        _Indegree__max_indegree(from._Indegree__max_indegree) {
      GUM_CONS_CPY(StructuralConstraintIndegree);
    }

    /// move constructor
    StructuralConstraintIndegree::StructuralConstraintIndegree(
       StructuralConstraintIndegree&& from) :
        StructuralConstraintDiGraph(std::move(from)),
        _Indegree__max_parents(std::move(from._Indegree__max_parents)),
        _Indegree__max_indegree(std::move(from._Indegree__max_indegree)) {
      GUM_CONS_MOV(StructuralConstraintIndegree);
    }

    /// destructor
    StructuralConstraintIndegree::~StructuralConstraintIndegree() {
      GUM_DESTRUCTOR(StructuralConstraintIndegree);
    }

    /// copy operator
    StructuralConstraintIndegree& StructuralConstraintIndegree::operator=(
       const StructuralConstraintIndegree& from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(from);
        _Indegree__max_parents = from._Indegree__max_parents;
        _Indegree__max_indegree = from._Indegree__max_indegree;
      }

      return *this;
    }

    /// move operator
    StructuralConstraintIndegree& StructuralConstraintIndegree::operator=(
       StructuralConstraintIndegree&& from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(std::move(from));
        _Indegree__max_parents = std::move(from._Indegree__max_parents);
        _Indegree__max_indegree = std::move(from._Indegree__max_indegree);
      }

      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
