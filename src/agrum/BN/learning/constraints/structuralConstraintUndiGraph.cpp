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
 * @brief the base class for structural constraints used by learning algorithms
 * that learn an undirected graph structure
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintUndiGraph.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintUndiGraph_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph() {
      GUM_CONSTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(Size nb_nodes) {
      setGraph(nb_nodes);
      GUM_CONSTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// constructor starting with a given graph
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(const UndiGraph& graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// copy constructor
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(
       const StructuralConstraintUndiGraph& from) :
        _UndiGraph_graph_(from._UndiGraph_graph_) {
      GUM_CONS_CPY(StructuralConstraintUndiGraph);
    }

    /// move constructor
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(
       StructuralConstraintUndiGraph&& from) :
        _UndiGraph_graph_(std::move(from._UndiGraph_graph_)) {
      GUM_CONS_MOV(StructuralConstraintUndiGraph);
    }

    /// destructor
    StructuralConstraintUndiGraph::~StructuralConstraintUndiGraph() {
      GUM_DESTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// copy operator
    StructuralConstraintUndiGraph&
       StructuralConstraintUndiGraph::operator=(const StructuralConstraintUndiGraph& from) {
      if (this != &from) { _UndiGraph_graph_ = from._UndiGraph_graph_; }
      return *this;
    }

    /// move operator
    StructuralConstraintUndiGraph&
       StructuralConstraintUndiGraph::operator=(StructuralConstraintUndiGraph&& from) {
      if (this != &from) { _UndiGraph_graph_ = std::move(from._UndiGraph_graph_); }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
