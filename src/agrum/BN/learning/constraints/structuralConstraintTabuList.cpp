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
 * @brief the class imposing a N-sized tabu list as a structural constraints for
 * learning algorithms
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintTabuList.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintTabuList_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintTabuList::StructuralConstraintTabuList() :
        _TabuList_changes_(GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE) {
      // put dummy elements into the list (this avoids having tests to do
      // afterwards)
      for (NodeId i = 0; i < GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE; ++i) {
        _TabuList_changes_.insert(ArcAddition(std::numeric_limits< NodeId >::max() - i,
                                              std::numeric_limits< NodeId >::max()),
                                  i);
      }

      GUM_CONSTRUCTOR(StructuralConstraintTabuList);
    }

    /// constructor starting with a given graph
    StructuralConstraintTabuList::StructuralConstraintTabuList(const DiGraph& graph) :
        _TabuList_changes_(GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE) {
      // put dummy elements into the list (this avoids having tests to do
      // afterwards)
      for (NodeId i = 0; i < GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE; ++i) {
        _TabuList_changes_.insert(ArcAddition(std::numeric_limits< NodeId >::max() - i,
                                              std::numeric_limits< NodeId >::max()),
                                  i);
      }

      GUM_CONSTRUCTOR(StructuralConstraintTabuList);
    }

    /// copy constructor
    StructuralConstraintTabuList::StructuralConstraintTabuList(
       const StructuralConstraintTabuList& from) :
        _TabuList_changes_(from._TabuList_changes_),
        _TabuList_offset_(from._TabuList_offset_) {
      GUM_CONS_CPY(StructuralConstraintTabuList);
    }

    /// move constructor
    StructuralConstraintTabuList::StructuralConstraintTabuList(
       StructuralConstraintTabuList&& from) :
        _TabuList_changes_(std::move(from._TabuList_changes_)),
        _TabuList_offset_(std::move(from._TabuList_offset_)) {
      GUM_CONS_MOV(StructuralConstraintTabuList);
    }

    /// destructor
    StructuralConstraintTabuList::~StructuralConstraintTabuList() {
      GUM_DESTRUCTOR(StructuralConstraintTabuList);
    }

    /// copy operator
    StructuralConstraintTabuList&
       StructuralConstraintTabuList::operator=(const StructuralConstraintTabuList& from) {
      if (this != &from) {
        _TabuList_changes_ = from._TabuList_changes_;
        _TabuList_offset_  = from._TabuList_offset_;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintTabuList&
       StructuralConstraintTabuList::operator=(StructuralConstraintTabuList&& from) {
      if (this != &from) {
        _TabuList_changes_ = std::move(from._TabuList_changes_);
        _TabuList_offset_  = std::move(from._TabuList_offset_);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
