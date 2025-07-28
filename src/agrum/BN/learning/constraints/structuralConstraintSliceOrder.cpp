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
 * @brief the structural constraint imposing a partial order over nodes
 *
 * In DBNs, it is forbidden to add arcs from nodes at time slice t to nodes at
 * time slice s, where s < t. This class allows for taking this kind of
 *constraint
 * into account by imposing a partial order over the nodes: arcs (X,Y) can then
 * only be added if X <= Y in the partial order.
 * @warning: there may exist free variables, that is variables whose order
 * w.r.t. the other variables is unspecified. In this case, arcs adjacent
 * to them can be constructed. The partial order is specified by assigning
 * numbers to nodes (through a NodeProperty). Nodes without number (i.e., that
 * do not belong to the property) are free.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintSliceOrder.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintSliceOrder_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintSliceOrder::StructuralConstraintSliceOrder() {
      GUM_CONSTRUCTOR(StructuralConstraintSliceOrder);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintSliceOrder::StructuralConstraintSliceOrder(
        const NodeProperty< NodeId >& order) {
      for (auto iter = order.cbegin(); iter != order.cend(); ++iter) {
        _DiGraph_graph_.addNodeWithId(iter.key());
      }
      _SliceOrder_order_ = order;

      GUM_CONSTRUCTOR(StructuralConstraintSliceOrder);
    }

    /// constructor starting with a given graph
    StructuralConstraintSliceOrder::StructuralConstraintSliceOrder(
        const DiGraph&                graph,
        const NodeProperty< NodeId >& order) :
        StructuralConstraintDiGraph(graph), _SliceOrder_order_(order) {
      GUM_CONSTRUCTOR(StructuralConstraintSliceOrder);
    }

    /// copy constructor
    StructuralConstraintSliceOrder::StructuralConstraintSliceOrder(
        const StructuralConstraintSliceOrder& from) :
        StructuralConstraintDiGraph(from), _SliceOrder_order_(from._SliceOrder_order_) {
      GUM_CONS_CPY(StructuralConstraintSliceOrder);
    }

    /// move constructor
    StructuralConstraintSliceOrder::StructuralConstraintSliceOrder(
        StructuralConstraintSliceOrder&& from) :
        StructuralConstraintDiGraph(std::move(from)),
        _SliceOrder_order_(std::move(from._SliceOrder_order_)) {
      GUM_CONS_MOV(StructuralConstraintSliceOrder);
    }

    /// destructor
    StructuralConstraintSliceOrder::~StructuralConstraintSliceOrder() {
      GUM_DESTRUCTOR(StructuralConstraintSliceOrder);
    }

    /// copy operator
    StructuralConstraintSliceOrder&
        StructuralConstraintSliceOrder::operator=(const StructuralConstraintSliceOrder& from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(from);
        _SliceOrder_order_ = from._SliceOrder_order_;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintSliceOrder&
        StructuralConstraintSliceOrder::operator=(StructuralConstraintSliceOrder&& from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(std::move(from));
        _SliceOrder_order_ = std::move(from._SliceOrder_order_);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
