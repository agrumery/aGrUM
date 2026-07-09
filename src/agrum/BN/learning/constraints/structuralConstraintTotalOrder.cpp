/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief the structural constraint imposing a total order over nodes
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintTotalOrder.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintTotalOrder_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintTotalOrder::StructuralConstraintTotalOrder() {
      GUM_CONSTRUCTOR(StructuralConstraintTotalOrder);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintTotalOrder::StructuralConstraintTotalOrder(
        const Sequence< NodeId >& order) : _total_order_(order) {
      GUM_CONSTRUCTOR(StructuralConstraintTotalOrder);
    }

    /// constructor starting with a given graph
    StructuralConstraintTotalOrder::StructuralConstraintTotalOrder(
        const DiGraph&            graph,
        const Sequence< NodeId >& order) : _total_order_(order) {
      GUM_CONSTRUCTOR(StructuralConstraintTotalOrder);
    }

    /// copy constructor
    StructuralConstraintTotalOrder::StructuralConstraintTotalOrder(
        const StructuralConstraintTotalOrder& from) : _total_order_(from._total_order_) {
      GUM_CONS_CPY(StructuralConstraintTotalOrder);
    }

    /// move constructor
    StructuralConstraintTotalOrder::StructuralConstraintTotalOrder(
        StructuralConstraintTotalOrder&& from) : _total_order_(std::move(from._total_order_)) {
      GUM_CONS_MOV(StructuralConstraintTotalOrder);
    }

    /// destructor
    StructuralConstraintTotalOrder::~StructuralConstraintTotalOrder() {
      GUM_DESTRUCTOR(StructuralConstraintTotalOrder);
    }

    /// copy operator
    StructuralConstraintTotalOrder&
        StructuralConstraintTotalOrder::operator=(const StructuralConstraintTotalOrder& from) {
      if (this != &from) { _total_order_ = from._total_order_; }
      return *this;
    }

    /// move operator
    StructuralConstraintTotalOrder&
        StructuralConstraintTotalOrder::operator=(StructuralConstraintTotalOrder&& from) {
      if (this != &from) { _total_order_ = std::move(from._total_order_); }
      return *this;
    }


    bool
        StructuralConstraintTotalOrder::isAlwaysInvalidAlone(const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION :
          return !checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION : return false;

        case GraphChangeType::ARC_REVERSAL :
          return !checkArcAdditionAlone(change.node2(), change.node1());

        case GraphChangeType::ARC_TRIANGLE_DELETION1 :
          return !checkArcTriangleDeletion1Alone(change.node1(), change.node2(), change.node3());

        case GraphChangeType::ARC_TRIANGLE_DELETION2 :
          return !checkArcTriangleDeletion2Alone(change.node1(), change.node2(), change.node3());

        default :
          GUM_ERROR(OperationNotAllowed,
                    "Graph change operation "
                        << change.typeAsString()
                        << " is not supported by the Total Order structural constraint");
      }
    }

    bool
        StructuralConstraintTotalOrder::checkModificationAlone(const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION :
          return checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION :
          return checkArcDeletionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_REVERSAL :
          return checkArcReversalAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_TRIANGLE_DELETION1 :
          return checkArcTriangleDeletion1Alone(change.node1(), change.node2(), change.node3());

        case GraphChangeType::ARC_TRIANGLE_DELETION2 :
          return checkArcTriangleDeletion2Alone(change.node1(), change.node2(), change.node3());

        default :
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not "
                    "supported by the Total Order structural constraint");
      }
    }
  } /* namespace learning */

} /* namespace gum */
