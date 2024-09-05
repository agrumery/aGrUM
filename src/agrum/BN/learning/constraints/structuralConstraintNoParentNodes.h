/**
*
*   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
* @brief the structural constraint for forbidding parents for some nodes
* during structure learning
*
* @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
*/
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_NO_PARENT_NODES_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_NO_PARENT_NODES_H

#include <agrum/agrum.h>

#include <agrum/BN/learning/constraints/structuralConstraint.h>



 namespace gum::learning {

   /** @class StructuralConstraintNoParentNodes
    * @brief the structural constraint for forbidding parents for some nodes
    * @ingroup learning_group
    */
   class StructuralConstraintNoParentNodes: public virtual StructuralConstraintEmpty {
     public:
     // ##########################################################################
     /// @name Constructors / Destructors
     // ##########################################################################
     /// @{

     /// default constructor
     StructuralConstraintNoParentNodes();

     /// constructor starting with a given graph
     StructuralConstraintNoParentNodes(const DiGraph& graph);

     /// copy constructor
     StructuralConstraintNoParentNodes(const StructuralConstraintNoParentNodes& from);

     /// move constructor
     StructuralConstraintNoParentNodes(StructuralConstraintNoParentNodes&& from);

     /// destructor
     virtual ~StructuralConstraintNoParentNodes();

     /// @}

     // ##########################################################################
     /// @name Operators
     // ##########################################################################
     /// @{

     /// copy operator
     StructuralConstraintNoParentNodes& operator=(const StructuralConstraintNoParentNodes& from);

     /// move operator
     StructuralConstraintNoParentNodes& operator=(StructuralConstraintNoParentNodes&& from);

     /// @}

     // ##########################################################################
     /// @name Specific Accessors / Modifiers
     // ##########################################################################
     /// @{

     /// assign a set of forbidden arcs
     void setNodes(const NodeSet& set);

     /// assign a new forbidden arc
     void addNode(NodeId node);

     /// remove a forbidden arc
     void eraseNode(NodeId node);

     /// returns the set of mandatory arcs
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
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintNoParentNodes
#  include <agrum/BN/learning/constraints/structuralConstraintPatternHeader.h>
#  undef GUM_CONSTRAINT_CLASS_NAME
#endif   // DOXYGEN_SHOULD_SKIP_THIS

     protected:
     /// the ForbiddenArcs on which we perform checks
     NodeSet _noParentNodes_;
   };

 } // namespace gum::learning



/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintNoParentNodes_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_NO_PARENT_NODES_H */
