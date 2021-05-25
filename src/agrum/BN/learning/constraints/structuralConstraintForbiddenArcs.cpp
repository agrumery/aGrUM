/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintForbiddenArcs.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintForbiddenArcs_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintForbiddenArcs::StructuralConstraintForbiddenArcs() {
      GUM_CONSTRUCTOR(StructuralConstraintForbiddenArcs);
    }

    /// constructor starting with a given graph
    StructuralConstraintForbiddenArcs::StructuralConstraintForbiddenArcs(
       const DiGraph& graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintForbiddenArcs);
    }

    /// copy constructor
    StructuralConstraintForbiddenArcs::StructuralConstraintForbiddenArcs(
       const StructuralConstraintForbiddenArcs& from) :
         _ForbiddenArcs_forbidden_arcs_(from. _ForbiddenArcs_forbidden_arcs_) {
      GUM_CONS_CPY(StructuralConstraintForbiddenArcs);
    }

    /// move constructor
    StructuralConstraintForbiddenArcs::StructuralConstraintForbiddenArcs(
       StructuralConstraintForbiddenArcs&& from) :
         _ForbiddenArcs_forbidden_arcs_(
           std::move(from. _ForbiddenArcs_forbidden_arcs_)) {
      GUM_CONS_MOV(StructuralConstraintForbiddenArcs);
    }

    /// destructor
    StructuralConstraintForbiddenArcs::~StructuralConstraintForbiddenArcs() {
      GUM_DESTRUCTOR(StructuralConstraintForbiddenArcs);
    }

    /// copy operator
    StructuralConstraintForbiddenArcs&
       StructuralConstraintForbiddenArcs::operator=(
          const StructuralConstraintForbiddenArcs& from) {
      if (this != &from) {
         _ForbiddenArcs_forbidden_arcs_ = from. _ForbiddenArcs_forbidden_arcs_;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintForbiddenArcs&
       StructuralConstraintForbiddenArcs::operator=(
          StructuralConstraintForbiddenArcs&& from) {
      if (this != &from) {
         _ForbiddenArcs_forbidden_arcs_
           = std::move(from. _ForbiddenArcs_forbidden_arcs_);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
