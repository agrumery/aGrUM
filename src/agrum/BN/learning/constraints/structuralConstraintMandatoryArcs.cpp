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
 * @brief the structural constraint indicating that some arcs shall never be
 * removed or reversed
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintMandatoryArcs.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintMandatoryArcs_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs() {
      GUM_CONSTRUCTOR(StructuralConstraintMandatoryArcs);
    }

    /// constructor starting with a given graph
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs(const DiGraph& graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintMandatoryArcs);
    }

    /// copy constructor
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs(
       const StructuralConstraintMandatoryArcs& from) :
        _MandatoryArcs_mandatory_arcs_(from._MandatoryArcs_mandatory_arcs_) {
      GUM_CONS_CPY(StructuralConstraintMandatoryArcs);
    }

    /// move constructor
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs(
       StructuralConstraintMandatoryArcs&& from) :
        _MandatoryArcs_mandatory_arcs_(std::move(from._MandatoryArcs_mandatory_arcs_)) {
      GUM_CONS_MOV(StructuralConstraintMandatoryArcs);
    }

    /// destructor
    StructuralConstraintMandatoryArcs::~StructuralConstraintMandatoryArcs() {
      GUM_DESTRUCTOR(StructuralConstraintMandatoryArcs);
    }

    /// copy operator
    StructuralConstraintMandatoryArcs&
       StructuralConstraintMandatoryArcs::operator=(const StructuralConstraintMandatoryArcs& from) {
      if (this != &from) { _MandatoryArcs_mandatory_arcs_ = from._MandatoryArcs_mandatory_arcs_; }
      return *this;
    }

    /// move operator
    StructuralConstraintMandatoryArcs&
       StructuralConstraintMandatoryArcs::operator=(StructuralConstraintMandatoryArcs&& from) {
      if (this != &from) {
        _MandatoryArcs_mandatory_arcs_ = std::move(from._MandatoryArcs_mandatory_arcs_);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
