
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief the base class for structural constraints imposed by DAGs
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/BN/learning/constraints/structuralConstraintDAG.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintDAG_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintDAG::StructuralConstraintDAG() {
      GUM_CONSTRUCTOR(StructuralConstraintDAG);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintDAG::StructuralConstraintDAG(Size nb_nodes) :
        StructuralConstraintDiGraph(nb_nodes) {
      StructuralConstraintDiGraph::setGraph(nb_nodes);

      DAG g;
      for (NodeId i = 0; i < nb_nodes; ++i) {
        g.addNodeWithId(i);
      }
      DAG__cycle_detector_.setDAG(g);

      GUM_CONSTRUCTOR(StructuralConstraintDAG);
    }

    /// constructor starting with a given graph
    StructuralConstraintDAG::StructuralConstraintDAG(const DAG& graph) {
      StructuralConstraintDiGraph::setGraph(graph);
      DAG__cycle_detector_.setDAG(graph);

      GUM_CONSTRUCTOR(StructuralConstraintDAG);
    }

    /// copy constructor
    StructuralConstraintDAG::StructuralConstraintDAG(
       const StructuralConstraintDAG& from) :
        StructuralConstraintDiGraph(from),
        DAG__cycle_detector_(from.DAG__cycle_detector_) {
      GUM_CONS_CPY(StructuralConstraintDAG);
    }

    /// move constructor
    StructuralConstraintDAG::StructuralConstraintDAG(
       StructuralConstraintDAG&& from) :
        StructuralConstraintDiGraph(std::move(from)),
        DAG__cycle_detector_(std::move(from.DAG__cycle_detector_)) {
      GUM_CONS_MOV(StructuralConstraintDAG);
    }

    /// destructor
    StructuralConstraintDAG::~StructuralConstraintDAG() {
      GUM_DESTRUCTOR(StructuralConstraintDAG);
    }

    /// copy operator
    StructuralConstraintDAG&
       StructuralConstraintDAG::operator=(const StructuralConstraintDAG& from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(from);
        DAG__cycle_detector_ = from.DAG__cycle_detector_;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintDAG&
       StructuralConstraintDAG::operator=(StructuralConstraintDAG&& from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(std::move(from));
        DAG__cycle_detector_ = std::move(from.DAG__cycle_detector_);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
