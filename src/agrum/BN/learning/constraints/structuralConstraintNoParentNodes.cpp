/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief the structural constraint for forbidding the creation of some arcs
 * during structure learning
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintNoParentNodes.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintNoParentNodes_inl.h>
#endif /* GUM_NO_INLINE */


namespace gum::learning {

  /// default constructor
  StructuralConstraintNoParentNodes::StructuralConstraintNoParentNodes() {
    GUM_CONSTRUCTOR(StructuralConstraintNoParentNodes);
  }

  /// constructor starting with a given graph
  StructuralConstraintNoParentNodes::StructuralConstraintNoParentNodes(const DiGraph& graph) {
    setGraph(graph);
    GUM_CONSTRUCTOR(StructuralConstraintNoParentNodes);
  }

  /// copy constructor
  StructuralConstraintNoParentNodes::StructuralConstraintNoParentNodes(
      const StructuralConstraintNoParentNodes& from) : _noParentNodes_(from._noParentNodes_) {
    GUM_CONS_CPY(StructuralConstraintNoParentNodes);
  }

  /// move constructor
  StructuralConstraintNoParentNodes::StructuralConstraintNoParentNodes(
      StructuralConstraintNoParentNodes&& from) : _noParentNodes_(std::move(from._noParentNodes_)) {
    GUM_CONS_MOV(StructuralConstraintNoParentNodes);
  }

  /// destructor
  StructuralConstraintNoParentNodes::~StructuralConstraintNoParentNodes() {
    GUM_DESTRUCTOR(StructuralConstraintNoParentNodes);
  }

  /// copy operator
  StructuralConstraintNoParentNodes&
      StructuralConstraintNoParentNodes::operator=(const StructuralConstraintNoParentNodes& from) {
    if (this != &from) { _noParentNodes_ = from._noParentNodes_; }
    return *this;
  }

  /// move operator
  StructuralConstraintNoParentNodes&
      StructuralConstraintNoParentNodes::operator=(StructuralConstraintNoParentNodes&& from) {
    if (this != &from) { _noParentNodes_ = std::move(from._noParentNodes_); }
    return *this;
  }

}   // namespace gum::learning
