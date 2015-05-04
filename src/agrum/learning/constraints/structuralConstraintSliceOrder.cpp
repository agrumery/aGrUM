/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief the structural constraint imposing a partial order over nodes
 *
 * In DBNs, it is forbidden to add arcs from nodes at time slice t to nodes at
 * time slice s, where s < t. This class allows for taking this kind of constraint
 * into account by imposing a partial order over the nodes: arcs (X,Y) can then
 * only be added if X <= Y in the partial order.
 * @warning: there may exist free variables, that is variables whose order
 * w.r.t. the other variables is unspecified. In this case, arcs adjacent
 * to them can be constructed. The partial order is specified by assigning
 * numbers to nodes (through a NodeProperty). Nodes without number (i.e., that
 * do not belong to the property) are free.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/constraints/structuralConstraintSliceOrder.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/constraints/structuralConstraintSliceOrder.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintSliceOrder::StructuralConstraintSliceOrder() {
      GUM_CONSTRUCTOR(StructuralConstraintSliceOrder);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintSliceOrder::StructuralConstraintSliceOrder(
        const NodeProperty<unsigned int> &order) {
      for (auto iter = order.cbegin(); iter != order.cend(); ++iter) {
        _DiGraph__graph.addNode(iter.key());
      }
      _SliceOrder__order = order;

      GUM_CONSTRUCTOR(StructuralConstraintSliceOrder);
    }

    /// constructor starting with a given graph
    StructuralConstraintSliceOrder::StructuralConstraintSliceOrder(
        const DiGraph &graph, const NodeProperty<unsigned int> &order)
        : StructuralConstraintDiGraph(graph), _SliceOrder__order(order) {
      GUM_CONSTRUCTOR(StructuralConstraintSliceOrder);
    }

    /// copy constructor
    StructuralConstraintSliceOrder::StructuralConstraintSliceOrder(
        const StructuralConstraintSliceOrder &from)
        : StructuralConstraintDiGraph(from),
          _SliceOrder__order(from._SliceOrder__order) {
      GUM_CONS_CPY(StructuralConstraintSliceOrder);
    }

    /// move constructor
    StructuralConstraintSliceOrder::StructuralConstraintSliceOrder(
        StructuralConstraintSliceOrder &&from)
        : StructuralConstraintDiGraph(std::move(from)),
          _SliceOrder__order(std::move(from._SliceOrder__order)) {
      GUM_CONS_MOV(StructuralConstraintSliceOrder);
    }

    /// destructor
    StructuralConstraintSliceOrder::~StructuralConstraintSliceOrder() {
      GUM_DESTRUCTOR(StructuralConstraintSliceOrder);
    }

    /// copy operator
    StructuralConstraintSliceOrder &StructuralConstraintSliceOrder::
    operator=(const StructuralConstraintSliceOrder &from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(from);
        _SliceOrder__order = from._SliceOrder__order;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintSliceOrder &StructuralConstraintSliceOrder::
    operator=(StructuralConstraintSliceOrder &&from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(std::move(from));
        _SliceOrder__order = std::move(from._SliceOrder__order);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
