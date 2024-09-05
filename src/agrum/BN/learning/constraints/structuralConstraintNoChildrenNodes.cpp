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

#include <agrum/BN/learning/constraints/structuralConstraintNoChildrenNodes.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintNoChildrenNodes_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum::learning {

  /// default constructor
  StructuralConstraintNoChildrenNodes::StructuralConstraintNoChildrenNodes() {
    GUM_CONSTRUCTOR(StructuralConstraintNoChildrenNodes);
  }

  /// constructor starting with a given graph
  StructuralConstraintNoChildrenNodes::StructuralConstraintNoChildrenNodes(const DiGraph& graph) {
    setGraph(graph);
    GUM_CONSTRUCTOR(StructuralConstraintNoChildrenNodes);
  }

  /// copy constructor
  StructuralConstraintNoChildrenNodes::StructuralConstraintNoChildrenNodes(
      const StructuralConstraintNoChildrenNodes& from) : _noChildrenNodes_(from._noChildrenNodes_) {
    GUM_CONS_CPY(StructuralConstraintNoChildrenNodes);
  }

  /// move constructor
  StructuralConstraintNoChildrenNodes::StructuralConstraintNoChildrenNodes(
      StructuralConstraintNoChildrenNodes&& from) :
      _noChildrenNodes_(std::move(from._noChildrenNodes_)) {
    GUM_CONS_MOV(StructuralConstraintNoChildrenNodes);
  }

  /// destructor
  StructuralConstraintNoChildrenNodes::~StructuralConstraintNoChildrenNodes() {
    GUM_DESTRUCTOR(StructuralConstraintNoChildrenNodes);
  }

  /// copy operator
  StructuralConstraintNoChildrenNodes& StructuralConstraintNoChildrenNodes::operator=(
      const StructuralConstraintNoChildrenNodes& from) {
    if (this != &from) { _noChildrenNodes_ = from._noChildrenNodes_; }
    return *this;
  }

  /// move operator
  StructuralConstraintNoChildrenNodes&
      StructuralConstraintNoChildrenNodes::operator=(StructuralConstraintNoChildrenNodes&& from) {
    if (this != &from) { _noChildrenNodes_ = std::move(from._noChildrenNodes_); }
    return *this;
  }

}   // namespace gum::learning
