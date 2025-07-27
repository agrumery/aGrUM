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
 * @brief the structural constraint for forbidding children for some nodes
 * during structure learning
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_NO_CHILDREN_NODES_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_NO_CHILDREN_NODES_H

#include <agrum/agrum.h>

#include <agrum/BN/learning/constraints/structuralConstraint.h>

namespace gum::learning {

  /** @class StructuralConstraintNoChildrenNodes
   * @brief the structural constraint for forbidding children for some nodes
   * @ingroup learning_group
   */
  class StructuralConstraintNoChildrenNodes: public virtual StructuralConstraintEmpty {
    public:
    // ##########################################################################
    /// @name Constructors / Destructors
    // ##########################################################################
    /// @{

    /// default constructor
    StructuralConstraintNoChildrenNodes();

    /// constructor starting with a given graph
    StructuralConstraintNoChildrenNodes(const DiGraph& graph);

    /// copy constructor
    StructuralConstraintNoChildrenNodes(const StructuralConstraintNoChildrenNodes& from);

    /// move constructor
    StructuralConstraintNoChildrenNodes(StructuralConstraintNoChildrenNodes&& from);

    /// destructor
    virtual ~StructuralConstraintNoChildrenNodes();

    /// @}

    // ##########################################################################
    /// @name Operators
    // ##########################################################################
    /// @{

    /// copy operator
    StructuralConstraintNoChildrenNodes& operator=(const StructuralConstraintNoChildrenNodes& from);

    /// move operator
    StructuralConstraintNoChildrenNodes& operator=(StructuralConstraintNoChildrenNodes&& from);

    /// @}

    // ##########################################################################
    /// @name Specific Accessors / Modifiers
    // ##########################################################################
    /// @{

    /// assign a set of nodes without children
    void setNodes(const NodeSet& set);

    /// assign a new node without children
    void addNode(NodeId node);

    /// remove a forbidden w node without children
    void eraseNode(NodeId node);

    /// returns the set of w nodes without children
    const NodeSet& nodes() const;

    /// sets a new graph from which we will perform checkings
    void setGraphAlone(const DiGraph& graph);

    /// notify the constraint of a modification of the graph
    void modifyGraphAlone(const ArcAddition& change);

    /// notify the constraint of a modification of the graph
    void modifyGraphAlone(const ArcDeletion& change);

    /// notify the constraint of a modification of the graph
    void modifyGraphAlone(const ArcReversal& change);

    /// notify the constraint of a modification of the graph
    void modifyGraphAlone(const GraphChange& change);

    /// indicates whether a change will always violate the constraint
    /** Some learning algorithms need examine several times whether a given
     * graph change can be applied. For instance, the first time arc (X,Y)
     * addition is considered, the learning algorithm may discard this change
     * because it violates the structural constraint (e.g., if the latter
     * enforces a DAG structure, this arc addition might induce a directed
     * cycle), but, later on, other arc removal may induce that the arc
     * addition is now possible.
     * Such change is thus not always invalid. Conversely,
     * there are changes that can be discarded once and for all. For instance,
     * in a 2TBN structure, it is always impossible to add a backward-time
     * arc.
     * Such graph changes are always invalid and are therefore tagged as such
     * by the isAlwaysInvalid method. */
    bool isAlwaysInvalidAlone(const GraphChange& change) const;

    /// checks whether the constraints enable to add arc (x,y)
    bool checkArcAdditionAlone(NodeId x, NodeId y) const;

    /// checks whether the constraints enable to remove arc (x,y)
    bool checkArcDeletionAlone(NodeId x, NodeId y) const;

    /// checks whether the constraints enable to reverse arc (x,y)
    bool checkArcReversalAlone(NodeId x, NodeId y) const;

    /// checks whether the constraints enable to perform a graph change
    bool checkModificationAlone(const GraphChange& change) const;

    /// checks whether the constraints enable to add an arc
    bool checkModificationAlone(const ArcAddition& change) const;

    /// checks whether the constraints enable to remove an arc
    bool checkModificationAlone(const ArcDeletion& change) const;

    /// checks whether the constraints enable to reverse an arc
    bool checkModificationAlone(const ArcReversal& change) const;

    /// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// include the set of methods that enable the structural constraint to
// be standalone, i.e., that it needs not be included into a
// StructuralConstraintSetStatic to be used by learning algorithms
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintNoChildrenNodes
#  include <agrum/BN/learning/constraints/structuralConstraintPatternHeader.h>
#  undef GUM_CONSTRAINT_CLASS_NAME
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    protected:
    /// the set of w nodes without children on which we perform checks
    NodeSet _noChildrenNodes_;
  };

}   // namespace gum::learning

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintNoChildrenNodes_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_NO_CHILDREN_NODES_H */
