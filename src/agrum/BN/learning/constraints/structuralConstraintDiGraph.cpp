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
 * @brief the base class for structural constraints used by learning algorithms
 * that learn a directed graph structure
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintDiGraph.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintDiGraph_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintDiGraph::StructuralConstraintDiGraph() {
      GUM_CONSTRUCTOR(StructuralConstraintDiGraph);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintDiGraph::StructuralConstraintDiGraph(Size nb_nodes) {
      setGraph(nb_nodes);
      GUM_CONSTRUCTOR(StructuralConstraintDiGraph);
    }

    /// constructor starting with a given graph
    StructuralConstraintDiGraph::StructuralConstraintDiGraph(const DiGraph& graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintDiGraph);
    }

    /// copy constructor
    StructuralConstraintDiGraph::StructuralConstraintDiGraph(
        const StructuralConstraintDiGraph& from) : _graph_(from._graph_) {
      GUM_CONS_CPY(StructuralConstraintDiGraph);
    }

    /// move constructor
    StructuralConstraintDiGraph::StructuralConstraintDiGraph(
        StructuralConstraintDiGraph&& from) noexcept : _graph_(std::move(from._graph_)) {
      GUM_CONS_MOV(StructuralConstraintDiGraph);
    }

    /// destructor
    StructuralConstraintDiGraph::~StructuralConstraintDiGraph() {
      GUM_DESTRUCTOR(StructuralConstraintDiGraph);
    }

    /// copy operator
    StructuralConstraintDiGraph&
        StructuralConstraintDiGraph::operator=(const StructuralConstraintDiGraph& from) {
      if (this != &from) { _graph_ = from._graph_; }
      return *this;
    }

    /// move operator
    StructuralConstraintDiGraph&
        StructuralConstraintDiGraph::operator=(StructuralConstraintDiGraph&& from) {
      if (this != &from) { _graph_ = std::move(from._graph_); }
      return *this;
    }


  bool StructuralConstraintDiGraph::checkModificationAlone(const GraphChange& change) const {
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
                  "Graph change operation "
                      << change.typeAsString()
                      << " is not supported by the DiGraph structural constraint");
    }
  }

  void StructuralConstraintDiGraph::modifyGraphAlone(const GraphChange& change) {
    switch (change.type()) {
      case GraphChangeType::ARC_ADDITION :
        modifyGraphAlone(reinterpret_cast< const ArcAddition& >(change));
        break;

      case GraphChangeType::ARC_DELETION :
        modifyGraphAlone(reinterpret_cast< const ArcDeletion& >(change));
        break;

      case GraphChangeType::ARC_REVERSAL :
        modifyGraphAlone(reinterpret_cast< const ArcReversal& >(change));
        break;

      case GraphChangeType::ARC_TRIANGLE_DELETION1 :
        modifyGraphAlone(reinterpret_cast< const ArcTriangleDeletion1& >(change));
        break;

      case GraphChangeType::ARC_TRIANGLE_DELETION2 :
        modifyGraphAlone(reinterpret_cast< const ArcTriangleDeletion2& >(change));
        break;

      default :
        GUM_ERROR(OperationNotAllowed,
                  "Graph change operation "
                      << change.typeAsString()
                      << " is not supported by the DiGraph structural constraint")
    }
  }
  } /* namespace learning */

} /* namespace gum */
