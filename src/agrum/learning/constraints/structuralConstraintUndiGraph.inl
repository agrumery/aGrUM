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
 * @brief the base class for structural constraints used by learning algorithms
 * that learn an undirected graph structure
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets a new graph from which we will perform checkings
    INLINE void
    StructuralConstraintUndiGraph::setGraphAlone(const UndiGraph &graph) {
      _UndiGraph__graph = graph;
    }

    /// checks whether the constraints enable to add edge (x,y)
    INLINE bool StructuralConstraintUndiGraph::checkEdgeAdditionAlone(NodeId x,
                                                                      NodeId y) const
        noexcept {
      return _UndiGraph__graph.existsNode(x) && _UndiGraph__graph.existsNode(y) &&
             !_UndiGraph__graph.existsEdge(x, y);
    }

    /// checks whether the constraints enable to remove edge (x,y)
    INLINE bool StructuralConstraintUndiGraph::checkEdgeDeletionAlone(NodeId x,
                                                                      NodeId y) const
        noexcept {
      return _UndiGraph__graph.existsEdge(x, y);
    }

    /// checks whether the constraints enable to add an edge
    INLINE bool StructuralConstraintUndiGraph::checkModificationAlone(
        const EdgeAddition &change) const noexcept {
      return checkEdgeAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an edge
    INLINE bool StructuralConstraintUndiGraph::checkModificationAlone(
        const EdgeDeletion &change) const noexcept {
      return checkEdgeDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool StructuralConstraintUndiGraph::checkModificationAlone(
        const GraphChange &change) const noexcept {
      switch (change.type()) {
        case GraphChangeType::EDGE_ADDITION:
          return checkEdgeAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::EDGE_DELETION:
          return checkEdgeDeletionAlone(change.node1(), change.node2());

        default:
          GUM_ERROR(OperationNotAllowed,
                    "arc modifications are not "
                    "supported by StructuralConstraintUndiGraph");
      }
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintUndiGraph::modifyGraphAlone(const EdgeAddition &change) {
      _UndiGraph__graph.addEdge(change.node1(), change.node2());
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintUndiGraph::modifyGraphAlone(const EdgeDeletion &change) {
      _UndiGraph__graph.eraseEdge(Edge(change.node1(), change.node2()));
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintUndiGraph::modifyGraphAlone(const GraphChange &change) {
      switch (change.type()) {
        case GraphChangeType::EDGE_ADDITION:
          modifyGraphAlone(reinterpret_cast<const EdgeAddition &>(change));
          break;

        case GraphChangeType::EDGE_DELETION:
          modifyGraphAlone(reinterpret_cast<const EdgeDeletion &>(change));
          break;

        default:
          GUM_ERROR(OperationNotAllowed, "arc modifications are not supported "
                                         "by StructuralConstraintUndiGraph");
      }
    }

    /// indicates whether a change will always violate the constraint
    INLINE bool
    StructuralConstraintUndiGraph::isAlwaysInvalidAlone(const GraphChange &) const
        noexcept {
      return false;
    }

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintUndiGraph::setGraph(unsigned int nb_nodes) {
      _UndiGraph__graph.clear();
      for (unsigned int i = 0; i < nb_nodes; ++i) {
        _UndiGraph__graph.addNode(i);
      }
    }

// include all the methods applicable to the whole class hierarchy
#define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintUndiGraph
#include <agrum/learning/constraints/structuralConstraintPattern4UndiGraphRootInline.h>
#undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
