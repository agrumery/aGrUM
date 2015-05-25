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

// this file should be included at the end of all the StructuralConstraints header
// files (see StructuralConstraintDAG to see how it should be included).

#ifdef GUM_CONSTRAINT_CLASS_NAME

// ##########################################################################
/// @name Accessors / Modifiers
// ##########################################################################
/// @{

/// sets a new graph from which we will perform checkings
void setGraph(const DiGraph &graph);

/// notify the constraint of a modification of the graph
/** @warning If an already existing arc is added nothing is done. In
 * particular, no exception is raised. */
void modifyGraph(const ArcAddition &change);

/// notify the constraint of a modification of the graph
/** @warning If a nonexisting arc is removed, nothing is done. In
 * particular, no exception is raised. */
void modifyGraph(const ArcDeletion &change);

/// notify the constraint of a modification of the graph
/** @warning If an already existing arc is added, or if a nonexisting arc
 * is removed, nothing is done. In particular, no exception is raised. */
void modifyGraph(const ArcReversal &change);

/// notify the constraint of a modification of the graph
/** @warning If an already existing arc is added, or if a nonexisting arc
 * is removed, nothing is done. In particular, no exception is raised. */
void modifyGraph(const GraphChange &change);

/// indicates whether a change will always violate the constraint
/** Some learning algorithms need examine several times whether a given
 * graph change can be applied. For instance, the first time arc (X,Y)
 * addition is considered, the learning algorithm may discard this change
 * because it violates the structural constraint (e.g., if the latter
 * enforces a DAG structure, this arc addition might induce a directed
 * cycle), but, later on, other arc removal may induce that the arc addition
 * is now possible. Such change is thus not always invalid. Conversely,
 * there are changes that can be discarded once and for all. For instance,
 * in a 2TBN structure, it is always impossible to add a backward-time arc.
 * Such graph changes are always invalid and are therefore tagged as such
 * by the isAlwaysInvalid method. */
bool isAlwaysInvalid(const GraphChange &change) const noexcept;

/// checks whether the constraints enable to add arc (x,y)
bool checkArcAddition(NodeId x, NodeId y) const noexcept;

/// checks whether the constraints enable to remove arc (x,y)
bool checkArcDeletion(NodeId x, NodeId y) const noexcept;

/// checks whether the constraints enable to reverse arc (x,y)
bool checkArcReversal(NodeId x, NodeId y) const noexcept;

/// checks whether the constraints enable to add an arc
bool checkModification(const ArcAddition &change) const noexcept;

/// checks whether the constraints enable to remove an arc
bool checkModification(const ArcDeletion &change) const noexcept;

/// checks whether the constraints enable to reverse an arc
bool checkModification(const ArcReversal &change) const noexcept;

/// checks whether the constraints enable to perform a graph change
bool checkModification(const GraphChange &change) const noexcept;

/// @}

#endif /* GUM_CONSTRAINT_CLASS_NAME */
