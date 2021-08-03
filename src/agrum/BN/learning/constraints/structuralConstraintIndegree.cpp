/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief the class for structural constraints limiting the number of parents
 * of nodes in a directed graph
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
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
    StructuralConstraintIndegree::StructuralConstraintIndegree(Size nb_nodes, Size max_indegree) :
        StructuralConstraintDiGraph(nb_nodes), _Indegree_max_indegree_(max_indegree) {
      for (NodeId i = 0; i < nb_nodes; ++i) {
        _Indegree_max_parents_.insert(i, max_indegree);
      }
      GUM_CONSTRUCTOR(StructuralConstraintIndegree);
    }

    /// constructor starting with a given graph
    StructuralConstraintIndegree::StructuralConstraintIndegree(const DiGraph& graph,
                                                               Size           max_indegree) :
        StructuralConstraintDiGraph(graph),
        _Indegree_max_indegree_(max_indegree) {
      for (const auto id: graph) {
        _Indegree_max_parents_.insert(id, max_indegree);
      }
      GUM_CONSTRUCTOR(StructuralConstraintIndegree);
    }

    /// copy constructor
    StructuralConstraintIndegree::StructuralConstraintIndegree(
       const StructuralConstraintIndegree& from) :
        StructuralConstraintDiGraph(from),
        _Indegree_max_parents_(from._Indegree_max_parents_),
        _Indegree_max_indegree_(from._Indegree_max_indegree_) {
      GUM_CONS_CPY(StructuralConstraintIndegree);
    }

    /// move constructor
    StructuralConstraintIndegree::StructuralConstraintIndegree(
       StructuralConstraintIndegree&& from) :
        StructuralConstraintDiGraph(std::move(from)),
        _Indegree_max_parents_(std::move(from._Indegree_max_parents_)),
        _Indegree_max_indegree_(std::move(from._Indegree_max_indegree_)) {
      GUM_CONS_MOV(StructuralConstraintIndegree);
    }

    /// destructor
    StructuralConstraintIndegree::~StructuralConstraintIndegree() {
      GUM_DESTRUCTOR(StructuralConstraintIndegree);
    }

    /// copy operator
    StructuralConstraintIndegree&
       StructuralConstraintIndegree::operator=(const StructuralConstraintIndegree& from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(from);
        _Indegree_max_parents_               = from._Indegree_max_parents_;
        _Indegree_max_indegree_              = from._Indegree_max_indegree_;
      }

      return *this;
    }

    /// move operator
    StructuralConstraintIndegree&
       StructuralConstraintIndegree::operator=(StructuralConstraintIndegree&& from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(std::move(from));
        _Indegree_max_parents_               = std::move(from._Indegree_max_parents_);
        _Indegree_max_indegree_              = std::move(from._Indegree_max_indegree_);
      }

      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
