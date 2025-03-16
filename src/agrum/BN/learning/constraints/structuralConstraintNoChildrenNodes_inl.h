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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/




#include <agrum/BN/learning/constraints/structuralConstraintNoChildrenNodes.h>

/** @file
 * @brief the structural constraint for forbidding the creation of some arcs
 * during structure learning
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintNoChildrenNodes::setGraphAlone(const DiGraph& graph) {}

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintNoChildrenNodes::checkArcAdditionAlone(NodeId x,
                                                                           NodeId y) const {
      return !_noChildrenNodes_.exists(x);
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintNoChildrenNodes::checkArcDeletionAlone(NodeId x,
                                                                           NodeId y) const {
      return true;
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintNoChildrenNodes::checkArcReversalAlone(NodeId x,
                                                                           NodeId y) const {
      return !_noChildrenNodes_.exists(y);
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool StructuralConstraintNoChildrenNodes::checkModificationAlone(
        const ArcAddition& change) const {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool StructuralConstraintNoChildrenNodes::checkModificationAlone(
        const ArcDeletion& change) const {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool StructuralConstraintNoChildrenNodes::checkModificationAlone(
        const ArcReversal& change) const {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool StructuralConstraintNoChildrenNodes::checkModificationAlone(
        const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION :
          return checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION :
          return checkArcDeletionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_REVERSAL :
          return checkArcReversalAlone(change.node1(), change.node2());

        default :
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not "
                    "supported by StructuralConstraintNoChildrenNodes");
      }
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintNoChildrenNodes::modifyGraphAlone(const ArcAddition& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintNoChildrenNodes::modifyGraphAlone(const ArcDeletion& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintNoChildrenNodes::modifyGraphAlone(const ArcReversal& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintNoChildrenNodes::modifyGraphAlone(const GraphChange& change) {}

    /// indicates whether a change will always violate the constraint
    INLINE bool
        StructuralConstraintNoChildrenNodes::isAlwaysInvalidAlone(const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION :
          return !checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION :
          return checkArcDeletionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_REVERSAL :
          return !checkArcAdditionAlone(change.node2(), change.node1());

        default :
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not supported "
                    "by StructuralConstraintNoChildrenNodes");
      }
    }

    /// assign a set of forbidden arcs
    INLINE void StructuralConstraintNoChildrenNodes::setNodes(const NodeSet& set) {
      _noChildrenNodes_ = set;
    }

    /// assign a new forbidden arc
    INLINE void StructuralConstraintNoChildrenNodes::addNode(NodeId node) {
      _noChildrenNodes_.insert(node);
    }

    /// remove a forbidden arc
    INLINE void StructuralConstraintNoChildrenNodes::eraseNode(NodeId node) {
      _noChildrenNodes_.erase(node);
    }

    /// returns the set of mandatory arcs
    INLINE const NodeSet& StructuralConstraintNoChildrenNodes::nodes() const {
      return _noChildrenNodes_;
    }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintNoChildrenNodes
#  include <agrum/BN/learning/constraints/structuralConstraintPatternRootInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
