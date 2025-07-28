/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief the class imposing a N-sized tabu list as a structural constraints for
 * learning algorithms
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
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
        _TabuList_changes_(from._TabuList_changes_), _TabuList_offset_(from._TabuList_offset_) {
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
